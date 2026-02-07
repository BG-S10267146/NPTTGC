#include "AppState.h"
#include "Screen.h"

int main()
{
    AppState appState;
    Screen screen(appState);
    screen.startup();
    return 0;
}
