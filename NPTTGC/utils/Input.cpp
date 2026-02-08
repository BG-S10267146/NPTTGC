#include "Input.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <climits>
#include <string>

namespace InputHelper
{
    bool readInteger(int &value)
    {
        char buffer[100];
        if (fgets(buffer, sizeof(buffer), stdin) == nullptr)
        {
            return false;
        }

        // Reset errno before strtol to detect overflow
        errno = 0;
        char *endPtr;
        long result = strtol(buffer, &endPtr, 10);

        // Check for conversion errors
        if (errno == ERANGE || result > INT_MAX || result < INT_MIN)
        {
            return false; // Overflow/underflow
        }

        // Skip trailing whitespace
        while (*endPtr && isspace(static_cast<unsigned char>(*endPtr)))
        {
            endPtr++;
        }

        // Check if entire string was consumed (valid integer)
        if (*endPtr != '\0')
        {
            return false;
        }

        value = static_cast<int>(result);
        return true;
    }

    bool readIntegerFromString(const std::string &str, int &value)
    {
        if (str.empty())
        {
            return false;
        }

        // Reset errno before strtol to detect overflow
        errno = 0;
        char *endPtr;
        long result = strtol(str.c_str(), &endPtr, 10);

        // Check for conversion errors
        if (errno == ERANGE || result > INT_MAX || result < INT_MIN)
        {
            return false; // Overflow/underflow
        }

        // Skip trailing whitespace
        while (*endPtr && isspace(static_cast<unsigned char>(*endPtr)))
        {
            endPtr++;
        }

        // Check if entire string was consumed (valid integer)
        if (*endPtr != '\0')
        {
            return false;
        }

        value = static_cast<int>(result);
        return true;
    }
}
