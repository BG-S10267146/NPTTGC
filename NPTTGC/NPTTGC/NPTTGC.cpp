// NPTTGC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <cstring>
#include <string>
#include "Member.h"
#include "Game.h"
#include "Borrow.h"
#include "Review.h"

// ============ Menu Functions ============

// Function to display admin menu
void displayAdminMenu()
{
    printf("\n========== ADMIN MENU ==========\n");
    printf("1. Add a new board game\n");
    printf("2. Remove a board game\n");
    printf("3. Add a new member\n");
    printf("4. Display borrow summary\n");
    printf("5. Display games by player count\n");
    printf("6. View game reviews\n");
    printf("7. Logout\n");
    printf("================================\n");
    printf("Enter your choice: ");
}

// Function to display member menu
void displayMemberMenu()
{
    printf("\n========== MEMBER MENU ==========\n");
    printf("1. Borrow a board game\n");
    printf("2. Return a board game\n");
    printf("3. Display my borrow summary\n");
    printf("4. Write a review\n");
    printf("5. View game reviews\n");
    printf("6. Display games by player count\n");
    printf("7. Logout\n");
    printf("=================================\n");
    printf("Enter your choice: ");
}

// Function to handle admin adding a game
void handleAddGame(Vector<Game>& games, SuffixArray& gameNames)
{
    printf("\n=== Add New Board Game ===\n");
    
    char nameBuf[200];
    printf("Enter game name: ");
    fgets(nameBuf, sizeof(nameBuf), stdin);
    nameBuf[strcspn(nameBuf, "\n")] = 0;
    
    int minPlayers, maxPlayers, minPlaytime, maxPlaytime, yearPublished;
    
    printf("Enter minimum players: ");
    scanf("%d", &minPlayers);
    
    printf("Enter maximum players: ");
    scanf("%d", &maxPlayers);
    
    printf("Enter minimum playtime (minutes): ");
    scanf("%d", &minPlaytime);
    
    printf("Enter maximum playtime (minutes): ");
    scanf("%d", &maxPlaytime);
    
    // Automatically set year published to current year (2026)
    yearPublished = 2026;
    printf("Year published: %d (set to current year)\n", yearPublished);
    getchar();
    
    // Create game
    Game newGame;
    newGame.id = games.getSize() + 1;
    newGame.name = nameBuf;
    newGame.minPlayers = minPlayers;
    newGame.maxPlayers = maxPlayers;
    newGame.minPlaytime = minPlaytime;
    newGame.maxPlaytime = maxPlaytime;
    newGame.yearPublished = yearPublished;
    newGame.isDeleted = false;
    
    addGame(games, gameNames, newGame);
}

// Function to handle admin adding a member
void handleAddMember(Vector<Member>& members, Dictionary<std::string, MemberID>& membersByUsername)
{
    printf("\n=== Add New Member ===\n");
    
    char usernameBuf[100];
    char passwordBuf[100];
    
    printf("Enter username: ");
    fgets(usernameBuf, sizeof(usernameBuf), stdin);
    usernameBuf[strcspn(usernameBuf, "\n")] = 0;
    
    printf("Enter password: ");
    fgets(passwordBuf, sizeof(passwordBuf), stdin);
    passwordBuf[strcspn(passwordBuf, "\n")] = 0;
    
    printf("Is this an admin account? (y/n): ");
    char isAdminChoice;
    scanf("%c", &isAdminChoice);
    getchar();
    
    bool isAdmin = (isAdminChoice == 'y' || isAdminChoice == 'Y');
    
    addMember(members, membersByUsername, std::string(usernameBuf), std::string(passwordBuf), isAdmin);
}

// Function to handle member writing a review
void handleWriteReview(int memberId, Vector<Game>& games, SuffixArray& gameNames, Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame)
{
    printf("\n=== Write a Review ===\n");
    
    // Search for games
    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;
    
    Vector<int> matchingGameIndices = gameNames.search(searchTerm);
    
    if (matchingGameIndices.isEmpty())
    {
        printf("No games found matching your search.\n");
        return;
    }
    
    printf("\nMatching games:\n");
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
               i, game.id, game.name.c_str(), game.minPlayers, game.maxPlayers);
    }
    
    printf("\nEnter the number of the game to review (0-%d): ", matchingGameIndices.getSize() - 1);
    int selection;
    scanf("%d", &selection);
    getchar();
    
    if (selection < 0 || selection >= matchingGameIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }
    
    Game selectedGame = games.get(matchingGameIndices.get(selection));
    addReview(memberId, selectedGame.id, reviews, reviewsByGame);
}

// Function to handle viewing game reviews
void handleViewReviews(Vector<Game>& games, SuffixArray& gameNames, Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame)
{
    printf("\n=== View Game Reviews ===\n");
    
    // Search for games
    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;
    
    Vector<int> matchingGameIndices = gameNames.search(searchTerm);
    
    if (matchingGameIndices.isEmpty())
    {
        printf("No games found matching your search.\n");
        return;
    }
    
    printf("\nMatching games:\n");
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        printf("  %d. ID: %d, Name: %s\n", i, game.id, game.name.c_str());
    }
    
    printf("\nEnter the number of the game to view reviews (0-%d): ", matchingGameIndices.getSize() - 1);
    int selection;
    scanf("%d", &selection);
    getchar();
    
    if (selection < 0 || selection >= matchingGameIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }
    
    Game selectedGame = games.get(matchingGameIndices.get(selection));
    displayGameReviews(selectedGame.id, reviews, reviewsByGame);
    displayGameAverageRating(selectedGame.id, reviews, reviewsByGame);
}

int main()
{
    printf("====================================\n");
    printf("  NPTTGC Board Game Club System\n");
    printf("====================================\n\n");
    
    // Load data from CSV files
    printf("Loading data from CSV files...\n");
    loadMembersFromCSV(members, membersByUsername, "members.csv");
    loadGamesFromCSV(games, gameNames, "games.csv");
    loadBorrowsFromCSV(borrows, borrowedGames, "borrows.csv");
    loadReviewsFromCSV(reviews, reviewsByGame, "reviews.csv");
    printf("\nData loaded successfully!\n");
    
    // Main program loop
    while (true)
    {
        printf("\n====================================\n");
        printf("           LOGIN\n");
        printf("====================================\n");
        
        char usernameBuf[100];
        char passwordBuf[100];
        
        printf("Username (or 'quit' to exit): ");
        fgets(usernameBuf, sizeof(usernameBuf), stdin);
        usernameBuf[strcspn(usernameBuf, "\n")] = 0;
        
        if (strcmp(usernameBuf, "quit") == 0)
        {
            printf("\nThank you for using NPTTGC Board Game Club System!\n");
            break;
        }
        
        printf("Password: ");
        fgets(passwordBuf, sizeof(passwordBuf), stdin);
        passwordBuf[strcspn(passwordBuf, "\n")] = 0;
        
        // Authenticate user
        Member* currentUser = authenticateMember(members, membersByUsername, 
                                                 std::string(usernameBuf), 
                                                 std::string(passwordBuf));
        
        if (currentUser == nullptr)
        {
            printf("\nInvalid username or password. Please try again.\n");
            continue;
        }
        
        printf("\nWelcome, %s!\n", currentUser->username.c_str());
        
        // Menu loop for logged-in user
        bool loggedIn = true;
        while (loggedIn)
        {
            if (currentUser->isAdmin)
            {
                // Admin menu
                displayAdminMenu();
                int choice;
                scanf("%d", &choice);
                getchar();
                
                switch (choice)
                {
                    case 1:
                        handleAddGame(games, gameNames);
                        break;
                    case 2:
                        removeGame(games, gameNames, borrowedGames);
                        break;
                    case 3:
                        handleAddMember(members, membersByUsername);
                        break;
                    case 4:
                        displayBorrowsSummary(borrows, games, members);
                        break;
                    case 5:
                        displayGamesByPlayerCount(games);
                        break;
                    case 6:
                        handleViewReviews(games, gameNames, reviews, reviewsByGame);
                        break;
                    case 7:
                        printf("\nLogging out...\n");
                        loggedIn = false;
                        break;
                    default:
                        printf("\nInvalid choice. Please try again.\n");
                }
            }
            else
            {
                // Member menu
                displayMemberMenu();
                int choice;
                scanf("%d", &choice);
                getchar();
                
                switch (choice)
                {
                    case 1:
                        memberBorrowGame(currentUser->id, games, borrows, gameNames, borrowedGames);
                        break;
                    case 2:
                        memberReturnGame(currentUser->id, games, borrows, borrowedGames);
                        break;
                    case 3:
                        memberDisplayBorrowSummary(currentUser->id, games, borrows);
                        break;
                    case 4:
                        handleWriteReview(currentUser->id, games, gameNames, reviews, reviewsByGame);
                        break;
                    case 5:
                        handleViewReviews(games, gameNames, reviews, reviewsByGame);
                        break;
                    case 6:
                        displayGamesByPlayerCount(games);
                        break;
                    case 7:
                        printf("\nLogging out...\n");
                        loggedIn = false;
                        break;
                    default:
                        printf("\nInvalid choice. Please try again.\n");
                }
            }
        }
    }
    
    return 0;
}

