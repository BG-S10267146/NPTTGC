/**
 * Goh Jun Liang Bryan, Group 2, S10267146, P01
 * Haziq Danish Bin Hairil Rizal, Group2, S10267828, P01
 */

#ifndef SET_H
#define SET_H

#include <string>
#include <stdexcept>
#include "Hash.h"

template <typename T>
class SetNode
{
public:
    T value;
    SetNode<T> *next;

    SetNode(const T &val)
        : value(val), next(nullptr)
    {
    }
};

/// Hash table implementation of a set storing unique values with separate chaining.
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

    void resize()
    {
        int oldCapacity = capacity;
        SetNode<T> **oldItems = items;

        capacity = capacity * 2;
        items = new SetNode<T> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        for (int i = 0; i < oldCapacity; i++)
        {
            SetNode<T> *current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->value);
                current = current->next;
            }
        }

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
    Set(int initialCapacity = 10)
        : capacity(initialCapacity), size(0)
    {
        items = new SetNode<T> *[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }
    }

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

    /*
     * Adds a value to the set if it doesn't already exist.
     * Parameters: value - the value to add
     * Returns: true if the value was added, false if it was already in the set
     */
    bool insert(const T &value)
    {
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = getIndex(value);
        SetNode<T> *current = items[index];

        while (current != nullptr)
        {
            if (current->value == value)
            {
                return false;
            }
            current = current->next;
        }

        SetNode<T> *newNode = new SetNode<T>(value);
        newNode->next = items[index];
        items[index] = newNode;
        size++;
        return true;
    }

    /*
     * Removes a value from the set if it exists.
     * Parameters: value - the value to remove
     * Returns: true if the value was removed, false if it was not in the set
     */
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

    /*
     * Checks whether a value is stored in the set.
     * Parameters: value - the value to check for
     * Returns: true if the value exists in the set, false otherwise
     */
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

    /*
     * Returns the number of values currently in the set.
     * Parameters: none
     * Returns: the count of unique values
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
     * Checks whether the set contains any values.
     * Parameters: none
     * Returns: true if the set is empty, false otherwise
     */
    bool isEmpty() const
    {
        return size == 0;
    }

    /*
     * Removes all values from the set, leaving it empty.
     * Parameters: none
     * Returns: nothing
     */
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
