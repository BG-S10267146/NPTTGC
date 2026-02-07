#include "Screen.h"
#include "utils/Input.h"
#include <cstdio>
#include <cstring>
#include <ctime>

Screen::Screen(AppState &state) : appState(state) {}

void Screen::startup()
{
    printf("====================================\n");
    printf("  NPTTGC Board Game Club System\n");
    printf("====================================\n\n");

    printf("Loading data from CSV files...\n");
    appState.loadData();
    printf("\nData loaded successfully!\n");

    login();
}

void Screen::login()
{
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
            return;
        }

        std::optional<Member> currentUser = appState.authenticateMember(std::string(usernameBuf));

        if (!currentUser.has_value())
        {
            printf("\nInvalid username. Please try again.\n");
            continue;
        }

        printf("\nWelcome, %s!\n", currentUser->username.c_str());

        if (currentUser->isAdmin)
        {
            adminMenu();
        }
        else
        {
            memberMenu();
        }

        appState.logout();
    }
}

void Screen::adminMenu()
{
    bool loggedIn = true;
    while (loggedIn)
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

        int choice;
        if (!readInteger(choice))
        {
            printf("Invalid selection. Please enter a number.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            addGame();
            break;
        case 2:
            removeGame();
            break;
        case 3:
            addMember();
            break;
        case 4:
            borrowsSummary();
            break;
        case 5:
            gamesByPlayerCount();
            break;
        case 6:
            viewReviews();
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

void Screen::memberMenu()
{
    bool loggedIn = true;
    while (loggedIn)
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

        int choice;
        if (!readInteger(choice))
        {
            printf("Invalid selection. Please enter a number.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            borrowGame();
            break;
        case 2:
            returnGame();
            break;
        case 3:
            myBorrowSummary();
            break;
        case 4:
            writeReview();
            break;
        case 5:
            viewReviews();
            break;
        case 6:
            gamesByPlayerCount();
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

void Screen::addGame()
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

    std::time_t t = std::time(nullptr);
    std::tm now;
    localtime_s(&now, &t);
    int currentYear = now.tm_year + 1900;

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

    Game newGame;
    newGame.id = appState.getGames().getSize() + 1;
    newGame.name = nameBuf;
    newGame.minPlayers = minPlayers;
    newGame.maxPlayers = maxPlayers;
    newGame.minPlaytime = minPlaytime;
    newGame.maxPlaytime = maxPlaytime;
    newGame.yearPublished = yearPublished;
    newGame.isDeleted = false;

    if (appState.addGame(newGame))
    {
        printf("Game '%s' added successfully with ID %d.\n", newGame.name.c_str(), newGame.id);
    }
    else
    {
        printf("Failed to add game.\n");
    }
}

void Screen::removeGame()
{
    printf("\n=== Remove Board Game ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;

    Vector<int> matchingGameIndices = appState.searchGames(searchTerm);

    if (matchingGameIndices.isEmpty())
    {
        printf("No games found matching your search.\n");
        return;
    }

    printf("\nMatching games:\n");
    Vector<Game> activeGames;
    const Vector<Game> &allGames = appState.getGames();

    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = allGames.get(gameIdx);
        if (!game.isDeleted)
        {
            printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
                   activeGames.getSize(), game.id, game.name.c_str(), game.minPlayers, game.maxPlayers);
            activeGames.append(game);
        }
    }

    if (activeGames.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    printf("\nEnter the number of the game to remove (0-%d): ", activeGames.getSize() - 1);
    int selection;
    scanf_s("%d", &selection);
    getchar();

    if (selection < 0 || selection >= activeGames.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    Game gameToRemove = activeGames.get(selection);

    if (appState.isGameBorrowed(gameToRemove.id))
    {
        printf("Cannot remove game '%s' (ID: %d) because it is currently borrowed and not returned.\n",
               gameToRemove.name.c_str(), gameToRemove.id);
        return;
    }

    if (appState.removeGame(gameToRemove.id))
    {
        printf("Game '%s' (ID: %d) removed successfully.\n", gameToRemove.name.c_str(), gameToRemove.id);
    }
    else
    {
        printf("Failed to remove game.\n");
    }
}

void Screen::addMember()
{
    printf("\n=== Add New Member ===\n");

    char usernameBuf[100];

    printf("Enter username: ");
    fgets(usernameBuf, sizeof(usernameBuf), stdin);
    usernameBuf[strcspn(usernameBuf, "\n")] = 0;

    printf("Is this an admin account? (y/n): ");
    char isAdminChoice;
    scanf_s("%c", &isAdminChoice);
    getchar();

    bool isAdmin = (isAdminChoice == 'y' || isAdminChoice == 'Y');

    if (appState.addMember(std::string(usernameBuf), isAdmin))
    {
        printf("Member '%s' added successfully.\n", usernameBuf);
    }
    else
    {
        printf("Error: Username '%s' already exists.\n", usernameBuf);
    }
}

void Screen::borrowsSummary()
{
    Vector<Borrow> allBorrows = appState.getAllBorrows();

    if (allBorrows.isEmpty())
    {
        printf("No borrow records found.\n");
        return;
    }

    int totalBorrows = allBorrows.getSize();
    int borrowsPerPage = 10;
    int totalPages = (totalBorrows + borrowsPerPage - 1) / borrowsPerPage;
    int currentPage = 0;

    while (true)
    {
        int startIdx = currentPage * borrowsPerPage;
        int endIdx = startIdx + borrowsPerPage;
        if (endIdx > totalBorrows)
            endIdx = totalBorrows;

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
            Borrow borrow = allBorrows.get(i);
            std::string status = (borrow.dateReturned == "N/A" || borrow.dateReturned.empty()) ? "Borrowed" : "Returned";
            printf("%-20s %-30s %-30s %-20s %-20s\n",
                   appState.getMemberNameById(borrow.memberId).c_str(),
                   appState.getGameNameById(borrow.gameId).c_str(),
                   borrow.dateBorrowed.c_str(),
                   borrow.dateReturned.c_str(),
                   status.c_str());
        }
        printf("=============================================================================\n");

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

void Screen::gamesByPlayerCount()
{
    printf("\n=== Display Games by Player Count ===\n");

    int playerCount;
    printf("Enter number of players: ");
    if (!readInteger(playerCount))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (playerCount <= 0)
    {
        printf("Invalid number of players.\n");
        return;
    }

    Vector<Game> matchingGames = appState.getGamesForPlayerCount(playerCount);

    if (matchingGames.isEmpty())
    {
        printf("No games found that can be played with %d player(s).\n", playerCount);
        return;
    }

    int totalGames = matchingGames.getSize();
    int gamesPerPage = 10;
    int totalPages = (totalGames + gamesPerPage - 1) / gamesPerPage;
    int currentPage = 0;

    while (true)
    {
        int startIdx = currentPage * gamesPerPage;
        int endIdx = startIdx + gamesPerPage;
        if (endIdx > totalGames)
            endIdx = totalGames;

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

void Screen::viewReviews()
{
    printf("\n=== View Game Reviews ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;

    Vector<int> matchingGameIndices = appState.searchGames(searchTerm);

    if (matchingGameIndices.isEmpty())
    {
        printf("No games found matching your search.\n");
        return;
    }

    printf("\nMatching games:\n");
    Vector<Game> activeGames;
    const Vector<Game> &allGames = appState.getGames();

    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = allGames.get(gameIdx);
        if (!game.isDeleted)
        {
            printf("  %d. ID: %d, Name: %s\n", activeGames.getSize(), game.id, game.name.c_str());
            activeGames.append(game);
        }
    }

    if (activeGames.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    printf("\nEnter the number of the game to view reviews (0-%d): ", activeGames.getSize() - 1);
    int selection;
    if (!readInteger(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (selection < 0 || selection >= activeGames.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    Game selectedGame = activeGames.get(selection);
    Vector<Review> gameReviews = appState.getReviewsForGame(selectedGame.id);

    printf("\n=== Reviews for Game ID %d ===\n\n", selectedGame.id);

    if (gameReviews.isEmpty())
    {
        printf("No reviews found for this game.\n");
        return;
    }

    printf("%-15s %-8s %-40s\n", "Username", "Rating", "Content");
    printf("%-15s %-8s %-40s\n", "--------", "------", "----------------------------------------");

    for (int i = 0; i < gameReviews.getSize(); i++)
    {
        Review review = gameReviews.get(i);
        std::string username = appState.getMemberNameById(review.userId);
        printf("%-15s %-8d %-40s\n", username.c_str(), review.rating, review.content.c_str());
    }
    printf("\n");

    float averageRating = appState.getAverageRating(selectedGame.id);
    printf("Average rating for Game ID %d: %.2f/5 (%d reviews)\n",
           selectedGame.id, averageRating, gameReviews.getSize());
}

void Screen::borrowGame()
{
    printf("\n=== Borrow Board Game ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;

    Vector<int> matchingGameIndices = appState.searchGames(searchTerm);

    printf("\nAvailable games matching '%s':\n", searchTerm.c_str());
    Vector<Game> availableGames;
    const Vector<Game> &allGames = appState.getGames();

    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = allGames.get(gameIdx);

        if (!game.isDeleted && !appState.isGameBorrowed(game.id))
        {
            printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
                   availableGames.getSize(), game.id, game.name.c_str(),
                   game.minPlayers, game.maxPlayers);
            availableGames.append(game);
        }
    }

    if (availableGames.isEmpty())
    {
        printf("No available games found matching your search.\n");
        return;
    }

    printf("\nEnter the number of the game to borrow (0-%d): ", availableGames.getSize() - 1);
    int selection;
    if (!readInteger(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (selection < 0 || selection >= availableGames.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    Game selectedGame = availableGames.get(selection);

    if (appState.borrowGame(selectedGame.id))
    {
        printf("Game '%s' borrowed successfully.\n", selectedGame.name.c_str());
    }
    else
    {
        printf("Failed to borrow game.\n");
    }
}

void Screen::returnGame()
{
    printf("\n=== Return Board Game ===\n");

    Vector<Borrow> memberBorrows = appState.getMemberBorrows();
    Vector<Borrow> activeBorrows;

    for (int i = 0; i < memberBorrows.getSize(); i++)
    {
        Borrow borrow = memberBorrows.get(i);
        if (borrow.dateReturned == "N/A" || borrow.dateReturned.empty())
        {
            activeBorrows.append(borrow);
        }
    }

    if (activeBorrows.isEmpty())
    {
        printf("You have no borrowed games to return.\n");
        return;
    }

    printf("\nYour borrowed games:\n");
    for (int i = 0; i < activeBorrows.getSize(); i++)
    {
        Borrow borrow = activeBorrows.get(i);
        printf("  %d. Game: %s (ID: %d), Borrowed: %s\n",
               i, appState.getGameNameById(borrow.gameId).c_str(), borrow.gameId, borrow.dateBorrowed.c_str());
    }

    printf("\nEnter the number of the game to return (0-%d): ", activeBorrows.getSize() - 1);
    int selection;
    if (!readInteger(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (selection < 0 || selection >= activeBorrows.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    Borrow borrowToReturn = activeBorrows.get(selection);

    if (appState.returnGame(borrowToReturn.borrowId))
    {
        printf("Game '%s' returned successfully.\n", appState.getGameNameById(borrowToReturn.gameId).c_str());
    }
    else
    {
        printf("Failed to return game.\n");
    }
}

void Screen::myBorrowSummary()
{
    printf("\n=== Your Borrow Summary ===\n");

    Vector<Borrow> memberBorrows = appState.getMemberBorrows();

    if (memberBorrows.isEmpty())
    {
        printf("You have no borrow records.\n");
        return;
    }

    printf("\n%-30s %-20s %-20s\n", "Game Name", "Date Borrowed", "Date Returned");
    printf("%-30s %-20s %-20s\n", "-----------------------------", "-------------------", "-------------------");

    for (int i = 0; i < memberBorrows.getSize(); i++)
    {
        Borrow borrow = memberBorrows.get(i);
        printf("%-30s %-20s %-20s\n",
               appState.getGameNameById(borrow.gameId).c_str(),
               borrow.dateBorrowed.c_str(),
               borrow.dateReturned.empty() ? "Not Returned" : borrow.dateReturned.c_str());
    }
    printf("\n");
}

void Screen::writeReview()
{
    printf("\n=== Write a Review ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    fgets(searchBuf, sizeof(searchBuf), stdin);
    searchBuf[strcspn(searchBuf, "\n")] = 0;
    std::string searchTerm = searchBuf;

    Vector<int> matchingGameIndices = appState.searchGames(searchTerm);

    if (matchingGameIndices.isEmpty())
    {
        printf("No games found matching your search.\n");
        return;
    }

    printf("\nMatching games:\n");
    Vector<Game> activeGames;
    const Vector<Game> &allGames = appState.getGames();

    for (int i = 0; i < matchingGameIndices.getSize(); i++)
    {
        int gameIdx = matchingGameIndices.get(i);
        Game game = allGames.get(gameIdx);
        if (!game.isDeleted)
        {
            printf("  %d. ID: %d, Name: %s, Players: %d-%d\n",
                   activeGames.getSize(), game.id, game.name.c_str(), game.minPlayers, game.maxPlayers);
            activeGames.append(game);
        }
    }

    if (activeGames.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    printf("\nEnter the number of the game to review (0-%d): ", activeGames.getSize() - 1);
    int selection;
    if (!readInteger(selection))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (selection < 0 || selection >= activeGames.getSize())
    {
        printf("Invalid selection.\n");
        return;
    }

    Game selectedGame = activeGames.get(selection);

    printf("\n=== Write Review ===\n");

    printf("Enter rating (1-5): ");
    int rating;
    if (!readInteger(rating))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (rating < 1 || rating > 5)
    {
        printf("Invalid rating. Please enter a value between 1 and 5.\n");
        return;
    }

    char contentBuf[500];
    printf("Enter review content: ");
    fgets(contentBuf, sizeof(contentBuf), stdin);
    contentBuf[strcspn(contentBuf, "\n")] = 0;

    if (appState.addReview(selectedGame.id, rating, std::string(contentBuf)))
    {
        printf("Review submitted successfully!\n");
    }
    else
    {
        printf("Failed to submit review.\n");
    }
}
