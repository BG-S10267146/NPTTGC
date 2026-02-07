#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include <string>
#include <stdexcept>
#include <cctype>
#include <functional>
#include "Vector.h"
#include "Set.h"
#include "Search.h"
#include "Sort.h"

// Structure to store suffix information
struct SuffixEntry
{
    int i; // Position in combined string
    int j; // Item index (which game this suffix belongs to)

    SuffixEntry() : i(0), j(0) {}
    SuffixEntry(int i, int j) : i(i), j(j) {}
};

class SuffixArray
{
private:
    std::string text;             // Lowercase concatenated strings
    Vector<SuffixEntry> suffixes; // Array of suffix entries
    size_t size;                  // Number of suffixes

    // Convert string to lowercase
    std::string toLowerCase(const std::string &str) const
    {
        std::string result = "";
        for (size_t i = 0; i < str.length(); i++)
        {
            result += std::tolower((unsigned char)str[i]);
        }
        return result;
    }

public:
    //  Constructor
    SuffixArray() : text(""), size(0)
    {
    }

    // Build suffix array from a vector of strings
    static SuffixArray build(int itemCount, std::function<std::string(int)> getItem)
    {
        SuffixArray sa;

        for (int i = 0, k = 0; i < itemCount; i++)
        {
            if (i > 0)
            {
                sa.text += '\x1F'; // Unit separator
                sa.suffixes.append(SuffixEntry(k, i - 1));
                k++;
            }

            sa.text += sa.toLowerCase(getItem(i));

            for (int j = 0; j < getItem(i).length(); j++, k++)
            {
                sa.suffixes.append(SuffixEntry(k, i));
            }
        }

        sa.size = sa.text.length();

        // Sort suffixes using the Sort library
        std::function<int(const SuffixEntry &, const SuffixEntry &)> compareFunc =
            [&sa](const SuffixEntry &a, const SuffixEntry &b) -> int
        {
            // Compare two suffixes lexicographically
            int pos1 = a.i;
            int pos2 = b.i;

            if (pos1 == pos2)
                return 0;

            while (pos1 < (int)sa.text.length() && pos2 < (int)sa.text.length())
            {
                if (sa.text[pos1] != sa.text[pos2])
                {
                    return sa.text[pos1] - sa.text[pos2];
                }
                pos1++;
                pos2++;
            }

            return (pos1 == (int)sa.text.length()) ? -1 : 1;
        };

        Sort::quicksort<SuffixEntry>(sa.suffixes, compareFunc);

        return sa;
    }

    // Search for query string and return matching item indices
    Vector<int> search(const std::string &query)
    {
        Vector<int> results;

        if (query.empty() || size == 0)
        {
            return results;
        }

        std::string lowerQuery = toLowerCase(query);

        // Find lower bound
        int lower = binarySearch(size, [&](int mid)
                                 {
            std::string suffix = text.substr(suffixes[mid].i);
            return suffix < lowerQuery ? -1 : 1; }, false);

        // Find upper bound
        int upper = binarySearch(size, [&](int mid)
                                 {
            std::string suffix = text.substr(suffixes[mid].i);
            bool suffixStartsWithQuery = suffix.length() >= lowerQuery.length() &&
                                        suffix.substr(0, lowerQuery.length()) == lowerQuery;
            return suffixStartsWithQuery || suffix < lowerQuery  ? -1 : 1; }, false);

        // Collect matching item indices into results without duplicates
        Set<int> found;
        for (int i = lower; i < upper; i++)
        {
            int itemIndex = suffixes[i].j;
            if (!found.exists(itemIndex))
            {
                results.append(itemIndex);
                found.insert(itemIndex);
            }
        }

        return results;
    }

    // Get size
    int getSize() const
    {
        return size;
    }
};

#endif
