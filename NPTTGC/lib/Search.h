#ifndef SEARCH_H
#define SEARCH_H

#include <functional>

/*
 *Binary search on a range [0, size) with a custom comparison function.
 *The compare function should return: 0 (match), -1 (search right), or 1 (search left).
 * If returnNotFound is true, returns -1 when not found; otherwise returns the insertion point.
*/
int binarySearch(int size, std::function<int(int)> compare, bool returnNotFound = true);

#endif
