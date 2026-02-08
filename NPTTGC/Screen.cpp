#include "Screen.h"
#include "utils/Input.h"
#include "utils/StringHelper.h"
#include "utils/UIHelper.h"
#include "utils/PaginationHelper.h"
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
        StringHelper::readLine(usernameBuf, sizeof(usernameBuf));

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
    StringHelper::readLine(nameBuf, sizeof(nameBuf));

    if (strlen(nameBuf) == 0)
    {
        printf("Error: Game name cannot be empty.\n");
        return;
    }

    // Check if game with this name already exists (non-deleted)
    int existingGameIndex = -1;
    Vector<Game> allGames = appState.getGames();
    for (int i = 0; i < allGames.getSize(); i++)
    {
        if (allGames.get(i).name == std::string(nameBuf) && !allGames.get(i).isDeleted)
        {
            existingGameIndex = i;
            break;
        }
    }

    int minPlayers, maxPlayers, minPlaytime, maxPlaytime, yearPublished;

    if (existingGameIndex != -1)
    {
        Game &existingGame = allGames[existingGameIndex];
        printf("\nGame '%s' already exists with the following parameters:\n", existingGame.name.c_str());
        printf("  Min Players: %d\n", existingGame.minPlayers);
        printf("  Max Players: %d\n", existingGame.maxPlayers);
        printf("  Min Playtime: %d minutes\n", existingGame.minPlaytime);
        printf("  Max Playtime: %d minutes\n", existingGame.maxPlaytime);
        printf("  Year Published: %d\n", existingGame.yearPublished);
        printf("\nAll parameters must match the existing game to add a duplicate copy.\n");
        printf("Press Enter to use these values or type 'cancel' to abort.\n");

        char confirmBuf[20];
        StringHelper::readLine(confirmBuf, sizeof(confirmBuf));

        if (strcmp(confirmBuf, "cancel") == 0)
        {
            printf("Game creation cancelled.\n");
            return;
        }

        // Autofill from existing game
        minPlayers = existingGame.minPlayers;
        maxPlayers = existingGame.maxPlayers;
        minPlaytime = existingGame.minPlaytime;
        maxPlaytime = existingGame.maxPlaytime;
        yearPublished = existingGame.yearPublished;
    }
    else
    {
        // New game - prompt for all parameters
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
        StringHelper::readLine(yearBuf, sizeof(yearBuf));

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
    }

    Game newGame;
    newGame.id = appState.games.maxKey() + 1;
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
    StringHelper::readLine(searchBuf, sizeof(searchBuf));
    std::string searchTerm = searchBuf;

    Vector<Game> activeGames = appState.searchGames(
        searchTerm,
        [](const Game &g)
        { return !g.isDeleted; });

    if (activeGames.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    PaginationHelper pagination(activeGames.getSize(), 10);
    int selectedGame = -1;

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Matching games for '%s'\n", searchTerm.c_str());
        pagination.printPageInfo();
        UIHelper::printSeparator();

        for (int i = startIdx; i < endIdx; i++)
        {
            Game game = activeGames.get(i);
            int pageLocalIdx = i - startIdx;
            printf("%d. Name: %s, Players: %d-%d\n",
                   pageLocalIdx, game.name.c_str(), game.minPlayers, game.maxPlayers);
        }
        UIHelper::printSeparator();

        pagination.printNavigation(true);

        int result = pagination.handleInput(true);
        if (result == -1)
        {
            return; // Quit
        }
        else if (result >= 0)
        {
            selectedGame = pagination.toGlobalIndex(result);
            break;
        }
        // result == -2 means navigation occurred, continue loop
    }

    if (selectedGame >= 0)
    {
        Game gameToRemove = activeGames.get(selectedGame);

        if (appState.isGameBorrowed(gameToRemove.id))
        {
            printf("Cannot remove game '%s' because it is currently borrowed and not returned.\n",
                   gameToRemove.name.c_str());
            return;
        }

        if (appState.removeGame(gameToRemove.id))
        {
            printf("Game '%s' removed successfully.\n", gameToRemove.name.c_str());
        }
        else
        {
            printf("Failed to remove game.\n");
        }
    }
}

void Screen::addMember()
{
    printf("\n=== Add New Member ===\n");

    char usernameBuf[100];

    printf("Enter username: ");
    StringHelper::readLine(usernameBuf, sizeof(usernameBuf));

    printf("Is this an admin account? (y/n): ");
    char isAdminChoice;
    scanf_s("%c", &isAdminChoice, 1);
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

    PaginationHelper pagination(allBorrows.getSize(), 10);

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator(77);
        printf("Borrow Summary (Sorted by Date)\n");
        pagination.printPageInfo();
        UIHelper::printSeparator(77);
        printf("\n");
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
        UIHelper::printSeparator(77);

        pagination.printNavigation(false);

        int result = pagination.handleInput(false);
        if (result == -1)
        {
            break; // Quit
        }
        // result == -2 means navigation occurred, continue loop
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

    PaginationHelper pagination(matchingGames.getSize(), 10);

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Games that can be played with %d player(s) (sorted alphabetically)\n", playerCount);
        pagination.printPageInfo();
        UIHelper::printSeparator();

        for (int i = startIdx; i < endIdx; i++)
        {
            Game game = matchingGames.get(i);
            printf("%d. %s\n", i + 1, game.name.c_str());
            printf("   Players: %d-%d | Playtime: %d-%d min | Year: %d\n",
                   game.minPlayers, game.maxPlayers,
                   game.minPlaytime, game.maxPlaytime,
                   game.yearPublished);
        }
        UIHelper::printSeparator();

        pagination.printNavigation(false);

        int result = pagination.handleInput(false);
        if (result == -1)
        {
            break; // Quit
        }
        // result == -2 means navigation occurred, continue loop
    }
}

void Screen::viewReviews()
{
    printf("\n=== View Game Reviews ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    StringHelper::readLine(searchBuf, sizeof(searchBuf));
    std::string searchTerm = searchBuf;

    Set<std::string> seenGameNames;
    Vector<Game> activeGames = appState.searchGames(
        searchTerm,
        [&](const Game &g)
        {
            bool include = !g.isDeleted && !seenGameNames.exists(g.name);
            if (include)
            {
                seenGameNames.insert(g.name);
            }
            return include;
        });

    if (activeGames.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    PaginationHelper pagination(activeGames.getSize(), 10);
    int selectedGame = -1;

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Matching games for '%s'\n", searchTerm.c_str());
        pagination.printPageInfo();
        UIHelper::printSeparator();

        for (int i = startIdx; i < endIdx; i++)
        {
            Game game = activeGames.get(i);
            int pageLocalIdx = i - startIdx;
            printf("%d. Name: %s\n", pageLocalIdx, game.name.c_str());
        }
        UIHelper::printSeparator();

        pagination.printNavigation(true);

        int result = pagination.handleInput(true);
        if (result == -1)
        {
            return; // Quit
        }
        else if (result >= 0)
        {
            selectedGame = pagination.toGlobalIndex(result);
            break;
        }
        // result == -2 means navigation occurred, continue loop
    }

    if (selectedGame >= 0)
    {
        Game selectedGameObj = activeGames.get(selectedGame);
        Vector<Review> gameReviews = appState.getReviewsForGameName(selectedGameObj.name);

        printf("\n=== Reviews for Game: %s ===\n\n", selectedGameObj.name.c_str());

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

        float averageRating = appState.getAverageRatingByGameName(selectedGameObj.name);
        printf("Average rating for %s: %.2f/5 (%d reviews)\n",
               selectedGameObj.name.c_str(), averageRating, gameReviews.getSize());
    }
}

void Screen::borrowGame()
{
    printf("\n=== Borrow Board Game ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    StringHelper::readLine(searchBuf, sizeof(searchBuf));
    std::string searchTerm = searchBuf;

    Vector<Game> availableGames = appState.searchGames(
        searchTerm,
        [&](const Game &g)
        { return !g.isDeleted && !appState.isGameBorrowed(g.id); });

    if (availableGames.isEmpty())
    {
        printf("No available games found matching your search.\n");
        return;
    }

    PaginationHelper pagination(availableGames.getSize(), 10);
    int selectedGame = -1;

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Available games matching '%s'\n", searchTerm.c_str());
        pagination.printPageInfo();
        UIHelper::printSeparator();

        for (int i = startIdx; i < endIdx; i++)
        {
            Game game = availableGames.get(i);
            int pageLocalIdx = i - startIdx;
            printf("%d. Name: %s, Players: %d-%d\n",
                   pageLocalIdx, game.name.c_str(), game.minPlayers, game.maxPlayers);
        }
        UIHelper::printSeparator();

        pagination.printNavigation(true);

        int result = pagination.handleInput(true);
        if (result == -1)
        {
            return; // Quit
        }
        else if (result >= 0)
        {
            selectedGame = pagination.toGlobalIndex(result);
            break;
        }
        // result == -2 means navigation occurred, continue loop
    }

    if (selectedGame >= 0)
    {
        Game selectedGameObj = availableGames.get(selectedGame);

        if (appState.borrowGame(selectedGameObj.id))
        {
            printf("Game '%s' borrowed successfully.\n", selectedGameObj.name.c_str());
        }
        else
        {
            printf("Failed to borrow game.\n");
        }
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

    PaginationHelper pagination(activeBorrows.getSize(), 10);
    int selectedBorrow = -1;

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Your borrowed games\n");
        pagination.printPageInfo();
        UIHelper::printSeparator();

        for (int i = startIdx; i < endIdx; i++)
        {
            Borrow borrow = activeBorrows.get(i);
            int pageLocalIdx = i - startIdx;
            printf("%d. Game: %s, Borrowed: %s\n",
                   pageLocalIdx, appState.getGameNameById(borrow.gameId).c_str(), borrow.dateBorrowed.c_str());
        }
        UIHelper::printSeparator();

        pagination.printNavigation(true);

        int result = pagination.handleInput(true);
        if (result == -1)
        {
            return; // Quit
        }
        else if (result >= 0)
        {
            selectedBorrow = pagination.toGlobalIndex(result);
            break;
        }
        // result == -2 means navigation occurred, continue loop
    }

    if (selectedBorrow >= 0)
    {
        Borrow borrowToReturn = activeBorrows.get(selectedBorrow);

        if (appState.returnGame(borrowToReturn.borrowId))
        {
            printf("Game '%s' returned successfully.\n", appState.getGameNameById(borrowToReturn.gameId).c_str());
        }
        else
        {
            printf("Failed to return game.\n");
        }
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

    PaginationHelper pagination(memberBorrows.getSize(), 10);

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Your Borrow Summary\n");
        pagination.printPageInfo();
        UIHelper::printSeparator();
        printf("\n");
        printf("%-30s %-20s %-20s\n", "Game Name", "Date Borrowed", "Date Returned");
        printf("%-30s %-20s %-20s\n", "-----------------------------", "-------------------", "-------------------");

        for (int i = startIdx; i < endIdx; i++)
        {
            Borrow borrow = memberBorrows.get(i);
            printf("%-30s %-20s %-20s\n",
                   appState.getGameNameById(borrow.gameId).c_str(),
                   borrow.dateBorrowed.c_str(),
                   borrow.dateReturned.empty() ? "Not Returned" : borrow.dateReturned.c_str());
        }
        UIHelper::printSeparator();

        pagination.printNavigation(false);

        int result = pagination.handleInput(false);
        if (result == -1)
        {
            break; // Quit
        }
        // result == -2 means navigation occurred, continue loop
    }
}

void Screen::writeReview()
{
    printf("\n=== Write a Review ===\n");

    char searchBuf[100];
    printf("Enter game name to search: ");
    StringHelper::readLine(searchBuf, sizeof(searchBuf));
    std::string searchTerm = searchBuf;

    Set<std::string> seenGameNames;
    Vector<Game> activeGames = appState.searchGames(
        searchTerm,
        [&](const Game &g)
        {
            bool include = !g.isDeleted && !seenGameNames.exists(g.name);
            if (include)
            {
                seenGameNames.insert(g.name);
            }
            return include;
        });

    if (activeGames.isEmpty())
    {
        printf("No active games found matching your search.\n");
        return;
    }

    PaginationHelper pagination(activeGames.getSize(), 10);
    int selectedGame = -1;

    while (true)
    {
        int startIdx = pagination.getStartIdx();
        int endIdx = pagination.getEndIdx();

        printf("\n");
        UIHelper::printSeparator();
        printf("Matching games for '%s'\n", searchTerm.c_str());
        pagination.printPageInfo();
        UIHelper::printSeparator();

        for (int i = startIdx; i < endIdx; i++)
        {
            Game game = activeGames.get(i);
            int pageLocalIdx = i - startIdx;
            printf("%d. Name: %s, Players: %d-%d\n",
                   pageLocalIdx, game.name.c_str(), game.minPlayers, game.maxPlayers);
        }
        UIHelper::printSeparator();

        pagination.printNavigation(true);

        int result = pagination.handleInput(true);
        if (result == -1)
        {
            return; // Quit
        }
        else if (result >= 0)
        {
            selectedGame = pagination.toGlobalIndex(result);
            break;
        }
        // result == -2 means navigation occurred, continue loop
    }

    if (selectedGame >= 0)
    {
        Game selectedGameObj = activeGames.get(selectedGame);

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
        StringHelper::readLine(contentBuf, sizeof(contentBuf));

        if (appState.addReview(selectedGameObj.id, rating, std::string(contentBuf)))
        {
            printf("Review submitted successfully!\n");
        }
        else
        {
            printf("Failed to submit review.\n");
        }
    }
}
