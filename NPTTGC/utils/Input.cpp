#include "Input.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>

bool readInteger(int& value)
{
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) == nullptr)
        return false;

    char* endPtr;
    value = strtol(buffer, &endPtr, 10);

    while (*endPtr && isspace(*endPtr))
        endPtr++;

    return (*endPtr == '\0');
}

bool readIntegerFromString(const char* str, int& value)
{
    if (str == nullptr || str[0] == '\0')
        return false;

    char* endPtr;
    value = strtol(str, &endPtr, 10);

    while (*endPtr && isspace(*endPtr))
        endPtr++;

    return (*endPtr == '\0');
}
