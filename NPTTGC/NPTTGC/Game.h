#ifndef GAME_H
#define GAME_H

#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include "Set.h"
#include "Vector.h"
#include "SuffixArray.h"
#include "CSVHelper.h"

struct Game
{
    int id;
    std::string name;
    int minPlayers;
    int maxPlayers;
    int minPlaytime;
    int maxPlaytime;
    int yearPublished;
    bool isDeleted;
};


// Global data structures
Vector<Game> games;
SuffixArray gameNames;

// Function to load games from CSV
void loadGamesFromCSV(Vector<Game>& games, SuffixArray& gameNames, const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        printf("Warning: Could not open %s. Starting with empty games list.\n", filename.c_str());
        return;
    }

    std::string line;
    // Skip header line
    std::getline(file, line);

    int gameId = 1;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        Vector<std::string> fields = splitCSVLine(line);
        if (fields.getSize() >= 6)
        {
            Game game;
            game.id = gameId++;
            game.name = trim(fields.get(0));
            game.minPlayers = atoi(fields.get(1).c_str());
            game.maxPlayers = atoi(fields.get(2).c_str());
            game.maxPlaytime = atoi(fields.get(3).c_str());
            game.minPlaytime = atoi(fields.get(4).c_str());
            game.yearPublished = atoi(fields.get(5).c_str());
            if (fields.getSize() >= 7)
            {
                game.isDeleted = atoi(fields.get(6).c_str()) != 0;
            }
            else
            {
                game.isDeleted = false;
            }
            games.append(game);
        }
    }
    file.close();

    // Build suffix array
    gameNames.rebuild(games);

    printf("Loaded %d games from %s\n", games.getSize(), filename.c_str());
}

// Function to save games to CSV
void saveGamesToCSV(Vector<Game>& games, const std::string& filename)
{
    std::ofstream file(filename.c_str());
    if (!file.is_open())
    {
        printf("Error: Could not open %s for writing.\n", filename.c_str());
        return;
    }

    // Write header
    file << "name,minplayers,maxplayers,maxplaytime,minplaytime,yearpublished,isdeleted\n";

    // Write games
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        file << escapeCSVField(game.name) << ","
             << game.minPlayers << ","
             << game.maxPlayers << ","
             << game.maxPlaytime << ","
             << game.minPlaytime << ","
               << game.yearPublished << ","
               << (game.isDeleted ? 1 : 0) << "\n";
    }

    file.close();
}

// Function to add a new board game
void addGame(Vector<Game>& games, SuffixArray& gameNames, const Game& newGame)
{
    // Add to games vector
    games.append(newGame);

    // Reconstruct gameNames suffix array
    gameNames.rebuild(games);

    printf("Game '%s' added successfully with ID %d.\n", newGame.name.c_str(), newGame.id);
    
    // Save changes to CSV
    saveGamesToCSV(games, "games.csv");
}

// Function to remove a board game
void removeGame(Vector<Game>& games, SuffixArray& gameNames, Set<int>& borrowedGames)
{
    printf("\n=== Remove Board Game ===\n");

    // Prompt for search query
    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;

    // Perform suffix array search
    Vector<int> matchingGameIndices = gameNames.search(searchTerm);

    if (matchingGameIndices.isEmpty())
    {
        printf("No games found matching your search.\n");
        return;
    }

    printf("\nMatching games:\n");
    Vector<int> activeIndices;
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        if (!game.isDeleted)
        {
            activeIndices.append(gameIdx);
            printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
                   activeIndices.getSize() - 1, game.id, game.name.c_str(), game.minPlayers, game.maxPlayers);
        }
    }

    if (activeIndices.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    // Prompt to select a game
    printf("\nEnter the number of the game to remove (0-%d): ", activeIndices.getSize() - 1);
    int selection;
    scanf("%d", &selection);
    getchar();

    if (selection < 0 || selection >= activeIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    int gameIdxToRemove = activeIndices.get(selection);
    Game& gameToRemove = games[gameIdxToRemove];

    if (gameToRemove.isDeleted)
    {
        printf("Game '%s' (ID: %d) is already removed.\n", gameToRemove.name.c_str(), gameToRemove.id);
        return;
    }

    if (borrowedGames.exists(std::to_string(gameToRemove.id)))
    {
        printf("Cannot remove game '%s' (ID: %d) because it is currently borrowed and not returned.\n",
               gameToRemove.name.c_str(), gameToRemove.id);
        return;
    }

    // Soft delete game (mark as deleted)
    gameToRemove.isDeleted = true;

    // Reconstruct gameNames suffix array
    gameNames.rebuild(games);

    printf("Game '%s' (ID: %d) removed successfully.\n", gameToRemove.name.c_str(), gameToRemove.id);
    
    // Save changes to CSV
    saveGamesToCSV(games, "games.csv");
}

// QuickSort helper functions
int partitionGames(Vector<Game>& gamesList, int low, int high)
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

void quickSortGames(Vector<Game>& gamesList, int low, int high)
{
    if (low < high)
    {
        int pi = partitionGames(gamesList, low, high);
        quickSortGames(gamesList, low, pi - 1);
        quickSortGames(gamesList, pi + 1, high);
    }
}

// Function to display games by player count
void displayGamesByPlayerCount(Vector<Game>& games)
{
    printf("\n=== Display Games by Player Count ===\n");
    
    int playerCount;
    printf("Enter number of players: ");
    scanf("%d", &playerCount);
    getchar();
    
    if (playerCount <= 0)
    {
        printf("Invalid number of players.\n");
        return;
    }
    
    // Linear scan to find matching games
    Vector<Game> matchingGames;
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        if (game.isDeleted)
        {
            continue;
        }
        // Range check: player count must be within min and max players
        if (playerCount >= game.minPlayers && playerCount <= game.maxPlayers)
        {
            matchingGames.append(game);
        }
    }
    
    if (matchingGames.isEmpty())
    {
        printf("No games found that can be played with %d player(s).\n", playerCount);
        return;
    }
    
    // Sort the matching games using QuickSort
    if (matchingGames.getSize() > 0)
    {
        quickSortGames(matchingGames, 0, matchingGames.getSize() - 1);
    }
    
    // Display results with pagination
    int totalGames = matchingGames.getSize();
    int gamesPerPage = 10;
    int totalPages = (totalGames + gamesPerPage - 1) / gamesPerPage;
    int currentPage = 0;
    
    while (true)
    {
        // Calculate range for current page
        int startIdx = currentPage * gamesPerPage;
        int endIdx = startIdx + gamesPerPage;
        if (endIdx > totalGames)
            endIdx = totalGames;
        
        // Display current page
        printf("\n=================================================================\n");
        printf("Games that can be played with %d player(s) (sorted alphabetically)\n", playerCount);
        printf("Page %d of %d (Showing %d-%d of %d games)\n", 
               currentPage + 1, totalPages, startIdx + 1, endIdx, totalGames);
        printf("=================================================================\n");
        
        for (int i = startIdx; i < endIdx; i++)
        {
            Game game = matchingGames.get(i);
            printf("%d. %s\n", i + 1, game.name.c_str());
            printf("   Players: %d-%d | Playtime: %d-%d min | Year: %d\n",
                   game.minPlayers, game.maxPlayers,
                   game.minPlaytime, game.maxPlaytime,
                   game.yearPublished);
        }
        printf("=================================================================\n");
        
        // Navigation options
        printf("\nNavigation Options:\n");
        if (currentPage > 0)
            printf("  [P] Previous page\n");
        if (currentPage < totalPages - 1)
            printf("  [N] Next page\n");
        printf("  [G] Go to specific page\n");
        printf("  [Q] Quit/Return to menu\n");
        printf("Enter your choice: ");
        
        char choice;
        scanf("%c", &choice);
        getchar();
        
        if (choice == 'N' || choice == 'n')
        {
            if (currentPage < totalPages - 1)
                currentPage++;
            else
                printf("Already at the last page.\n");
        }
        else if (choice == 'P' || choice == 'p')
        {
            if (currentPage > 0)
                currentPage--;
            else
                printf("Already at the first page.\n");
        }
        else if (choice == 'G' || choice == 'g')
        {
            printf("Enter page number (1-%d): ", totalPages);
            int pageNum;
            scanf("%d", &pageNum);
            getchar();
            
            if (pageNum >= 1 && pageNum <= totalPages)
            {
                currentPage = pageNum - 1;
            }
            else
            {
                printf("Invalid page number. Please enter a number between 1 and %d.\n", totalPages);
            }
        }
        else if (choice == 'Q' || choice == 'q')
        {
            break;
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }
    }
}

#endif
