#ifndef INPUT_H
#define INPUT_H

#include <string>

/// Helper functions for reading and validating numeric input
namespace InputHelper
{
    /// Reads an integer from stdin with validation
    /// @param value Reference to store the parsed integer value
    /// @return true if input was successfully read and parsed, false otherwise
    bool readInteger(int &value);

    /// Reads an integer from a string with validation
    /// @param str The string to parse
    /// @param value Reference to store the parsed integer value
    /// @return true if string was successfully parsed as integer, false otherwise
    bool readIntegerFromString(const std::string &str, int &value);
}

// For backward compatibility - global namespace wrappers
using InputHelper::readInteger;
using InputHelper::readIntegerFromString;

#endif
