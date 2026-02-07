#include "Search.h"
#include <functional>

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