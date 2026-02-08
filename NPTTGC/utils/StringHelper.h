#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <cstdio>
#include <cstring>
#include <climits>

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

    /// Reads a line from stdin, trims newline, and checks if it's empty
    /// @param buffer The buffer to store the input
    /// @param bufferSize The size of the buffer
    /// @return true if input is non-empty, false if empty
    inline bool readNonEmptyLine(char *buffer, size_t bufferSize)
    {
        if (!readLine(buffer, bufferSize))
        {
            return false;
        }
        return strlen(buffer) > 0;
    }

    /// Compares two strings case-insensitively
    /// @param str1 First string
    /// @param str2 Second string
    /// @return true if strings are equal (case-insensitive), false otherwise
    inline bool equalsIgnoreCase(const char *str1, const char *str2)
    {
        if (!str1 || !str2)
            return false;

#ifdef _WIN32
        return _stricmp(str1, str2) == 0;
#else
        return strcasecmp(str1, str2) == 0;
#endif
    }
}

#endif
