#ifndef GAME_H
#define GAME_H

#include <string>
#include <cstdio>
#include <cstring>
#include "Vector.h"
#include "SuffixArray.h"

struct Game
{
    int id;
    std::string name;
    std::string publisher;
    int minPlayers;
    int maxPlayers;
    float playtime;
};

// Global data structures
Vector<Game> games;
SuffixArray gameNames;

// Function to add a new board game
void addGame(Vector<Game>& games, SuffixArray& gameNames, const Game& newGame)
{
    // Add to games vector
    games.append(newGame);

    // Reconstruct gameNames suffix array
    gameNames.rebuild(games);

    printf("Game '%s' added successfully with ID %d.\n", newGame.name.c_str(), newGame.id);
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
        printf("  %d. ID: %d, Name: %s, Publisher: %s\n",
               i, game.id, game.name.c_str(), game.publisher.c_str());
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
}

#endif
