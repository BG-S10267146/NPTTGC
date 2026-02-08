#ifndef PAGINATION_HELPER_H
#define PAGINATION_HELPER_H

#include <cstdio>
#include <cstring>
#include <cstddef>
#include "StringHelper.h"
#include "UIHelper.h"

/// Helper class for handling pagination logic
class PaginationHelper
{
private:
    int totalItems;
    int itemsPerPage;
    int totalPages;
    int currentPage;

    /// Validates pagination input
    /// Returns: '0'-'9' if digit selected, 'N' for next, 'P' for previous, 'G' for goto, 'Q' for quit, 'I' for invalid
    char validateInput(const char *input)
    {
        if (!input || strlen(input) == 0)
            return 'I';

        // Single digit (0-9)
        if (strlen(input) == 1 && input[0] >= '0' && input[0] <= '9')
            return input[0];

        // Single character commands (case-insensitive)
        if (strlen(input) == 1)
        {
            char cmd = input[0];
            if (cmd == 'N' || cmd == 'n')
                return 'N';
            if (cmd == 'P' || cmd == 'p')
                return 'P';
            if (cmd == 'G' || cmd == 'g')
                return 'G';
            if (cmd == 'Q' || cmd == 'q')
                return 'Q';
        }

        return 'I'; // Invalid
    }

public:
    /// Constructor
    /// @param totalItems Total number of items to paginate (must be >= 0)
    /// @param itemsPerPage Number of items per page (must be > 0, default: 10)
    PaginationHelper(int totalItems, int itemsPerPage = 10)
        : totalItems(totalItems < 0 ? 0 : totalItems), 
          itemsPerPage(itemsPerPage <= 0 ? 10 : itemsPerPage), 
          currentPage(0)
    {
        if (this->totalItems == 0)
        {
            totalPages = 0;
        }
        else
        {
            totalPages = (this->totalItems + this->itemsPerPage - 1) / this->itemsPerPage;
        }
    }

    /// Gets the start index for the current page
    int getStartIdx() const
    {
        return currentPage * itemsPerPage;
    }

    /// Gets the end index for the current page (exclusive)
    int getEndIdx() const
    {
        int endIdx = getStartIdx() + itemsPerPage;
        return (endIdx > totalItems) ? totalItems : endIdx;
    }

    /// Gets the current page number (0-indexed)
    int getCurrentPage() const
    {
        return currentPage;
    }

    /// Gets the total number of pages
    int getTotalPages() const
    {
        return totalPages;
    }

    /// Gets the total number of items
    int getTotalItems() const
    {
        return totalItems;
    }

    /// Prints pagination information
    void printPageInfo() const
    {
        UIHelper::printPaginationInfo(currentPage, totalPages, getStartIdx(), getEndIdx(), totalItems);
    }

    /// Prints navigation options
    /// @param withSelection If true, includes [0-9] select option
    void printNavigation(bool withSelection) const
    {
        UIHelper::printNavigationOptions(
            withSelection,
            currentPage > 0,
            currentPage < totalPages - 1,
            totalPages > 1);
    }

    /// Handles pagination navigation input
    /// @param withSelection If true, allows item selection (returns 0-9 index)
    /// @return -1 if quit, -2 if navigation only (page changed), or 0-9 for selected item index
    int handleInput(bool withSelection)
    {
        char choice[10];
        printf("Enter your choice: ");
        StringHelper::readLine(choice, sizeof(choice));

        char validatedInput = validateInput(choice);

        if (validatedInput >= '0' && validatedInput <= '9' && withSelection)
        {
            int pageLocalIdx = validatedInput - '0';
            int globalIdx = getStartIdx() + pageLocalIdx;

            if (globalIdx < getEndIdx())
            {
                return pageLocalIdx;
            }
            else
            {
                printf("Item not available on this page.\n");
                return -2;
            }
        }
        else if (validatedInput == 'N')
        {
            if (currentPage < totalPages - 1)
            {
                currentPage++;
            }
            else
            {
                printf("Already at the last page.\n");
            }
            return -2;
        }
        else if (validatedInput == 'P')
        {
            if (currentPage > 0)
            {
                currentPage--;
            }
            else
            {
                printf("Already at the first page.\n");
            }
            return -2;
        }
        else if (validatedInput == 'G')
        {
            if (totalPages <= 1)
            {
                printf("Only one page available.\n");
            }
            else
            {
                printf("Enter page number (1-%d): ", totalPages);
                char pageBuf[10];
                StringHelper::readLine(pageBuf, sizeof(pageBuf));

                // Validate input contains only digits (and optional leading spaces)
                bool validInput = true;
                size_t i = 0;
                while (pageBuf[i] == ' ') i++; // skip leading spaces
                if (pageBuf[i] == '\0')
                {
                    validInput = false;
                }
                else
                {
                    while (pageBuf[i] != '\0')
                    {
                        if (pageBuf[i] < '0' || pageBuf[i] > '9')
                        {
                            validInput = false;
                            break;
                        }
                        i++;
                    }
                }

                if (validInput)
                {
                    int pageNum = atoi(pageBuf);
                    if (pageNum > 0 && pageNum <= totalPages)
                    {
                        currentPage = pageNum - 1;
                    }
                    else
                    {
                        printf("Invalid page number.\n");
                    }
                }
                else
                {
                    printf("Invalid page number.\n");
                }
            }
            return -2;
        }
        else if (validatedInput == 'Q')
        {
            return -1; // Quit
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
            return -2;
        }
    }

    /// Converts a page-local index to a global index
    /// @param pageLocalIdx The local index on the current page (0-9)
    /// @return The global index in the full collection
    int toGlobalIndex(int pageLocalIdx) const
    {
        return getStartIdx() + pageLocalIdx;
    }
};

#endif
