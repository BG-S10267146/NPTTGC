/**
 * Goh Jun Liang Bryan, Group 2, S10267146, P01
 * Haziq Danish Bin Hairil Rizal, Group2, S10267828, P01
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>

/// Dynamic array implementation with automatic resizing.
template <typename T>
class Vector
{
private:
    T *items;
    int size;
    int capacity;

    void resize()
    {
        int newCapacity = capacity * 2;
        T *newItems = new T[newCapacity];

        for (int i = 0; i < size; i++)
        {
            newItems[i] = items[i];
        }

        delete[] items;
        items = newItems;
        capacity = newCapacity;
    }

public:
    Vector(int initialCapacity = 10)
        : capacity(initialCapacity), size(0)
    {
        items = new T[capacity];
    }

    ~Vector()
    {
        delete[] items;
    }

    Vector(const Vector &other)
        : size(other.size), capacity(other.capacity)
    {
        items = new T[capacity];
        for (int i = 0; i < size; i++)
        {
            items[i] = other.items[i];
        }
    }

    Vector &operator=(const Vector &other)
    {
        if (this != &other)
        {
            delete[] items;
            size = other.size;
            capacity = other.capacity;
            items = new T[capacity];
            for (int i = 0; i < size; i++)
            {
                items[i] = other.items[i];
            }
        }
        return *this;
    }

    /// Retrieves the element at a specific position in the vector.
    /// Parameters: index - the position to access (0-based)
    /// Returns: a copy of the element at that position, or throws out_of_range if invalid
    T get(int index) const
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

    /// Accesses an element by position using bracket notation, allowing modification.
    /// Parameters: index - the position to access (0-based)
    /// Returns: a reference to the element that can be modified, or throws out_of_range if invalid
    T &operator[](int index)
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

    /// Accesses an element by position using bracket notation (read-only).
    /// Parameters: index - the position to access (0-based)
    /// Returns: a const reference to the element, or throws out_of_range if invalid
    const T &operator[](int index) const
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

    /// Removes the element at a specific position, shifting all following elements left.
    /// Parameters: index - the position of the element to remove (0-based)
    /// Returns: nothing, or throws out_of_range if invalid
    void remove(int index)
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }

        for (int i = index; i < size - 1; i++)
        {
            items[i] = items[i + 1];
        }
        size--;
    }

    /// Adds an element to the end of the vector, expanding capacity if needed.
    /// Parameters: value - the element to add
    /// Returns: nothing
    void append(T value)
    {
        if (size >= capacity)
        {
            resize();
        }
        items[size] = value;
        size++;
    }

    /// Inserts an element at a specific position, shifting all following elements right.
    /// Parameters: index - the position to insert at (0-based), value - the element to insert
    /// Returns: nothing, or throws out_of_range if index is invalid
    void insert(int index, T value)
    {
        if (index < 0 || index > size)
        {
            throw std::out_of_range("Vector index out of range");
        }

        if (size >= capacity)
        {
            resize();
        }

        for (int i = size; i > index; i--)
        {
            items[i] = items[i - 1];
        }
        items[index] = value;
        size++;
    }

    /// Returns the number of elements currently in the vector.
    /// Parameters: none
    /// Returns: the count of elements
    int getSize() const
    {
        return size;
    }

    /// Returns the current capacity of the internal array.
    /// Parameters: none
    /// Returns: the maximum number of elements before resizing occurs
    int getCapacity() const
    {
        return capacity;
    }

    /// Checks whether the vector contains any elements.
    /// Parameters: none
    /// Returns: true if the vector is empty, false otherwise
    bool isEmpty() const
    {
        return size == 0;
    }

    /// Removes all elements from the vector, leaving it empty.
    /// Parameters: none
    /// Returns: nothing
    void clear()
    {
        size = 0;
    }
};

#endif
