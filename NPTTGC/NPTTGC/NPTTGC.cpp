// NPTTGC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <cstring>
#include "Member.h"
#include "Game.h"
#include "Borrow.h"
#include "Review.h"

int main()
{
    // Example usage of Member management
    printf("=== Member Management Example ===\n\n");

    // Add members
    addMember(members, membersByUsername, "alice");
    addMember(members, membersByUsername, "bob");
    addMember(members, membersByUsername, "charlie");
    addMember(members, membersByUsername, "diana");

    printf("\n");

    // Try to add duplicate member
    addMember(members, membersByUsername, "alice");

    printf("\n");

    // Display all members
    printf("Total members: %d\n", members.getSize());
    printf("Members by username dictionary size: %d\n\n", membersByUsername.getSize());

    printf("Members list:\n");
    for (int i = 0; i < members.getSize(); i++)
    {
        Member member = members.get(i);
        printf("  ID: %d, Username: %s\n", member.id, member.username.c_str());
    }

    printf("\n");

    // Verify membersByUsername mapping
    printf("Member ID lookup:\n");
    printf("  'bob' -> ID %d\n", membersByUsername.get("bob"));
    printf("  'diana' -> ID %d\n", membersByUsername.get("diana"));

    // Example usage of Board Game management
    printf("\n=== Board Game Management Example ===\n\n");

    // Add some games
    printf("Adding games...\n");
    printf("Game 1:\n");
    printf("  Name: Catan\n");
    printf("  Publisher: Catan Studio\n");
    printf("  Players: 2-4\n");
    printf("  Playtime: 90 minutes\n");
    Game game1;
    game1.id = games.getSize();
    game1.name = "Catan";
    game1.publisher = "Catan Studio";
    game1.minPlayers = 2;
    game1.maxPlayers = 4;
    game1.playtime = 90.0f;
    addGame(games, gameNames, game1);

    printf("\nGame 2:\n");
    printf("  Name: Ticket to Ride\n");
    printf("  Publisher: Days of Wonder\n");
    printf("  Players: 2-5\n");
    printf("  Playtime: 60 minutes\n");
    Game game2;
    game2.id = games.getSize();
    game2.name = "Ticket to Ride";
    game2.publisher = "Days of Wonder";
    game2.minPlayers = 2;
    game2.maxPlayers = 5;
    game2.playtime = 60.0f;
    addGame(games, gameNames, game2);

    printf("\n");

    // Display all games
    printf("Total games: %d\n", games.getSize());
    printf("Game names suffix array size: %d\n\n", gameNames.getSize());

    printf("Games list:\n");
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        printf("  ID: %d, Name: %s, Publisher: %s, Players: %d-%d, Playtime: %.0f min\n",
               game.id, game.name.c_str(), game.publisher.c_str(), 
               game.minPlayers, game.maxPlayers, game.playtime);
    }

    // Example of removing a game
    printf("\n=== Game Removal Example ===\n");
    printf("Before removal - Total games: %d\n", games.getSize());
    removeGame(games, gameNames);
    printf("After removal - Total games: %d\n", games.getSize());

    printf("\nRemaining games:\n");
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        printf("  ID: %d, Name: %s, Publisher: %s\n",
               game.id, game.name.c_str(), game.publisher.c_str());
    }

    // Example usage of Borrow management
    printf("\n=== Borrow Management Example ===\n\n");

    // Add some borrow records
    printf("Adding borrow records...\n");
    Borrow borrow1;
    borrow1.memberId = 1;
    borrow1.gameId = 0;
    borrow1.dateBorrowed = "2026-01-15";
    borrow1.dateReturned = "2026-01-22";
    borrows.append(borrow1);
    printf("Record 1: Member 1 borrowed Game 0 on 2026-01-15, returned 2026-01-22\n");

    Borrow borrow2;
    borrow2.memberId = 0;
    borrow2.gameId = 1;
    borrow2.dateBorrowed = "2026-01-10";
    borrow2.dateReturned = "2026-01-18";
    borrows.append(borrow2);
    printf("Record 2: Member 0 borrowed Game 1 on 2026-01-10, returned 2026-01-18\n");

    Borrow borrow3;
    borrow3.memberId = 2;
    borrow3.gameId = 0;
    borrow3.dateBorrowed = "2026-01-20";
    borrow3.dateReturned = "2026-01-27";
    borrows.append(borrow3);
    printf("Record 3: Member 2 borrowed Game 0 on 2026-01-20, returned 2026-01-27\n");

    Borrow borrow4;
    borrow4.memberId = 3;
    borrow4.gameId = 1;
    borrow4.dateBorrowed = "2026-01-10";
    borrow4.dateReturned = "2026-01-25";
    borrows.append(borrow4);
    printf("Record 4: Member 3 borrowed Game 1 on 2026-01-10, returned 2026-01-25\n");

    printf("\nTotal borrow records: %d\n", borrows.getSize());

    // Display sorted borrow summary
    displayBorrowsSummary(borrows);

    // Example usage of Member Borrow management
    printf("\n=== Member Borrow/Return Example ===\n\n");

    // Simulate member 0 borrowing a game
    printf("Member 0 (alice) borrowing a game:\n");
    memberBorrowGame(0, games, borrows, gameNames, borrowedGames);

    // Simulate member 1 borrowing a game
    printf("\n\nMember 1 (bob) borrowing a game:\n");
    memberBorrowGame(1, games, borrows, gameNames, borrowedGames);

    // Display member 0's borrow summary
    printf("\n\nMember 0 (alice) borrow summary:\n");
    memberDisplayBorrowSummary(0, games, borrows);

    // Simulate member 0 returning a game
    printf("\nMember 0 (alice) returning a game:\n");
    memberReturnGame(0, games, borrows, borrowedGames);

    // Display all borrow records after return
    printf("\n\nUpdated borrow summary (all records):\n");
    displayBorrowsSummary(borrows);

    // Example usage of Review management
    printf("\n=== Review Management Example ===\n\n");

    // Add some reviews
    printf("Adding reviews...\n");
    addReview(0, 0, reviews, reviewsByGame);
    printf("Review added by member 0 for game 0\n");

    addReview(1, 0, reviews, reviewsByGame);
    printf("Review added by member 1 for game 0\n");

    addReview(2, 1, reviews, reviewsByGame);
    printf("Review added by member 2 for game 1\n");

    addReview(3, 1, reviews, reviewsByGame);
    printf("Review added by member 3 for game 1\n");

    printf("\nTotal reviews: %d\n", reviews.getSize());

    // Display reviews for game 0
    displayGameReviews(0, reviews, reviewsByGame);

    // Display average rating for game 0
    displayGameAverageRating(0, reviews, reviewsByGame);

    // Display reviews for game 1
    displayGameReviews(1, reviews, reviewsByGame);

    // Display average rating for game 1
    displayGameAverageRating(1, reviews, reviewsByGame);

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to add/manage files
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
