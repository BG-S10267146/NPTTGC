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

/**
 * Goh Jun Liang Bryan, Group 2, S10267146, P01
 * Haziq Danish Bin Hairil Rizal, Group2, S10267828, P01
 */
struct SuffixEntry
{
    // Position in combined string
    int i;
    // Item index (which item this suffix belongs to)
    int j;

    SuffixEntry() : i(0), j(0) {}
    SuffixEntry(int i, int j) : i(i), j(j) {}
};

/// Suffix array data structure for efficient substring searching across multiple strings.
class SuffixArray
{
private:
    std::string text;
    Vector<SuffixEntry> suffixes;
    size_t size;

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
    SuffixArray() : text(""), size(0)
    {
    }

    /// Builds a suffix array from multiple strings for efficient substring searching.
    template <typename T>
    static SuffixArray build(
        const Vector<T> &data,
        std::function<std::string(const T &)> getContent,
        std::function<int(const T &)> getId)
    {
        SuffixArray sa;

        for (int i = 0, k = 0; i < data.getSize(); i++)
        {
            if (i > 0)
            {
                sa.text += '\x1F';
                sa.suffixes.append(SuffixEntry(k, getId(data[i - 1])));
                k++;
            }

            const T &item = data[i];
            std::string content = getContent(item);
            sa.text += sa.toLowerCase(content);

            for (int j = 0; j < content.length(); j++, k++)
            {
                sa.suffixes.append(SuffixEntry(k, getId(item)));
            }
        }

        sa.size = sa.text.length();

        auto compareFunc =
            [&](const SuffixEntry &a, const SuffixEntry &b)
        {
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

        sa.size = sa.suffixes.getSize();
        Sort::quicksort<SuffixEntry>(sa.suffixes, compareFunc);

        return sa;
    }

    /// Searches for a query string and returns indices of all items containing it as a substring.
    Vector<int> search(const std::string &query)
    {
        Vector<int> results;

        if (query.empty() || size == 0)
        {
            return results;
        }

        std::string lowerQuery = toLowerCase(query);

        int lower = binarySearch(size, [&](int mid)
                                 {
            std::string suffix = text.substr(suffixes[mid].i);
            return suffix < lowerQuery ? -1 : 1; }, false);

        int upper = binarySearch(size, [&](int mid)
                                 {
            std::string suffix = text.substr(suffixes[mid].i);
            bool suffixStartsWithQuery = suffix.length() >= lowerQuery.length() &&
                                        suffix.substr(0, lowerQuery.length()) == lowerQuery;
            return suffixStartsWithQuery || suffix < lowerQuery  ? -1 : 1; }, false);

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
};

#endif
