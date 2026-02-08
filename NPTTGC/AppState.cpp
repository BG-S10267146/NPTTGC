#include "AppState.h"
#include "lib/CSVHelper.h"
#include "lib/Sort.h"
#include "utils/Date.h"
#include <cstdio>

AppState::AppState() : currentUserId(-1) {}

void AppState::loadData()
{
    loadMembers("members.csv");
    loadGames("games.csv");
    loadBorrows("borrows.csv");
    loadReviews("reviews.csv");
}

void AppState::saveData()
{
    saveToFile<int, Member>("members.csv", Member::csvHeader(), members, Member::toCSVRow);
    saveToFile<int, Game>("games.csv", Game::csvHeader(), games, Game::toCSVRow);
    saveToFile<int, Borrow>("borrows.csv", Borrow::csvHeader(), borrows, Borrow::toCSVRow);
    saveToFile<int, Review>("reviews.csv", Review::csvHeader(), reviews, Review::toCSVRow);
}

void AppState::loadMembers(const std::string &filename)
{
    auto buildMemberAndIndexUsername = [&](const Vector<std::string> &row)
    {
        Member m = Member::fromCSVRow(row);
        membersByUsername.insert(m.username, m.id);
        return m;
    };

    members = buildDictFromFile<int, Member>(
        filename,
        buildMemberAndIndexUsername,
        [](const Member &m)
        { return m.id; });

    if (members.isEmpty())
    {
        printf("Starting with default admin account.\n");
        Member admin(1, "admin", true);
        members.insert(admin.id, admin);
        membersByUsername.insert(admin.username, admin.id);
    }

    printf("Loaded %d members from %s\n", members.getSize(), filename.c_str());
}

void AppState::loadGames(const std::string &filename)
{
    games = buildDictFromFile<int, Game>(
        filename,
        Game::fromCSVRow,
        [](const Game &g)
        { return g.id; });
    rebuildGameNames();

    printf("Loaded %d games from %s\n", games.getSize(), filename.c_str());
}

void AppState::loadBorrows(const std::string &filename)
{
    auto buildBorrowAndMarkBorrowed = [&](const Vector<std::string> &row)
    {
        Borrow b = Borrow::fromCSVRow(row);
        Vector<int> borrowIds;
        if (borrowsByMember.exists(b.memberId))
        {
            borrowIds = borrowsByMember.get(b.memberId);
        }
        borrowIds.append(b.borrowId);
        borrowsByMember.insert(b.memberId, borrowIds);
        if (b.dateReturned.empty() || b.dateReturned == "N/A")
        {
            borrowedGames.insert(b.gameId);
        }
        return b;
    };

    borrows = buildDictFromFile<int, Borrow>(
        filename,
        buildBorrowAndMarkBorrowed,
        [](const Borrow &b)
        { return b.borrowId; });

    printf("Loaded %d borrow records from %s\n", borrows.getSize(), filename.c_str());
}

void AppState::loadReviews(const std::string &filename)
{
    auto buildReviewAndIndexByGame = [&](const Vector<std::string> &row)
    {
        Review r = Review::fromCSVRow(row);
        Vector<int> reviewIds;
        if (reviewsByGame.exists(r.gameId))
        {
            reviewIds = reviewsByGame.get(r.gameId);
        }
        reviewIds.append(r.reviewId);
        reviewsByGame.insert(r.gameId, reviewIds);
        return r;
    };

    reviews = buildDictFromFile<int, Review>(
        filename,
        buildReviewAndIndexByGame,
        [](const Review &r)
        { return r.reviewId; });

    printf("Loaded %d reviews from %s\n", reviews.getSize(), filename.c_str());
}

std::optional<Member> AppState::authenticateMember(const std::string &username)
{
    if (!membersByUsername.exists(username))
    {
        return std::nullopt;
    }

    int memberId = membersByUsername.get(username);

    if (members.exists(memberId))
    {
        currentUserId = memberId;
        return members.get(memberId);
    }

    return std::nullopt;
}

bool AppState::addMember(const std::string &username, bool isAdmin)
{
    if (membersByUsername.exists(username))
    {
        return false;
    }

    int memberId = members.maxKey() + 1;
    Member newMember(memberId, username, isAdmin);

    members.insert(memberId, newMember);
    membersByUsername.insert(username, memberId);

    saveToFile<int, Member>("members.csv", Member::csvHeader(), members, Member::toCSVRow);
    return true;
}

void AppState::logout()
{
    currentUserId = -1;
}

bool AppState::addGame(const Game &game)
{
    games.insert(game.id, game);
    rebuildGameNames();
    saveToFile<int, Game>("games.csv", Game::csvHeader(), games, Game::toCSVRow);
    return true;
}

bool AppState::removeGame(int gameId)
{
    if (borrowedGames.exists(gameId))
    {
        return false;
    }

    if (!games.exists(gameId))
    {
        return false;
    }

    Game game = games.get(gameId);
    game.isDeleted = true;
    games.insert(gameId, game);
    rebuildGameNames();
    saveToFile<int, Game>("games.csv", Game::csvHeader(), games, Game::toCSVRow);
    return true;
}

Vector<Game> AppState::getGamesForPlayerCount(int playerCount)
{
    Vector<Game> matchingGames;
    games.forEach([&](const int &id, const Game &game)
                  {
        if (!game.isDeleted && playerCount >= game.minPlayers && playerCount <= game.maxPlayers)
        {
            matchingGames.append(game);
        } });

    Sort::quicksort(
        matchingGames,
        [](const Game &a, const Game &b)
        { return a.name < b.name ? -1 : (a.name > b.name ? 1 : 0); });

    return matchingGames;
}

Vector<Game> AppState::searchGames(const std::string &query, std::function<bool(const Game &)> filter)
{
    Vector<int> ids = gameNames.search(query);
    Vector<Game> result;
    for (int i = 0; i < ids.getSize(); i++)
    {
        Game game = games.get(ids[i]);
        if (filter == nullptr || filter(game))
        {
            result.append(game);
        }
    }

    return result;
}

Game *AppState::getGameById(int gameId)
{
    if (!games.exists(gameId))
        return nullptr;

    static Game gameCache;
    gameCache = games.get(gameId);
    return &gameCache;
}

std::string AppState::getGameNameById(int gameId)
{
    if (games.exists(gameId))
    {
        return games.get(gameId).name;
    }
    return "Unknown Game";
}

bool AppState::borrowGame(int gameId)
{
    if (currentUserId == -1)
        return false;

    if (borrowedGames.exists(gameId))
        return false;

    Borrow newBorrow;
    newBorrow.borrowId = borrows.maxKey() + 1;
    newBorrow.memberId = currentUserId;
    newBorrow.gameId = gameId;
    newBorrow.dateBorrowed = getCurrentDateTime();
    newBorrow.dateReturned = "N/A";

    borrows.insert(newBorrow.borrowId, newBorrow);
    Vector<int> borrowIds;
    if (borrowsByMember.exists(currentUserId))
    {
        borrowIds = borrowsByMember.get(currentUserId);
    }
    borrowIds.append(newBorrow.borrowId);
    borrowsByMember.insert(currentUserId, borrowIds);
    borrowedGames.insert(gameId);

    saveToFile<int, Borrow>("borrows.csv", Borrow::csvHeader(), borrows, Borrow::toCSVRow);
    return true;
}

bool AppState::returnGame(int borrowId)
{
    if (currentUserId == -1)
        return false;

    if (!borrows.exists(borrowId))
        return false;

    Borrow borrow = borrows.get(borrowId);
    if (borrow.memberId != currentUserId)
        return false;

    borrow.dateReturned = getCurrentDateTime();
    borrows.insert(borrowId, borrow);
    borrowedGames.remove(borrow.gameId);

    saveToFile<int, Borrow>("borrows.csv", Borrow::csvHeader(), borrows, Borrow::toCSVRow);
    return true;
}

Vector<Borrow> AppState::getAllBorrows()
{
    Vector<Borrow> sortedBorrows = borrows.toVector();
    Sort::quicksort(sortedBorrows, [](const Borrow &b1, const Borrow &b2)
                    {
        int borrowCompare = compareDates(b1.dateBorrowed, b2.dateBorrowed);
        if (borrowCompare != 0)
            return borrowCompare;
        return compareDates(b1.dateReturned, b2.dateReturned); });
    return sortedBorrows;
}

Vector<Borrow> AppState::getMemberBorrows()
{
    if (currentUserId == -1)
        return Vector<Borrow>();

    Vector<Borrow> memberBorrows;
    if (!borrowsByMember.exists(currentUserId))
        return memberBorrows;

    Vector<int> borrowIds = borrowsByMember.get(currentUserId);
    for (int i = 0; i < borrowIds.getSize(); i++)
    {
        int borrowId = borrowIds.get(i);
        memberBorrows.append(borrows.get(borrowId));
    }

    Sort::quicksort(memberBorrows, [](const Borrow &b1, const Borrow &b2)
                    {
        int borrowCompare = compareDates(b1.dateBorrowed, b2.dateBorrowed);
        if (borrowCompare != 0)
            return borrowCompare;
        return compareDates(b1.dateReturned, b2.dateReturned); });
    return memberBorrows;
}

bool AppState::isGameBorrowed(int gameId)
{
    return borrowedGames.exists(gameId);
}

bool AppState::addReview(int gameId, int rating, const std::string &content)
{
    if (currentUserId == -1)
        return false;

    Review newReview;
    newReview.reviewId = reviews.maxKey() + 1;
    newReview.userId = currentUserId;
    newReview.gameId = gameId;
    newReview.rating = rating;
    newReview.content = content;

    reviews.insert(newReview.reviewId, newReview);
    Vector<int> reviewIds;
    if (reviewsByGame.exists(gameId))
    {
        reviewIds = reviewsByGame.get(gameId);
    }
    reviewIds.append(newReview.reviewId);
    reviewsByGame.insert(gameId, reviewIds);

    saveToFile<int, Review>("reviews.csv", Review::csvHeader(), reviews, Review::toCSVRow);
    return true;
}

Vector<Review> AppState::getReviewsForGame(int gameId)
{
    Vector<Review> gameReviews;
    if (!reviewsByGame.exists(gameId))
        return gameReviews;

    Vector<int> reviewIds = reviewsByGame.get(gameId);
    for (int i = 0; i < reviewIds.getSize(); i++)
    {
        int reviewId = reviewIds.get(i);
        gameReviews.append(reviews.get(reviewId));
    }
    return gameReviews;
}

Vector<Review> AppState::getReviewsForGameName(const std::string &gameName)
{
    Vector<Review> allReviews;
    Vector<Game> allGames = games.toVector();

    // First check if at least one non-deleted game with this name exists
    bool hasActiveGame = false;
    for (int i = 0; i < allGames.getSize(); i++)
    {
        if (allGames.get(i).name == gameName && !allGames.get(i).isDeleted)
        {
            hasActiveGame = true;
            break;
        }
    }

    // If at least one copy exists, get reviews from ALL games with this name (including deleted ones)
    if (hasActiveGame)
    {
        for (int i = 0; i < allGames.getSize(); i++)
        {
            if (allGames.get(i).name == gameName)
            {
                Vector<Review> gameReviews = getReviewsForGame(allGames.get(i).id);
                for (int j = 0; j < gameReviews.getSize(); j++)
                {
                    allReviews.append(gameReviews.get(j));
                }
            }
        }
    }

    return allReviews;
}

float AppState::getAverageRating(int gameId)
{
    Vector<Review> gameReviews = getReviewsForGame(gameId);
    if (gameReviews.isEmpty())
        return 0.0f;

    int totalRating = 0;
    for (int i = 0; i < gameReviews.getSize(); i++)
    {
        totalRating += gameReviews.get(i).rating;
    }

    return (float)totalRating / gameReviews.getSize();
}

float AppState::getAverageRatingByGameName(const std::string &gameName)
{
    Vector<Review> gameReviews = getReviewsForGameName(gameName);
    if (gameReviews.isEmpty())
        return 0.0f;

    int totalRating = 0;
    for (int i = 0; i < gameReviews.getSize(); i++)
    {
        totalRating += gameReviews.get(i).rating;
    }

    return (float)totalRating / gameReviews.getSize();
}

std::string AppState::getMemberNameById(int memberId)
{
    if (members.exists(memberId))
    {
        return members.get(memberId).username;
    }
    return "Unknown Member";
}

Vector<Game> AppState::getGames()
{
    return games.toVector();
}

Vector<Member> AppState::getMembers()
{
    return members.toVector();
}

void AppState::rebuildGameNames()
{
    gameNames = SuffixArray::build<Game>(
        games.toVector(),
        [](const Game &game)
        { return game.name; },
        [](const Game &game)
        { return game.id; });
}