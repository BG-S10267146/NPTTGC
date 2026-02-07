#ifndef SEARCH_H
#define SEARCH_H

#include <functional>

/// Generic binary search on a range [0, size).
/// The compare function returns: 0 (match), -1 (search right), or >0 (search left).
/// If returnNotFound is true, returns -1 if not found; otherwise returns the insertion point.
int binarySearch(int size, std::function<int(int)> compare, bool returnNotFound = true);

#endif
