#ifndef SORT_H
#define SORT_H

#include <functional>
#include "Vector.h"

namespace Sort
{
    // Median-of-three pivot selection for better performance
    template <typename T, typename Compare>
    void medianOfThree(Vector<T> &vec, int low, int high, Compare compare)
    {
        int mid = (high + low) / 2;

        // Order low, mid, high so that vec[low] <= vec[mid] <= vec[high]
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

        // Place median (now at mid) at high position for partitioning
        T temp = vec[mid];
        vec[mid] = vec[high];
        vec[high] = temp;
    }

    // Helper function for quicksort partitioning
    template <typename T, typename Compare>
    int partition(Vector<T> &vec, int low, int high, Compare compare)
    {
        // Use median-of-three pivot selection for better performance
        medianOfThree(vec, low, high, compare);

        T pivot = vec[high];
        int i = low - 1;

        for (int j = low; j < high; j++)
        {
            if (compare(vec[j], pivot) < 0)
            {
                i++;
                // Swap vec[i] and vec[j]
                T temp = vec[i];
                vec[i] = vec[j];
                vec[j] = temp;
            }
        }

        // Swap vec[i+1] and vec[high] (pivot)
        T temp = vec[i + 1];
        vec[i + 1] = vec[high];
        vec[high] = temp;

        return i + 1;
    }

    // Helper function for recursive quicksort
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

    // In-place quicksort with custom comparison function
    // Compare function should return: negative if a < b, 0 if a == b, positive if a > b
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
