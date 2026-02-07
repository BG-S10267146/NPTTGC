#ifndef DATE_H
#define DATE_H

#include <string>

/// Get current date and time as YYYY-MM-DD HH:MM
std::string getCurrentDateTime();

/// Compare two dates in YYYY-MM-DD format (-1 if date1 < date2, 0 if equal, 1 if date1 > date2)
int compareDates(const std::string& date1, const std::string& date2);

#endif
