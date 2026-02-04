#ifndef BORROW_H
#define BORROW_H

#include <string>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>
#include "Vector.h"
#include "Set.h"
#include "SuffixArray.h"
#include "Game.h"
#include "CSVHelper.h"

typedef int GameID;

struct Borrow
{
    int borrowId;
    int memberId;
    int gameId;
    std::string gameName;
    std::string dateBorrowed;
    std::string dateReturned;
};

// Global data structures
Vector<Borrow> borrows;
Set<GameID> borrowedGames;

// Helper function to convert int to string (cross-platform compatible)
std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Helper function to get current local datetime (YYYY-MM-DD HH:MM)
std::string getCurrentDateTime()
{
    std::time_t now = std::time(nullptr);
    std::tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &now);
#else
    timeInfo = *std::localtime(&now);
#endif
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &timeInfo);
    return std::string(buffer);
}

// Function to load borrows from CSV
void loadBorrowsFromCSV(Vector<Borrow>& borrows, Set<GameID>& borrowedGames, const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        printf("Warning: Could not open %s. Starting with empty borrows list.\n", filename.c_str());
        return;
    }

    std::string line;
    // Skip header line
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        Vector<std::string> fields = splitCSVLine(line);
        if (fields.getSize() >= 6)
        {
            Borrow borrow;
            borrow.borrowId = atoi(fields.get(0).c_str());
            borrow.memberId = atoi(fields.get(1).c_str());
            borrow.gameId = atoi(fields.get(2).c_str());
            borrow.gameName = trim(fields.get(3));
            borrow.dateBorrowed = trim(fields.get(4));
            borrow.dateReturned = trim(fields.get(5));
            
            borrows.append(borrow);
            
            // If not yet returned, mark as borrowed
            if (borrow.dateReturned.empty() || borrow.dateReturned == "N/A")
            {
                borrowedGames.insert(intToString(borrow.gameId), borrow.gameId);
            }
        }
    }
    file.close();

    printf("Loaded %d borrow records from %s\n", borrows.getSize(), filename.c_str());
}

// Function to save borrows to CSV
void saveBorrowsToCSV(Vector<Borrow>& borrows, const std::string& filename)
{
    std::ofstream file(filename.c_str());
    if (!file.is_open())
    {
        printf("Error: Could not open %s for writing.\n", filename.c_str());
        return;
    }

    // Write header
    file << "borrowId,memberId,gameId,gameName,dateBorrowed,dateReturned\n";

    // Write borrows
    for (int i = 0; i < borrows.getSize(); i++)
    {
        Borrow borrow = borrows.get(i);
        file << borrow.borrowId << ","
             << borrow.memberId << ","
             << borrow.gameId << ","
             << escapeCSVField(borrow.gameName) << ","
             << escapeCSVField(borrow.dateBorrowed) << ","
             << escapeCSVField(borrow.dateReturned) << "\n";
    }

    file.close();
}

// Function to get next borrow ID
int getNextBorrowId(Vector<Borrow>& borrows)
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

// Helper function to compare dates (YYYY-MM-DD format)
int compareDates(const std::string& date1, const std::string& date2)
{
    if (date1 < date2)
        return -1;
    else if (date1 > date2)
        return 1;
    else
        return 0;
}

// Helper function to compare two borrows for sorting
int compareBorrows(const Borrow& b1, const Borrow& b2)
{
    // First, compare by dateBorrowed
    int borrowCompare = compareDates(b1.dateBorrowed, b2.dateBorrowed);
    if (borrowCompare != 0)
        return borrowCompare;

    // If dateBorrowed is the same, compare by dateReturned
    return compareDates(b1.dateReturned, b2.dateReturned);
}

// Function to sort borrows vector by datetime borrowed and datetime returned
void sortBorrows(Vector<Borrow>& borrows)
{
    int size = borrows.getSize();

    // Bubble sort
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            Borrow b1 = borrows.get(j);
            Borrow b2 = borrows.get(j + 1);

            if (compareBorrows(b1, b2) > 0)
            {
                // Swap
                Borrow temp = b1;
                borrows[j] = b2;
                borrows[j + 1] = temp;
            }
        }
    }
}

// Function to display a summary of games borrowed/returned
void displayBorrowsSummary(Vector<Borrow>& borrows)
{
    if (borrows.isEmpty())
    {
        printf("No borrow records found.\n");
        return;
    }

    // Create a copy to sort without modifying the original
    Vector<Borrow> sortedBorrows = borrows;
    sortBorrows(sortedBorrows);

    printf("\n=== Borrow Summary (Sorted) ===\n\n");
    printf("%-12s %-10s %-20s %-20s\n", "Member ID", "Game ID", "Date Borrowed", "Date Returned");
    printf("%-12s %-10s %-20s %-20s\n", "-----------", "---------", "-------------------", "-------------------");

    for (int i = 0; i < sortedBorrows.getSize(); i++)
    {
        Borrow borrow = sortedBorrows.get(i);
        printf("%-12d %-10d %-20s %-20s\n",
               borrow.memberId,
               borrow.gameId,
               borrow.dateBorrowed.c_str(),
               borrow.dateReturned.c_str());
    }
    printf("\n");
}

// Forward declaration for Game
struct Game;
class SuffixArray;

// Function to borrow a board game for a member
void memberBorrowGame(int memberId, Vector<Game>& games, Vector<Borrow>& borrows, 
                      SuffixArray& gameNames, Set<GameID>& borrowedGames)
{
    printf("\n=== Borrow Board Game ===\n");

    // Search for games using suffix array
    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;

    // Perform suffix array search
    Vector<int> matchingGameIndices = gameNames.search(searchTerm);

    printf("\nAvailable games matching '%s':\n", searchTerm.c_str());
    Vector<int> availableIndices;

    // Filter out borrowed games
    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = games.get(gameIdx);
        
        if (!borrowedGames.exists(intToString(game.id)))
        {
            availableIndices.append(gameIdx);
        }
    }

    if (availableIndices.isEmpty())
    {
        printf("No available games found matching your search.\n");
        return;
    }

    // Display available games
    for (int i = 0; i < availableIndices.getSize(); i++)
    {
        int gameIdx = availableIndices.get(i);
        Game game = games.get(gameIdx);
        printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
               i, game.id, game.name.c_str(),
               game.minPlayers, game.maxPlayers);
    }

    // Prompt to select a game
    printf("\nEnter the number of the game to borrow (0-%d): ", availableIndices.getSize() - 1);
    int selection;
    scanf("%d", &selection);
    getchar();

    if (selection < 0 || selection >= availableIndices.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    Game selectedGame = games.get(availableIndices.get(selection));

    // Add borrow record
    Borrow newBorrow;
    newBorrow.borrowId = getNextBorrowId(borrows);
    newBorrow.memberId = memberId;
    newBorrow.gameId = selectedGame.id;
    newBorrow.gameName = selectedGame.name;
    newBorrow.dateBorrowed = getCurrentDateTime();
    newBorrow.dateReturned = "N/A";  // Not returned yet
    borrows.append(newBorrow);

    // Mark game as borrowed
    borrowedGames.insert(intToString(selectedGame.id), selectedGame.id);

    printf("Game '%s' borrowed successfully.\n", selectedGame.name.c_str());
    
    // Save changes to CSV
    saveBorrowsToCSV(borrows, "borrows.csv");
}

// Function for a member to return a borrowed game
void memberReturnGame(int memberId, Vector<Game>& games, Vector<Borrow>& borrows, 
                      Set<GameID>& borrowedGames)
{
    printf("\n=== Return Board Game ===\n");

    // Find games borrowed by this member
    Vector<Borrow> memberBorrows;
    Vector<int> borrowIndices;

    for (int i = 0; i < borrows.getSize(); i++)
    {
        Borrow borrow = borrows.get(i);
        if (borrow.memberId == memberId && (borrow.dateReturned == "N/A" || borrow.dateReturned.empty()))
        {
            memberBorrows.append(borrow);
            borrowIndices.append(i);
        }
    }

    if (memberBorrows.isEmpty())
    {
        printf("You have no borrowed games to return.\n");
        return;
    }

    // Display borrowed games
    printf("\nYour borrowed games:\n");
    for (int i = 0; i < memberBorrows.getSize(); i++)
    {
        Borrow borrow = memberBorrows.get(i);
        printf("  %d. Game: %s (ID: %d), Borrowed: %s\n",
               i, borrow.gameName.c_str(), borrow.gameId, borrow.dateBorrowed.c_str());
    }

    // Prompt to select a game to return
    printf("\nEnter the number of the game to return (0-%d): ", memberBorrows.getSize() - 1);
    int selection;
    scanf("%d", &selection);
    getchar();

    if (selection < 0 || selection >= memberBorrows.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    // Update borrow record
    Borrow& selectedBorrow = borrows[borrowIndices.get(selection)];
    selectedBorrow.dateReturned = getCurrentDateTime();

    // Remove from borrowed games
    Borrow borrowToReturn = memberBorrows.get(selection);
    borrowedGames.remove(intToString(borrowToReturn.gameId));

    printf("Game '%s' returned successfully.\n", borrowToReturn.gameName.c_str());
    
    // Save changes to CSV
    saveBorrowsToCSV(borrows, "borrows.csv");
}

// Function to display member's borrow summary
void memberDisplayBorrowSummary(int memberId, Vector<Game>& games, Vector<Borrow>& borrows)
{
    printf("\n=== Your Borrow Summary ===\n");

    // Retrieve records for this member
    Vector<Borrow> memberBorrows;
    for (int i = 0; i < borrows.getSize(); i++)
    {
        Borrow borrow = borrows.get(i);
        if (borrow.memberId == memberId)
        {
            memberBorrows.append(borrow);
        }
    }

    if (memberBorrows.isEmpty())
    {
        printf("You have no borrow records.\n");
        return;
    }

    // Sort by date borrowed then date returned
    int size = memberBorrows.getSize();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            Borrow b1 = memberBorrows.get(j);
            Borrow b2 = memberBorrows.get(j + 1);

            if (compareBorrows(b1, b2) > 0)
            {
                Borrow temp = b1;
                memberBorrows[j] = b2;
                memberBorrows[j + 1] = temp;
            }
        }
    }

    // Display sorted records with game information
    printf("\n%-30s %-20s %-20s\n", "Game Name", "Date Borrowed", "Date Returned");
    printf("%-30s %-20s %-20s\n", "-----------------------------", "-------------------", "-------------------");

    for (int i = 0; i < memberBorrows.getSize(); i++)
    {
        Borrow borrow = memberBorrows.get(i);
        printf("%-30s %-20s %-20s\n",
               borrow.gameName.c_str(),
               borrow.dateBorrowed.c_str(),
               borrow.dateReturned.empty() ? "Not Returned" : borrow.dateReturned.c_str());
    }
    printf("\n");
}

#endif
