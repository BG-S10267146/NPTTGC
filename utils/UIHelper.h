#ifndef UI_HELPER_H
#define UI_HELPER_H

#include <cstdio>

/// Helper functions for UI display and formatting
namespace UIHelper
{
    /// Prints a separator line with equals signs
    /// @param length Length of the separator line (default: 65)
    inline void printSeparator(int length = 65)
    {
        for (int i = 0; i < length; i++)
        {
            printf("=");
        }
        printf("\n");
    }

    /// Prints pagination info line
    /// @param currentPage Current page number (1-indexed)
    /// @param totalPages Total number of pages
    /// @param startIdx Start index of items on current page (1-indexed)
    /// @param endIdx End index of items on current page (inclusive, 1-indexed)
    /// @param totalItems Total number of items
    inline void printPaginationInfo(int currentPage, int totalPages, int startIdx, int endIdx, int totalItems)
    {
        printf("Page %d of %d (Showing %d-%d of %d)\n",
               currentPage + 1, totalPages, startIdx + 1, endIdx, totalItems);
    }

    /// Prints navigation options for pagination
    /// @param withSelection If true, includes [0-9] select option
    /// @param hasPrevious If true, includes [P] previous page option
    /// @param hasNext If true, includes [N] next page option
    /// @param hasGoTo If true, includes [G] go to page option
    inline void printNavigationOptions(bool withSelection, bool hasPrevious, bool hasNext, bool hasGoTo)
    {
        printf("\nNavigation Options:\n");
        if (withSelection)
        {
            printf("  [0-9] Select item on this page\n");
        }
        if (hasPrevious)
        {
            printf("  [P] Previous page\n");
        }
        if (hasNext)
        {
            printf("  [N] Next page\n");
        }
        if (hasGoTo)
        {
            printf("  [G] Go to page number\n");
        }
        printf("  [Q] Quit/Return to menu\n");
    }
}

#endif
