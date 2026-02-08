#ifndef DATE_H
#define DATE_H

#include <string>

/// Helper functions for date and time operations
namespace DateHelper
{
    /// Gets the current date and time as a formatted string
    /// @return Current date/time in "YYYY-MM-DD HH:MM" format
    std::string getCurrentDateTime();

    /// Compares two dates in YYYY-MM-DD format (lexicographic comparison)
    /// @param date1 First date string to compare
    /// @param date2 Second date string to compare
    /// @return -1 if date1 < date2, 0 if equal, 1 if date1 > date2
    /// @note Assumes dates are in valid YYYY-MM-DD format for correct comparison
    int compareDates(const std::string &date1, const std::string &date2);
}

// For backward compatibility - global namespace wrappers
using DateHelper::compareDates;
using DateHelper::getCurrentDateTime;

#endif
