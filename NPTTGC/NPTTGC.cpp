// NPTTGC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include "Member.h"
#include "Game.h"
#include "Borrow.h"
#include "Review.h"

// ============ Helper Functions ============

// Function to safely read an integer from input
bool readInteger(int& value)
{
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) == nullptr)
        return false;
    
    // Check if the input is a valid integer
    char* endPtr;
    value = strtol(buffer, &endPtr, 10);
    
    // Check if conversion was successful and the rest is whitespace
    while (*endPtr && isspace(*endPtr))
        endPtr++;
    
    return (*endPtr == '\0');
}

// Function to safely read an integer from a string (not stdin)
bool readIntegerFromString(const char* str, int& value)
{
    if (str == nullptr || str[0] == '\0')
        return false;
    
    char* endPtr;
    value = strtol(str, &endPtr, 10);
    
    // Check if conversion was successful and the rest is whitespace
    while (*endPtr && isspace(*endPtr))
        endPtr++;
    
    return (*endPtr == '\0');
}

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
    
    if (strlen(nameBuf) == 0)
    {
        printf("Error: Game name cannot be empty.\n");
        return;
    }
    
    int minPlayers, maxPlayers, minPlaytime, maxPlaytime, yearPublished;
    
    printf("Enter minimum players: ");
    if (!readInteger(minPlayers) || minPlayers <= 0)
    {
        printf("Error: Minimum players must be a positive number.\n");
        return;
    }
    
    printf("Enter maximum players: ");
    if (!readInteger(maxPlayers) || maxPlayers <= 0)
    {
        printf("Error: Maximum players must be a positive number.\n");
        return;
    }
    
    if (maxPlayers < minPlayers)
    {
        printf("Error: Maximum players cannot be less than minimum players.\n");
        return;
    }
    
    printf("Enter minimum playtime (minutes): ");
    if (!readInteger(minPlaytime) || minPlaytime <= 0)
    {
        printf("Error: Minimum playtime must be a positive number.\n");
        return;
    }
    
    printf("Enter maximum playtime (minutes): ");
    if (!readInteger(maxPlaytime) || maxPlaytime <= 0)
    {
        printf("Error: Maximum playtime must be a positive number.\n");
        return;
    }
    
    if (maxPlaytime < minPlaytime)
    {
        printf("Error: Maximum playtime cannot be less than minimum playtime.\n");
        return;
    }
    
    // Get current year
    time_t now = time(nullptr);
    struct tm* timeInfo = localtime(&now);
    int currentYear = timeInfo->tm_year + 1900;
    
    printf("Enter year published (or press Enter for current year %d): ", currentYear);
    char yearBuf[10];
    fgets(yearBuf, sizeof(yearBuf), stdin);
    yearBuf[strcspn(yearBuf, "\n")] = 0;
    
    if (strlen(yearBuf) == 0)
    {
        yearPublished = currentYear;
    }
    else
    {
        if (!readIntegerFromString(yearBuf, yearPublished) || yearPublished < 1990 || yearPublished > currentYear)
        {
            printf("Error: Year published must be a valid number between 1990 and %d.\n", currentYear);
            return;
        }
    }
    printf("Year published: %d\n", yearPublished);
    
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
    
    printf("Enter username: ");
    fgets(usernameBuf, sizeof(usernameBuf), stdin);
    usernameBuf[strcspn(usernameBuf, "\n")] = 0;
    
    printf("Is this an admin account? (y/n): ");
    char isAdminChoice;
    scanf("%c", &isAdminChoice);
    getchar();
    
    bool isAdmin = (isAdminChoice == 'y' || isAdminChoice == 'Y');
    
    addMember(members, membersByUsername, std::string(usernameBuf), isAdmin);
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
    
    // Filter out deleted games
    Vector<int> availableGameIndices;
    printf("\nMatching games:\n");
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        if (!game.isDeleted)
        {
            printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
                   availableGameIndices.getSize(), game.id, game.name.c_str(), game.minPlayers, game.maxPlayers);
            availableGameIndices.append(gameIdx);
        }
    }
    
    if (availableGameIndices.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }
    
    printf("\nEnter the number of the game to review (0-%d): ", availableGameIndices.getSize() - 1);
    int selection;
    if (!readInteger(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }
    
    if (selection < 0 || selection >= availableGameIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }
    
    Game selectedGame = games.get(availableGameIndices.get(selection));
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
    
    // Filter out deleted games
    Vector<int> availableGameIndices;
    printf("\nMatching games:\n");
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        if (!game.isDeleted)
        {
            printf("  %d. ID: %d, Name: %s\n", availableGameIndices.getSize(), game.id, game.name.c_str());
            availableGameIndices.append(gameIdx);
        }
    }
    
    if (availableGameIndices.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }
    
    printf("\nEnter the number of the game to view reviews (0-%d): ", availableGameIndices.getSize() - 1);
    int selection;
    if (!readInteger(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }
    
    if (selection < 0 || selection >= availableGameIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }
    
    Game selectedGame = games.get(availableGameIndices.get(selection));
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
        
        printf("Username (or 'quit' to exit): ");
        fgets(usernameBuf, sizeof(usernameBuf), stdin);
        usernameBuf[strcspn(usernameBuf, "\n")] = 0;
        
        if (strcmp(usernameBuf, "quit") == 0)
        {
            printf("\nThank you for using NPTTGC Board Game Club System!\n");
            break;
        }
        
        // Authenticate user
        Member* currentUser = authenticateMember(members, membersByUsername, 
                                                 std::string(usernameBuf));
        
        if (currentUser == nullptr)
        {
            printf("\nInvalid username. Please try again.\n");
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
                if (!readInteger(choice))
                {
                    printf("Invalid selection. Please enter a number.\n");
                    continue;
                }
                
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
                if (!readInteger(choice))
                {
                    printf("Invalid selection. Please enter a number.\n");
                    continue;
                }
                
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

