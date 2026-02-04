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
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size - i - 1; j++)
            {
                if (compareSuffixes(suffixArray[j].i, suffixArray[j + 1].i) > 0)
                {
                    SuffixEntry temp = suffixArray[j];
                    suffixArray[j] = suffixArray[j + 1];
                    suffixArray[j + 1] = temp;
                }
            }
        }
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

        // Build combined string with \x1F separator
        text = "";
        for (int i = 0; i < items.getSize(); i++)
        {
            if (i > 0)
                text += '\x1F';  // Unit separator
            text += toLowerCase(items.get(i).name);
        }

        size = text.length();

        // Create suffix array entries
        suffixArray = new SuffixEntry[size];

        // Initialize suffix entries with i (position) and j (item index)
        for (int i = 0; i < size; i++)
        {
            suffixArray[i].i = i;

            // Calculate which item this position belongs to
            int itemIndex = 0;
            int currentPos = 0;
            for (int j = 0; j < items.getSize(); j++)
            {
                int itemLength = toLowerCase(items.get(j).name).length();
                if (i >= currentPos && i < currentPos + itemLength)
                {
                    itemIndex = j;
                    break;
                }
                currentPos += itemLength + 1;  // +1 for separator
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
        for (int i = 0; i < 100; i++)
        {
            found.append(false);
        }
        
        for (int i = lower; i < upper; i++)
        {
            int itemIndex = suffixArray[i].j;
            if (!found[itemIndex])
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
