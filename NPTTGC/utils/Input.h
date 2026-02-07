#ifndef INPUT_H
#define INPUT_H

/// Read an integer from stdin with validation
bool readInteger(int& value);

/// Read an integer from a string with validation
bool readIntegerFromString(const char* str, int& value);

#endif
