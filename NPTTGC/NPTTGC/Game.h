#ifndef GAME_H
#define GAME_H

#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
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
    file << "name,minplayers,maxplayers,maxplaytime,minplaytime,yearpublished\n";

    // Write games
    for (int i = 0; i < games.getSize(); i++)
    {
        Game game = games.get(i);
        file << escapeCSVField(game.name) << ","
             << game.minPlayers << ","
             << game.maxPlayers << ","
             << game.maxPlaytime << ","
             << game.minPlaytime << ","
             << game.yearPublished << "\n";
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
void removeGame(Vector<Game>& games, SuffixArray& gameNames)
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
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
               i, game.id, game.name.c_str(), game.minPlayers, game.maxPlayers);
    }

    // Prompt to select a game
    printf("\nEnter the number of the game to remove (0-%d): ", matchingGameIndices.getSize() - 1);
    int selection;
    scanf("%d", &selection);
    getchar();

    if (selection < 0 || selection >= matchingGameIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    int gameIdxToRemove = matchingGameIndices.get(selection);
    Game removedGame = games.get(gameIdxToRemove);

    // Remove game from vector
    games.remove(gameIdxToRemove);

    // Reconstruct gameNames suffix array
    gameNames.rebuild(games);

    printf("Game '%s' (ID: %d) removed successfully.\n", removedGame.name.c_str(), removedGame.id);
    
    // Save changes to CSV
    saveGamesToCSV(games, "games.csv");
}

#endif
