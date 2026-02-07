#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>

template <typename T>
class Vector
{
private:
    T *items;
    int size;
    int capacity;

    // Helper function to resize the vector
    void resize()
    {
        int newCapacity = capacity * 2;
        T *newItems = new T[newCapacity];

        // Copy existing elements to new array
        for (int i = 0; i < size; i++)
        {
            newItems[i] = items[i];
        }

        // Delete old array and update pointers
        delete[] items;
        items = newItems;
        capacity = newCapacity;
    }

public:
    // Constructor
    Vector(int initialCapacity = 10)
        : capacity(initialCapacity), size(0)
    {
        items = new T[capacity];
    }

    // Destructor
    ~Vector()
    {
        delete[] items;
    }

    // Copy constructor
    Vector(const Vector &other)
        : size(other.size), capacity(other.capacity)
    {
        items = new T[capacity];
        for (int i = 0; i < size; i++)
        {
            items[i] = other.items[i];
        }
    }

    // Assignment operator
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

    // Get element at index
    T get(int index) const
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

    // Access operator
    T &operator[](int index)
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

    // Remove element at index
    void remove(int index)
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }

        // Shift elements left
        for (int i = index; i < size - 1; i++)
        {
            items[i] = items[i + 1];
        }
        size--;
    }

    // Append element to the end
    void append(T value)
    {
        if (size >= capacity)
        {
            resize();
        }
        items[size] = value;
        size++;
    }

    // Insert element at specific index
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

        // Shift elements right
        for (int i = size; i > index; i--)
        {
            items[i] = items[i - 1];
        }
        items[index] = value;
        size++;
    }

    // Get current size
    int getSize() const
    {
        return size;
    }

    // Get current capacity
    int getCapacity() const
    {
        return capacity;
    }

    // Check if vector is empty
    bool isEmpty() const
    {
        return size == 0;
    }

    // Clear the vector
    void clear()
    {
        size = 0;
    }
};

#endif
