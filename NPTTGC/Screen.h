#ifndef SCREEN_H
#define SCREEN_H

#include "AppState.h"

/// Presentation layer - handles all UI and user interaction
class Screen
{
private:
    AppState& appState;

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
