#ifndef BORROW_H
#define BORROW_H

#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
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
    std::string dateBorrowed;
    std::string dateReturned;
};

// Global data structures
Vector<Borrow> borrows;
Set<GameID> borrowedGames;

// Helper function to safely read an integer from input
inline bool readIntegerBorrow(int& value)
{
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) == nullptr)
        return false;
    
    char* endPtr;
    value = strtol(buffer, &endPtr, 10);
    
    while (*endPtr && isspace(*endPtr))
        endPtr++;
    
    return (*endPtr == '\0');
}

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
        if (fields.getSize() >= 5)
        {
            Borrow borrow;
            borrow.borrowId = atoi(fields.get(0).c_str());
            borrow.memberId = atoi(fields.get(1).c_str());
            borrow.gameId = atoi(fields.get(2).c_str());
            int offset = (fields.getSize() >= 6) ? 1 : 0; // skip gameName if present
            borrow.dateBorrowed = trim(fields.get(3 + offset));
            borrow.dateReturned = trim(fields.get(4 + offset));
            
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
    file << "borrowId,memberId,gameId,dateBorrowed,dateReturned\n";

    // Write borrows
    for (int i = 0; i < borrows.getSize(); i++)
    {
        Borrow borrow = borrows.get(i);
        file << borrow.borrowId << ","
             << borrow.memberId << ","
             << borrow.gameId << ","
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

// Helper function to get game name by ID
std::string getGameNameById(const Vector<Game>& games, int gameId)
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

// Helper function to get member name by ID
std::string getMemberNameById(const Vector<Member>& members, int memberId)
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

// Function to display a summary of games borrowed/returned with pagination
void displayBorrowsSummary(Vector<Borrow>& borrows, Vector<Game>& games, Vector<Member>& members)
{
    if (borrows.isEmpty())
    {
        printf("No borrow records found.\n");
        return;
    }

    // Create a copy to sort without modifying the original
    Vector<Borrow> sortedBorrows = borrows;
    sortBorrows(sortedBorrows);

    // Pagination setup
    int totalBorrows = sortedBorrows.getSize();
    int borrowsPerPage = 10;
    int totalPages = (totalBorrows + borrowsPerPage - 1) / borrowsPerPage;
    int currentPage = 0;

    while (true)
    {
        // Calculate range for current page
        int startIdx = currentPage * borrowsPerPage;
        int endIdx = startIdx + borrowsPerPage;
        if (endIdx > totalBorrows)
            endIdx = totalBorrows;

        // Display current page
        printf("\n=============================================================================\n");
        printf("Borrow Summary (Sorted by Date)\n");
        printf("Page %d of %d (Showing %d-%d of %d records)\n",
               currentPage + 1, totalPages, startIdx + 1, endIdx, totalBorrows);
        printf("=============================================================================\n\n");
        printf("%-20s %-30s %-30s %-20s %-20s\n", 
               "Member", "Game", "Date Borrowed", "Date Returned", "Status");
        printf("%-20s %-30s %-30s %-20s %-20s\n",
               "--------------------", "------------------------------", "------------------------------",
               "--------------------", "--------------------");

        for (int i = startIdx; i < endIdx; i++)
        {
            Borrow borrow = sortedBorrows.get(i);
            std::string status = (borrow.dateReturned == "N/A" || borrow.dateReturned.empty()) ? "Borrowed" : "Returned";
            printf("%-20s %-30s %-30s %-20s %-20s\n",
                   getMemberNameById(members, borrow.memberId).c_str(),
                   getGameNameById(games, borrow.gameId).c_str(),
                   borrow.dateBorrowed.c_str(),
                   borrow.dateReturned.c_str(),
                   status.c_str());
        }
        printf("=============================================================================\n");

        // Navigation options
        printf("\nNavigation Options:\n");
        if (currentPage > 0)
            printf("  [P] Previous page\n");
        if (currentPage < totalPages - 1)
            printf("  [N] Next page\n");
        printf("  [1-%d] Go to page number\n", totalPages);
        printf("  [Q] Quit/Return to menu\n");
        printf("Enter your choice: ");

        char choice[10];
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;

        // Check if input is a number
        int pageNum = atoi(choice);
        if (pageNum > 0 && pageNum <= totalPages)
        {
            currentPage = pageNum - 1;
        }
        else if (choice[0] == 'N' || choice[0] == 'n')
        {
            if (currentPage < totalPages - 1)
                currentPage++;
            else
                printf("Already at the last page.\n");
        }
        else if (choice[0] == 'P' || choice[0] == 'p')
        {
            if (currentPage > 0)
                currentPage--;
            else
                printf("Already at the first page.\n");
        }
        else if (choice[0] == 'Q' || choice[0] == 'q')
        {
            break;
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }
    }
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
        
        if (!game.isDeleted && !borrowedGames.exists(intToString(game.id)))
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
    if (!readIntegerBorrow(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

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
               i, getGameNameById(games, borrow.gameId).c_str(), borrow.gameId, borrow.dateBorrowed.c_str());
    }

    // Prompt to select a game to return
    printf("\nEnter the number of the game to return (0-%d): ", memberBorrows.getSize() - 1);
    int selection;
    if (!readIntegerBorrow(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

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

    printf("Game '%s' returned successfully.\n", getGameNameById(games, borrowToReturn.gameId).c_str());
    
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
               getGameNameById(games, borrow.gameId).c_str(),
               borrow.dateBorrowed.c_str(),
               borrow.dateReturned.empty() ? "Not Returned" : borrow.dateReturned.c_str());
    }
    printf("\n");
}

#endif
