#include "Search.h"
#include <functional>

/**
 * Binary search on a range [0, size) with a custom comparison function
 * @param size The size of the range to search [0, size)
 * @param compare Custom comparison function that takes an index and returns:
 *                0 if match found, -1 to search right, 1 to search left
 * @param returnNotFound If true, returns -1 when not found; otherwise returns insertion point
 * @return The index of the match if found, or -1/insertion point depending on returnNotFound
 */
int binarySearch(int size, std::function<int(int)> compare, bool returnNotFound)
{
    int left = 0, right = size;

    while (left < right)
    {
        int mid = (left + right) / 2;
        int order = compare(mid);

        if (order == 0)
        {
            return mid;
        }
        else if (order == -1)
        {
            left = mid + 1;
        }
        else
        {
            right = mid;
        }
    }

    return returnNotFound ? -1 : left;
}