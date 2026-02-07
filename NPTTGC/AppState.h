#ifndef APPSTATE_H
#define APPSTATE_H

#include <string>
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
private:
    Vector<Member> members;
    Dictionary<std::string, MemberID> membersByUsername;
    Vector<Game> games;
    SuffixArray gameNames;
    Vector<Borrow> borrows;
    Set<GameID> borrowedGames;
    Vector<Review> reviews;
    Dictionary<int, int> reviewsByGame;
    int currentUserId;

    int getNextMemberId();
    int getNextBorrowId();
    int getNextReviewId();
    void quickSortGames(Vector<Game>& gamesList, int low, int high);
    int partitionGames(Vector<Game>& gamesList, int low, int high);
    void sortBorrows(Vector<Borrow>& borrowList);
    int compareBorrows(const Borrow& b1, const Borrow& b2);

public:
    AppState();

    void loadData();
    void saveData();
    void loadMembers(const std::string& filename);
    void loadGames(const std::string& filename);
    void loadBorrows(const std::string& filename);
    void loadReviews(const std::string& filename);

    Member* authenticateMember(const std::string& username);
    bool addMember(const std::string& username, bool isAdmin);
    void logout();
    Member* getCurrentUser();
    int getCurrentUserId();

    bool addGame(const Game& game);
    bool removeGame(int gameId);
    Vector<Game> getGamesForPlayerCount(int playerCount);
    Vector<int> searchGames(const std::string& query);
    Game* getGameById(int gameId);
    std::string getGameNameById(int gameId);

    bool borrowGame(int gameId);
    bool returnGame(int borrowId);
    Vector<Borrow> getAllBorrows();
    Vector<Borrow> getMemberBorrows();
    bool isGameBorrowed(int gameId);

    bool addReview(int gameId, int rating, const std::string& content);
    Vector<Review> getReviewsForGame(int gameId);
    float getAverageRating(int gameId);

    std::string getMemberNameById(int memberId);
    const Vector<Game>& getGames();
    const Vector<Member>& getMembers();
};

#endif
