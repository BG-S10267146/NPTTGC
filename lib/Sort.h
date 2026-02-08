#ifndef SORT_H
#define SORT_H

#include <functional>
#include "Vector.h"

namespace Sort
{
    /**
     * Median-of-three pivot selection helper for improved quicksort performance
     * Rearranges elements to place the median of first, middle, and last elements at the end
     * @param vec The vector to operate on
     * @param low The starting index of the range
     * @param high The ending index of the range
     * @param compare Comparison function returning negative if a < b, 0 if a == b, positive if a > b
     */
    template <typename T, typename Compare>
    void medianOfThree(Vector<T> &vec, int low, int high, Compare compare)
    {
        int mid = (high + low) / 2;

        if (compare(vec[mid], vec[low]) < 0)
        {
            T temp = vec[low];
            vec[low] = vec[mid];
            vec[mid] = temp;
        }
        if (compare(vec[high], vec[low]) < 0)
        {
            T temp = vec[low];
            vec[low] = vec[high];
            vec[high] = temp;
        }
        if (compare(vec[high], vec[mid]) < 0)
        {
            T temp = vec[mid];
            vec[mid] = vec[high];
            vec[high] = temp;
        }

        T temp = vec[mid];
        vec[mid] = vec[high];
        vec[high] = temp;
    }

    /**
     * Partitions the vector around a pivot using median-of-three selection
     * Elements less than pivot are moved to the left, greater to the right
     * @param vec The vector to partition
     * @param low The starting index of the range to partition
     * @param high The ending index of the range to partition
     * @param compare Comparison function returning negative if a < b, 0 if a == b, positive if a > b
     * @return The final position of the pivot element
     */
    template <typename T, typename Compare>
    int partition(Vector<T> &vec, int low, int high, Compare compare)
    {
        medianOfThree(vec, low, high, compare);

        T pivot = vec[high];
        int i = low - 1;

        for (int j = low; j < high; j++)
        {
            if (compare(vec[j], pivot) < 0)
            {
                i++;
                T temp = vec[i];
                vec[i] = vec[j];
                vec[j] = temp;
            }
        }

        T temp = vec[i + 1];
        vec[i + 1] = vec[high];
        vec[high] = temp;

        return i + 1;
    }

    /**
     * Recursive helper function for quicksort implementation
     * @param vec The vector to sort
     * @param low The starting index of the range to sort
     * @param high The ending index of the range to sort
     * @param compare Comparison function returning negative if a < b, 0 if a == b, positive if a > b
     */
    template <typename T, typename Compare>
    void quicksortHelper(Vector<T> &vec, int low, int high, Compare compare)
    {
        if (low < high)
        {
            int pivotIndex = partition(vec, low, high, compare);
            quicksortHelper(vec, low, pivotIndex - 1, compare);
            quicksortHelper(vec, pivotIndex + 1, high, compare);
        }
    }

    /**
     * In-place quicksort with median-of-three pivot selection
     * Sorts the entire vector using the provided comparison function
     * @param vec The vector to sort in-place
     * @param compare Comparison function returning negative if a < b, 0 if a == b, positive if a > b
     */
    template <typename T, typename Compare>
    void quicksort(Vector<T> &vec, Compare compare)
    {
        if (vec.getSize() > 1)
        {
            quicksortHelper(vec, 0, vec.getSize() - 1, compare);
        }
    }
}

#endif
