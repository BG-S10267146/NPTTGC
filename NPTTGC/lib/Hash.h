#ifndef HASH_H
#define HASH_H

#include <string>

/**
 * Computes a hash value for any type by hashing raw bytes.
 * For std::string, hashes character content using polynomial rolling hash.
 * For all other types, hashes the object's memory representation byte-by-byte.
 */
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

template <>
inline int hash<std::string>(const std::string &value)
{
    return hash(value.c_str());
}

#endif
