
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <stdexcept>
#include <functional>
#include "Hash.h"
#include "Vector.h"

/**
 * Goh Jun Liang Bryan, Group 2, S10267146, P01
 * Haziq Danish Bin Hairil Rizal, Group2, S10267828, P01
 */
template <typename K, typename V>
class DictionaryNode
{
public:
    K key;
    V item;
    DictionaryNode<K, V> *next;

    DictionaryNode(const K &k, const V &value)
        : key(k), item(value), next(nullptr)
    {
    }
};

/// Hash table implementation of a dictionary (key-value map) with separate chaining.
template <typename K, typename V>
class Dictionary
{
private:
    DictionaryNode<K, V> **items; // Array of pointers to DictionaryNode
    int size;
    int capacity;
    K maxKey_;

    int getIndex(const K &key) const
    {
        int hashValue = hash(key);
        return hashValue % capacity;
    }

    void resize()
    {
        int oldCapacity = capacity;
        DictionaryNode<K, V> **oldItems = items;

        // Create new hash table with doubled capacity
        capacity = capacity * 2;
        items = new DictionaryNode<K, V> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        // Rehash all entries from old table into new table
        // Keys may map to different buckets due to new capacity
        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<K, V> *current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->key, current->item);
                current = current->next;
            }
        }

        // Delete all nodes and the old table array
        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<K, V> *current = oldItems[i];
            while (current != nullptr)
            {
                DictionaryNode<K, V> *temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] oldItems;
    }

public:
    Dictionary(int initialCapacity = 10)
        : capacity(initialCapacity), size(0), maxKey_(K())
    {
        items = new DictionaryNode<K, V> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }
    }

    ~Dictionary()
    {
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<K, V> *current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<K, V> *temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] items;
    }

    Dictionary(const Dictionary &other)
        : capacity(other.capacity), size(other.size), maxKey_(other.maxKey_)
    {
        items = new DictionaryNode<K, V> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        // Deep copy all entries from other dictionary
        for (int i = 0; i < other.capacity; i++)
        {
            DictionaryNode<K, V> *otherCurrent = other.items[i];
            while (otherCurrent != nullptr)
            {
                insert(otherCurrent->key, otherCurrent->item);
                otherCurrent = otherCurrent->next;
            }
        }
    }

    Dictionary &operator=(const Dictionary &other)
    {
        if (this != &other)
        {
            // Clean up existing data
            for (int i = 0; i < capacity; i++)
            {
                DictionaryNode<K, V> *current = items[i];
                while (current != nullptr)
                {
                    DictionaryNode<K, V> *temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] items;

            // Allocate new hash table matching other's capacity
            capacity = other.capacity;
            size = 0;
            maxKey_ = other.maxKey_;
            items = new DictionaryNode<K, V> *[capacity];
            for (int i = 0; i < capacity; i++)
            {
                items[i] = nullptr;
            }

            // Deep copy all entries from other dictionary
            for (int i = 0; i < other.capacity; i++)
            {
                DictionaryNode<K, V> *otherCurrent = other.items[i];
                while (otherCurrent != nullptr)
                {
                    insert(otherCurrent->key, otherCurrent->item);
                    otherCurrent = otherCurrent->next;
                }
            }
        }
        return *this;
    }

    /*
     * Stores a key-value pair in the dictionary. If the key already exists, the value is updated.
     * Parameters: key - the key to store, item - the value to store
     * Returns: nothing
     */
    void insert(const K &key, const V &item)
    {
        // Maintain load factor below 0.5 for good performance
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = getIndex(key);
        DictionaryNode<K, V> *current = items[index];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->item = item;
                return;
            }
            current = current->next;
        }

        DictionaryNode<K, V> *newNode = new DictionaryNode<K, V>(key, item);
        newNode->next = items[index];
        items[index] = newNode;
        size++;

        // Update maxKey if this is the first key or a larger key
        if (size == 1 || key > maxKey_)
        {
            maxKey_ = key;
        }
    }

    /*
     * Removes a key-value pair from the dictionary by its key.
     * Parameters: key - the key to remove
     * Returns: true if the key was found and removed, false if the key was not found
     */
    bool remove(const K &key)
    {
        int index = getIndex(key);
        DictionaryNode<K, V> *current = items[index];
        DictionaryNode<K, V> *prev = nullptr;

        while (current != nullptr)
        {
            if (current->key == key)
            {
                if (prev == nullptr)
                {
                    items[index] = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                delete current;
                size--;
                return true;
            }
            prev = current;
            current = current->next;
        }

        return false;
    }

    /*
     * Gets the value associated with a key. Throws runtime_error if key not found.
     */
    V get(const K &key) const
    {
        int index = getIndex(key);
        DictionaryNode<K, V> *current = items[index];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                return current->item;
            }
            current = current->next;
        }

        throw std::runtime_error("Key not found in dictionary");
    }

    /*
     * Checks whether a key is stored in the dictionary.
     * Parameters: key - the key to check for
     * Returns: true if the key exists, false otherwise
     */
    bool exists(const K &key) const
    {
        int index = getIndex(key);
        DictionaryNode<K, V> *current = items[index];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                return true;
            }
            current = current->next;
        }

        return false;
    }

    /*
     * Returns the number of key-value pairs stored in the dictionary.
     * Parameters: none
     * Returns: the count of entries in the dictionary
     */
    int getSize() const
    {
        return size;
    }

    /*
     * Returns the current capacity of the internal hash table.
     * Parameters: none
     * Returns: the maximum number of entries before resizing occurs
     */
    int getCapacity() const
    {
        return capacity;
    }

    /*
     * Checks whether the dictionary contains any entries.
     * Parameters: none
     * Returns: true if the dictionary is empty, false otherwise
     */
    bool isEmpty() const
    {
        return size == 0;
    }

    /*
     * Removes all entries from the dictionary, resetting it to empty state.
     * Parameters: none
     * Returns: nothing
     */
    void clear()
    {
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<K, V> *current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<K, V> *temp = current;
                current = current->next;
                delete temp;
            }
            items[i] = nullptr;
        }
        size = 0;
        maxKey_ = K();
    }

    /*
     * Extracts all values from the dictionary and returns them as a Vector.
     * Parameters: none
     * Returns: a Vector containing all values in the dictionary
     */
    Vector<V> toVector() const
    {
        Vector<V> result;
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<K, V> *current = items[i];
            while (current != nullptr)
            {
                result.append(current->item);
                current = current->next;
            }
        }
        return result;
    }

    /*
     * Extracts all values from the dictionary and applies a transformation function, returning results as a Vector.
     * Parameters: transform - a function that converts each value from type V to type U
     * Returns: a Vector containing the transformed values
     */
    template <typename U>
    Vector<U> toVector(std::function<U(const V &)> transform) const
    {
        Vector<U> result;
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<K, V> *current = items[i];
            while (current != nullptr)
            {
                result.append(transform(current->item));
                current = current->next;
            }
        }
        return result;
    }

    /*
     * Calls a function for each key-value pair in the dictionary.
     * Parameters: func - a function that takes a key and value and performs an action
     * Returns: nothing
     * Note: If no modifications are made to the dictionary, iteration order remains consistent
     */
    void forEach(std::function<void(const K &, const V &)> func) const
    {
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<K, V> *current = items[i];
            while (current != nullptr)
            {
                func(current->key, current->item);
                current = current->next;
            }
        }
    }

    /*
     * Returns the largest key currently stored in the dictionary.
     * Parameters: none
     * Returns: the maximum key value, or default-constructed K if dictionary is empty
     */
    K maxKey() const
    {
        return maxKey_;
    }
};

#endif
