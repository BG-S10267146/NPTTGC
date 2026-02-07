#include "AppState.h"
#include "lib/CSVHelper.h"
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
    saveToFile<Member>("members.csv", Member::csvHeader(), members, Member::toCSVRow);
    saveToFile<Game>("games.csv", Game::csvHeader(), games, Game::toCSVRow);
    saveToFile<Borrow>("borrows.csv", Borrow::csvHeader(), borrows, Borrow::toCSVRow);
    saveToFile<Review>("reviews.csv", Review::csvHeader(), reviews, Review::toCSVRow);
}

void AppState::loadMembers(const std::string& filename)
{
    members = buildFromFile<Member>(filename, Member::fromCSVRow);

    if (members.isEmpty())
    {
        printf("Starting with default admin account.\n");
        Member admin;
        admin.id = 1;
        admin.username = "admin";
        admin.isAdmin = true;
        members.append(admin);
    }

    for (int i = 0; i < members.getSize(); i++)
    {
        Member member = members.get(i);
        membersByUsername.insert(member.username, member.id);
    }

    printf("Loaded %d members from %s\n", members.getSize(), filename.c_str());
}

void AppState::loadGames(const std::string& filename)
{
    games = buildFromFile<Game>(filename, Game::fromCSVRow);
    printf("Loaded %d games from %s\n", games.getSize(), filename.c_str());
    gameNames.rebuild(games);
}

void AppState::loadBorrows(const std::string& filename)
{
    borrows = buildFromFile<Borrow>(filename, Borrow::fromCSVRow);

    for (int i = 0; i < borrows.getSize(); i++)
    {
        Borrow borrow = borrows.get(i);
        if (borrow.dateReturned.empty() || borrow.dateReturned == "N/A")
        {
            borrowedGames.insert(borrow.gameId);
        }
    }

    printf("Loaded %d borrow records from %s\n", borrows.getSize(), filename.c_str());
}

void AppState::loadReviews(const std::string& filename)
{
    reviews = buildFromFile<Review>(filename, Review::fromCSVRow);

    for (int i = 0; i < reviews.getSize(); i++)
    {
        Review review = reviews.get(i);
        reviewsByGame.insert(review.gameId, review.reviewId);
    }

    printf("Loaded %d reviews from %s\n", reviews.getSize(), filename.c_str());
}

Member* AppState::authenticateMember(const std::string& username)
{
    if (!membersByUsername.exists(username))
    {
        return nullptr;
    }

    MemberID memberId = membersByUsername.get(username);

    for (int i = 0; i < members.getSize(); i++)
    {
        if (members[i].id == memberId)
        {
            currentUserId = memberId;
            return &members[i];
        }
    }

    return nullptr;
}

bool AppState::addMember(const std::string& username, bool isAdmin)
{
    if (membersByUsername.exists(username))
    {
        return false;
    }

    MemberID memberId = getNextMemberId();
    Member newMember;
    newMember.id = memberId;
    newMember.username = username;
    newMember.isAdmin = isAdmin;

    members.append(newMember);
    membersByUsername.insert(username, memberId);

    saveToFile<Member>("members.csv", Member::csvHeader(), members, Member::toCSVRow);
    return true;
}

void AppState::logout()
{
    currentUserId = -1;
}

Member* AppState::getCurrentUser()
{
    if (currentUserId == -1)
        return nullptr;

    for (int i = 0; i < members.getSize(); i++)
    {
        if (members[i].id == currentUserId)
        {
            return &members[i];
        }
    }
    return nullptr;
}

int AppState::getCurrentUserId()
{
    return currentUserId;
}

bool AppState::addGame(const Game& game)
{
    games.append(game);
    gameNames.rebuild(games);
    saveToFile<Game>("games.csv", Game::csvHeader(), games, Game::toCSVRow);
    return true;
}

bool AppState::removeGame(int gameId)
{
    if (borrowedGames.exists(gameId))
    {
        return false;
    }

    for (int i = 0; i < games.getSize(); i++)
    {
        if (games[i].id == gameId)
        {
            games[i].isDeleted = true;
            gameNames.rebuild(games);
            saveToFile<Game>("games.csv", Game::csvHeader(), games, Game::toCSVRow);
            return true;
        }
    }

    return false;
}

Vector<Game> AppState::getGamesForPlayerCount(int playerCount)
{
    Vector<Game> matchingGames;
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        if (!game.isDeleted && playerCount >= game.minPlayers && playerCount <= game.maxPlayers)
        {
            matchingGames.append(game);
        }
    }

    if (matchingGames.getSize() > 0)
    {
        quickSortGames(matchingGames, 0, matchingGames.getSize() - 1);
    }

    return matchingGames;
}

Vector<int> AppState::searchGames(const std::string& query)
{
    return gameNames.search(query);
}

Game* AppState::getGameById(int gameId)
{
    for (int i = 0; i < games.getSize(); i++)
    {
        if (games[i].id == gameId)
        {
            return &games[i];
        }
    }
    return nullptr;
}

std::string AppState::getGameNameById(int gameId)
{
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        if (game.id == gameId)
        {
            return game.name;
        }
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
    newBorrow.borrowId = getNextBorrowId();
    newBorrow.memberId = currentUserId;
    newBorrow.gameId = gameId;
    newBorrow.dateBorrowed = getCurrentDateTime();
    newBorrow.dateReturned = "N/A";
    borrows.append(newBorrow);

    borrowedGames.insert(gameId);

    saveToFile<Borrow>("borrows.csv", Borrow::csvHeader(), borrows, Borrow::toCSVRow);
    return true;
}

bool AppState::returnGame(int borrowId)
{
    if (currentUserId == -1)
        return false;

    for (int i = 0; i < borrows.getSize(); i++)
    {
        if (borrows[i].borrowId == borrowId && borrows[i].memberId == currentUserId)
        {
            borrows[i].dateReturned = getCurrentDateTime();
            borrowedGames.remove(borrows[i].gameId);
            saveToFile<Borrow>("borrows.csv", Borrow::csvHeader(), borrows, Borrow::toCSVRow);
            return true;
        }
    }

    return false;
}

Vector<Borrow> AppState::getAllBorrows()
{
    Vector<Borrow> sortedBorrows = borrows;
    sortBorrows(sortedBorrows);
    return sortedBorrows;
}

Vector<Borrow> AppState::getMemberBorrows()
{
    if (currentUserId == -1)
        return Vector<Borrow>();

    Vector<Borrow> memberBorrows;
    for (int i = 0; i < borrows.getSize(); i++)
    {
        Borrow borrow = borrows.get(i);
        if (borrow.memberId == currentUserId)
        {
            memberBorrows.append(borrow);
        }
    }

    sortBorrows(memberBorrows);
    return memberBorrows;
}

bool AppState::isGameBorrowed(int gameId)
{
    return borrowedGames.exists(gameId);
}

bool AppState::addReview(int gameId, int rating, const std::string& content)
{
    if (currentUserId == -1)
        return false;

    Review newReview;
    newReview.reviewId = getNextReviewId();
    newReview.userId = currentUserId;
    newReview.gameId = gameId;
    newReview.rating = rating;
    newReview.content = content;

    reviews.append(newReview);
    reviewsByGame.insert(gameId, newReview.reviewId);

    saveToFile<Review>("reviews.csv", Review::csvHeader(), reviews, Review::toCSVRow);
    return true;
}

Vector<Review> AppState::getReviewsForGame(int gameId)
{
    Vector<Review> gameReviews;
    for (int i = 0; i < reviews.getSize(); i++)
    {
        Review review = reviews.get(i);
        if (review.gameId == gameId)
        {
            gameReviews.append(review);
        }
    }
    return gameReviews;
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

std::string AppState::getMemberNameById(int memberId)
{
    for (int i = 0; i < members.getSize(); i++)
    {
        Member member = members.get(i);
        if (member.id == memberId)
        {
            return member.username;
        }
    }
    return "Unknown Member";
}

const Vector<Game>& AppState::getGames()
{
    return games;
}

const Vector<Member>& AppState::getMembers()
{
    return members;
}

int AppState::getNextMemberId()
{
    int maxId = 0;
    for (int i = 0; i < members.getSize(); i++)
    {
        if (members.get(i).id > maxId)
        {
            maxId = members.get(i).id;
        }
    }
    return maxId + 1;
}

int AppState::getNextBorrowId()
{
    int maxId = 0;
    for (int i = 0; i < borrows.getSize(); i++)
    {
        if (borrows.get(i).borrowId > maxId)
        {
            maxId = borrows.get(i).borrowId;
        }
    }
    return maxId + 1;
}

int AppState::getNextReviewId()
{
    int maxId = 0;
    for (int i = 0; i < reviews.getSize(); i++)
    {
        if (reviews.get(i).reviewId > maxId)
        {
            maxId = reviews.get(i).reviewId;
        }
    }
    return maxId + 1;
}

int AppState::partitionGames(Vector<Game>& gamesList, int low, int high)
{
    std::string pivot = gamesList.get(high).name;
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (gamesList.get(j).name < pivot)
        {
            i++;
            Game temp = gamesList.get(i);
            gamesList[i] = gamesList.get(j);
            gamesList[j] = temp;
        }
    }

    Game temp = gamesList.get(i + 1);
    gamesList[i + 1] = gamesList.get(high);
    gamesList[high] = temp;

    return i + 1;
}

void AppState::quickSortGames(Vector<Game>& gamesList, int low, int high)
{
    if (low < high)
    {
        int pi = partitionGames(gamesList, low, high);
        quickSortGames(gamesList, low, pi - 1);
        quickSortGames(gamesList, pi + 1, high);
    }
}

int AppState::compareBorrows(const Borrow& b1, const Borrow& b2)
{
    int borrowCompare = compareDates(b1.dateBorrowed, b2.dateBorrowed);
    if (borrowCompare != 0)
        return borrowCompare;

    return compareDates(b1.dateReturned, b2.dateReturned);
}

void AppState::sortBorrows(Vector<Borrow>& borrowList)
{
    int size = borrowList.getSize();

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            Borrow b1 = borrowList.get(j);
            Borrow b2 = borrowList.get(j + 1);

            if (compareBorrows(b1, b2) > 0)
            {
                Borrow temp = b1;
                borrowList[j] = b2;
                borrowList[j + 1] = temp;
            }
        }
    }
}
