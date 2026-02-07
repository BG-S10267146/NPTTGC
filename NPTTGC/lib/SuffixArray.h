#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include <string>
#include <stdexcept>
#include <cctype>
#include "Vector.h"

// Structure to store suffix information
struct SuffixEntry
{
    int i;  // Position in combined string
    int j;  // Item index (which game this suffix belongs to)
};

class SuffixArray
{
private:
    std::string text;              // Lowercase concatenated strings
    SuffixEntry* suffixArray;      // Array of suffix entries
    int size;                      // Number of suffixes

    // Convert character to lowercase
    char toLower(char c) const
    {
        return std::tolower((unsigned char)c);
    }

    // Convert string to lowercase
    std::string toLowerCase(const std::string& str) const
    {
        std::string result = "";
        for (size_t i = 0; i < str.length(); i++)
        {
            result += toLower(str[i]);
        }
        return result;
    }

    // Compare two suffixes lexicographically
    int compareSuffixes(int idx1, int idx2) const
    {
        int pos1 = idx1;
        int pos2 = idx2;

        while (pos1 < (int)text.length() && pos2 < (int)text.length())
        {
            if (text[pos1] != text[pos2])
            {
                return text[pos1] - text[pos2];
            }
            pos1++;
            pos2++;
        }

        // If one suffix is a prefix of the other
        if (pos1 == (int)text.length() && pos2 == (int)text.length())
            return 0;
        if (pos1 == (int)text.length())
            return -1;
        return 1;
    }

    // Bubble sort for suffix array
    void sortSuffixes()
    {
        quickSort(0, size - 1);
    }

    // QuickSort for suffix array (much faster than bubble sort)
    void quickSort(int left, int right)
    {
        if (left >= right)
            return;

        // Partition
        int pivot = left + (right - left) / 2;
        int i = left, j = right;
        SuffixEntry pivotEntry = suffixArray[pivot];

        while (i <= j)
        {
            while (compareSuffixes(suffixArray[i].i, pivotEntry.i) < 0)
                i++;
            while (compareSuffixes(suffixArray[j].i, pivotEntry.i) > 0)
                j--;

            if (i <= j)
            {
                SuffixEntry temp = suffixArray[i];
                suffixArray[i] = suffixArray[j];
                suffixArray[j] = temp;
                i++;
                j--;
            }
        }

        // Recursively sort
        if (left < j)
            quickSort(left, j);
        if (i < right)
            quickSort(i, right);
    }

    // Binary search for lower bound (first suffix >= query)
    int lowerBound(const std::string& query) const
    {
        int left = 0, right = size;

        while (left < right)
        {
            int mid = left + (right - left) / 2;
            std::string suffix = text.substr(suffixArray[mid].i);

            if (suffix < query)
            {
                left = mid + 1;
            }
            else
            {
                right = mid;
            }
        }

        return left;
    }

    // Binary search for upper bound (first suffix > query)
    int upperBound(const std::string& query) const
    {
        int left = 0, right = size;

        while (left < right)
        {
            int mid = left + (right - left) / 2;
            std::string suffix = text.substr(suffixArray[mid].i);

            // Check if suffix starts with query
            bool startsWith = false;
            if (suffix.length() >= query.length())
            {
                startsWith = (suffix.substr(0, query.length()) == query);
            }

            if (startsWith || suffix < query)
            {
                left = mid + 1;
            }
            else
            {
                right = mid;
            }
        }

        return left;
    }

public:
    // Constructor
    SuffixArray() : text(""), suffixArray(nullptr), size(0)
    {
    }

    // Destructor
    ~SuffixArray()
    {
        if (suffixArray != nullptr)
        {
            delete[] suffixArray;
        }
    }

    // Build suffix array from a vector of strings
    template <typename T>
    void build(const Vector<T>& items)
    {
        // Clean up old suffix array
        if (suffixArray != nullptr)
        {
            delete[] suffixArray;
        }

        // Build combined string with \x1F separator and track positions
        text = "";
        Vector<int> itemStartPositions;
        
        for (int i = 0; i < items.getSize(); i++)
        {
            itemStartPositions.append(text.length());
            if (i > 0)
                text += '\x1F';  // Unit separator
            text += toLowerCase(items.get(i).name);
        }

        size = text.length();

        // Create suffix array entries
        suffixArray = new SuffixEntry[size];

        // Initialize suffix entries with i (position) and j (item index)
        // Use pre-calculated positions to avoid nested loop
        for (int i = 0; i < size; i++)
        {
            suffixArray[i].i = i;

            // Binary search to find which item this position belongs to
            int itemIndex = 0;
            for (int j = items.getSize() - 1; j >= 0; j--)
            {
                if (i >= itemStartPositions.get(j))
                {
                    itemIndex = j;
                    break;
                }
            }
            suffixArray[i].j = itemIndex;
        }

        // Sort suffixes
        sortSuffixes();
    }

    // Rebuild suffix array from items
    template <typename T>
    void rebuild(const Vector<T>& items)
    {
        build(items);
    }

    // Search for query string and return matching item indices
    Vector<int> search(const std::string& query)
    {
        Vector<int> results;

        if (query.empty() || size == 0)
        {
            return results;
        }

        std::string lowerQuery = toLowerCase(query);

        // Find lower bound
        int lower = 0;
        int left = 0, right = size;
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            std::string suffix = text.substr(suffixArray[mid].i);
            if (suffix < lowerQuery)
            {
                left = mid + 1;
            }
            else
            {
                right = mid;
            }
        }
        lower = left;

        // Find upper bound
        int upper = size;
        left = 0;
        right = size;
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            std::string suffix = text.substr(suffixArray[mid].i);

            // Check if suffix starts with query
            bool startsWith = false;
            if (suffix.length() >= lowerQuery.length())
            {
                startsWith = (suffix.substr(0, lowerQuery.length()) == lowerQuery);
            }

            if (startsWith || suffix < lowerQuery)
            {
                left = mid + 1;
            }
            else
            {
                right = mid;
            }
        }
        upper = left;

        // Collect matching item indices, removing duplicates
        Vector<bool> found;
        // Dynamically size the found vector - accommodate all 629+ games
        for (int i = 0; i < 1000; i++)
        {
            found.append(false);
        }
        
        for (int i = lower; i < upper; i++)
        {
            int itemIndex = suffixArray[i].j;
            if (itemIndex < 1000 && !found[itemIndex])
            {
                results.append(itemIndex);
                found[itemIndex] = true;
            }
        }

        return results;
    }

    // Get size
    int getSize() const
    {
        return size;
    }

    // Clear
    void clear()
    {
        text = "";
        if (suffixArray != nullptr)
        {
            delete[] suffixArray;
            suffixArray = nullptr;
        }
        size = 0;
    }
};

#endif
