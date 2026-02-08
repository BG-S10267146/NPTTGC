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
    /// Reads a line from stdin and trims the trailing newline
    /// @param buffer The buffer to store the input
    /// @param bufferSize The size of the buffer
    /// @return true if input was read successfully, false otherwise
    inline bool readLine(char *buffer, size_t bufferSize)
    {
        if (!buffer || bufferSize == 0)
        {
            return false;
        }
        // fgets expects int, clamp to safe range
        int size = (bufferSize > INT_MAX) ? INT_MAX : static_cast<int>(bufferSize);
        if (!fgets(buffer, size, stdin))
        {
            return false;
        }
        buffer[strcspn(buffer, "\n")] = 0;
        return true;
    }

    /// Reads a line from stdin and returns it as a std::string
    /// Trims the trailing newline character
    /// @return The input string with newline removed
    inline std::string readLineToString()
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
