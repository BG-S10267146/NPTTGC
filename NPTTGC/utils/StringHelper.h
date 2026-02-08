#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>
#include <iostream>
#include <cctype>

/// Helper functions for string input and manipulation
namespace StringHelper
{
    /// Trims leading and trailing whitespace from a string
    /// @param str The string to trim
    /// @return A new string with leading and trailing whitespace removed
    inline std::string trim(const std::string &str)
    {
        size_t start = 0;
        size_t end = str.length();

        // Find first non-whitespace character
        while (start < end && std::isspace(static_cast<unsigned char>(str[start])))
        {
            start++;
        }

        // Find last non-whitespace character
        while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
        {
            end--;
        }

        return str.substr(start, end - start);
    }

    /// Reads a line from stdin and returns it as a std::string
    /// Automatically trims leading and trailing whitespace
    /// @return The input string with whitespace removed from both ends
    inline std::string readLine()
    {
        std::string result;
        if (std::getline(std::cin, result))
        {
            return trim(result);
        }
        return "";
    }
}

#endif
