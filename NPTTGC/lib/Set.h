#ifndef SET_H
#define SET_H

#include <string>
#include <stdexcept>
#include "Hash.h"

// Generic SetNode for storing values only (no keys)
template <typename T>
class SetNode
{
public:
    T value;
    SetNode<T> *next;

    // Constructor
    SetNode(const T &val)
        : value(val), next(nullptr)
    {
    }
};

// Set implementation using hash table
// Stores unique values using generic type hashing
template <typename T>
class Set
{
private:
    SetNode<T> **items;
    int size;
    int capacity;

    int getIndex(const T &value) const
    {
        int hashValue = ::hash(value);
        return ((hashValue % capacity) + capacity) % capacity;
    }

    // Helper function to resize hash table
    void resize()
    {
        int oldCapacity = capacity;
        SetNode<T> **oldItems = items;

        // Create new hash table with doubled capacity
        capacity = capacity * 2;
        items = new SetNode<T> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        // Rehash all existing entries into the new table
        for (int i = 0; i < oldCapacity; i++)
        {
            SetNode<T> *current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->value);
                current = current->next;
            }
        }

        // Delete old hash table
        for (int i = 0; i < oldCapacity; i++)
        {
            SetNode<T> *current = oldItems[i];
            while (current != nullptr)
            {
                SetNode<T> *temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] oldItems;
    }

public:
    // Constructor
    Set(int initialCapacity = 10)
        : capacity(initialCapacity), size(0)
    {
        items = new SetNode<T> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }
    }

    // Destructor
    ~Set()
    {
        for (int i = 0; i < capacity; i++)
        {
            SetNode<T> *current = items[i];
            while (current != nullptr)
            {
                SetNode<T> *temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] items;
    }

    // Copy constructor
    Set(const Set &other)
        : capacity(other.capacity), size(other.size)
    {
        items = new SetNode<T> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        for (int i = 0; i < other.capacity; i++)
        {
            SetNode<T> *otherCurrent = other.items[i];
            while (otherCurrent != nullptr)
            {
                insert(otherCurrent->value);
                otherCurrent = otherCurrent->next;
            }
        }
    }

    // Assignment operator
    Set &operator=(const Set &other)
    {
        if (this != &other)
        {
            for (int i = 0; i < capacity; i++)
            {
                SetNode<T> *current = items[i];
                while (current != nullptr)
                {
                    SetNode<T> *temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] items;

            capacity = other.capacity;
            size = 0;
            items = new SetNode<T> *[capacity];
            for (int i = 0; i < capacity; i++)
            {
                items[i] = nullptr;
            }

            for (int i = 0; i < other.capacity; i++)
            {
                SetNode<T> *otherCurrent = other.items[i];
                while (otherCurrent != nullptr)
                {
                    insert(otherCurrent->value);
                    otherCurrent = otherCurrent->next;
                }
            }
        }
        return *this;
    }

    // Insert value into set (returns true if inserted, false if already exists)
    bool insert(const T &value)
    {
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = getIndex(value);
        SetNode<T> *current = items[index];

        // Check if value already exists
        while (current != nullptr)
        {
            if (current->value == value)
            {
                return false; // Value already exists, don't insert duplicate
            }
            current = current->next;
        }

        // Value doesn't exist, insert at the beginning
        SetNode<T> *newNode = new SetNode<T>(value);
        newNode->next = items[index];
        items[index] = newNode;
        size++;
        return true;
    }

    // Remove value from set
    bool remove(const T &value)
    {
        int index = getIndex(value);
        SetNode<T> *current = items[index];
        SetNode<T> *prev = nullptr;

        while (current != nullptr)
        {
            if (current->value == value)
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

    // Check if value exists in set
    bool exists(const T &value) const
    {
        int index = getIndex(value);
        SetNode<T> *current = items[index];

        while (current != nullptr)
        {
            if (current->value == value)
            {
                return true;
            }
            current = current->next;
        }

        return false;
    }

    // Get size
    int getSize() const
    {
        return size;
    }

    // Get capacity
    int getCapacity() const
    {
        return capacity;
    }

    // Check if empty
    bool isEmpty() const
    {
        return size == 0;
    }

    // Clear all entries
    void clear()
    {
        for (int i = 0; i < capacity; i++)
        {
            SetNode<T> *current = items[i];
            while (current != nullptr)
            {
                SetNode<T> *temp = current;
                current = current->next;
                delete temp;
            }
            items[i] = nullptr;
        }
        size = 0;
    }
};

#endif
