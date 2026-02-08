#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <cstdio>
#include <cstring>
#include <climits>
#include <string>
#include <iostream>

/// Helper functions for string input and manipulation
namespace StringHelper
{
    /// Reads a line from stdin and returns it as a std::string
    /// Trims the trailing newline character
    /// @return The input string with newline removed
    inline std::string readLine()
    {
        std::string result;
        if (std::getline(std::cin, result))
        {
            return result;
        }
        return "";
    }
}

#endif
