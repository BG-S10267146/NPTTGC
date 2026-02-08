#ifndef APPSTATE_H
#define APPSTATE_H

#include <string>
#include <optional>
#include "lib/Vector.h"
#include "lib/Dictionary.h"
#include "lib/Set.h"
#include "lib/SuffixArray.h"
#include "models/Member.h"
#include "models/Game.h"
#include "models/Borrow.h"
#include "models/Review.h"

/// Application state coordinator - manages all data and business logic
class AppState
{
public:
    int currentUserId;
    Dictionary<int, Member> members;
    Dictionary<std::string, int> membersByUsername;
    Dictionary<int, Game> games;
    Dictionary<std::string, int> gamesByName;
    SuffixArray gameNames;
    Dictionary<int, Borrow> borrows;
    Dictionary<int, Vector<int>> borrowsByMember;
    Set<int> borrowedGames;
    Dictionary<int, Review> reviews;
    Dictionary<int, Vector<int>> reviewsByGame;

    AppState();

    void loadData();
    void saveData();
    void loadMembers(const std::string &filename);
    void loadGames(const std::string &filename);
    void loadBorrows(const std::string &filename);
    void loadReviews(const std::string &filename);

    std::optional<Member> authenticateMember(const std::string &username);
    bool addMember(const std::string &username, bool isAdmin);
    void logout();

    bool addGame(const Game &game);
    bool removeGame(int gameId);
    Vector<Game> getGamesForPlayerCount(int playerCount);
    Vector<int> searchGames(const std::string &query);
    Game *getGameById(int gameId);
    std::string getGameNameById(int gameId);

    bool borrowGame(int gameId);
    bool returnGame(int borrowId);
    Vector<Borrow> getAllBorrows();
    Vector<Borrow> getMemberBorrows();
    bool isGameBorrowed(int gameId);

    bool addReview(int gameId, int rating, const std::string &content);
    Vector<Review> getReviewsForGame(int gameId);
    float getAverageRating(int gameId);

    std::string getMemberNameById(int memberId);
    Vector<Game> getGames();
    Vector<Member> getMembers();
};

#endif
