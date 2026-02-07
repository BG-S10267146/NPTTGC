#ifndef HASH_H
#define HASH_H

#include <string>

/// Computes a hash value for generic types by hashing their memory representation.
template <typename T>
int hash(const T &value)
{
    const unsigned char *bytes = reinterpret_cast<const unsigned char *>(&value);
    int hashValue = 0;

    for (size_t i = 0; i < sizeof(T); i++)
    {
        hashValue = hashValue * 31 + bytes[i];
    }
    return hashValue;
}

/// Computes a polynomial rolling hash for std::string.
template <>
inline int hash<std::string>(const std::string &value)
{
    int hashValue = 0;
    for (size_t i = 0; i < value.length(); i++)
    {
        hashValue = hashValue * 31 + value[i];
    }
    return hashValue;
}

#endif
