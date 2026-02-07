#ifndef SCREEN_H
#define SCREEN_H

#include "AppState.h"

/// Presentation layer - handles all UI and user interaction
class Screen
{
private:
    AppState& appState;
    
    // Helper function to validate pagination input
    // Returns: 0-9 if digit selected, 'N' for next, 'P' for previous, 'G' for goto, 'Q' for quit, 'I' for invalid
    char validatePaginationInput(const char* input);

public:
    Screen(AppState& state);

    void startup();
    void login();

    void adminMenu();
    void addGame();
    void removeGame();
    void addMember();
    void borrowsSummary();
    void gamesByPlayerCount();
    void viewReviews();

    void memberMenu();
    void borrowGame();
    void returnGame();
    void myBorrowSummary();
    void writeReview();
};

#endif
