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

    /// Inserts a value into the set. Returns true if inserted, false if already exists.
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

    /// Removes a value from the set. Returns true if removed, false if not found.
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

    /// Checks if a value exists in the set.
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
