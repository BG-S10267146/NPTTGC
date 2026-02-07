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

    T get(int index) const
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

    T &operator[](int index)
    {
        if (index < 0 || index >= size)
        {
            throw std::out_of_range("Vector index out of range");
        }
        return items[index];
    }

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

    void append(T value)
    {
        if (size >= capacity)
        {
            resize();
        }
        items[size] = value;
        size++;
    }

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
        size = 0;
    }
};

#endif
