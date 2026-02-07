#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <stdexcept>
#include <functional>
#include "Hash.h"
#include "Vector.h"

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
        : capacity(initialCapacity), size(0)
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
        : capacity(other.capacity), size(other.size)
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

    /// Inserts or updates a key-value pair. If the key exists, its value is updated.
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
    }

    /// Removes a key-value pair by key. Returns true if removed, false if not found.
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

    /// Gets the value associated with a key. Throws runtime_error if key not found.
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

    /// Checks if a key exists in the dictionary.
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

    int getSize() const
    {
        return size;
    }

    int getCapacity() const
    {
        return capacity;
    }

    bool isEmpty() const
    {
        return size == 0;
    }

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
    }

    /// Converts all values in the dictionary to a Vector.
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

    /// Converts all values in the dictionary to a Vector, applying a transform function.
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
};

#endif
