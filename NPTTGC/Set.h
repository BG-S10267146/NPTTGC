#ifndef SET_H
#define SET_H

#include <string>
#include <stdexcept>

// Generic SetNode for storing values
template <typename T>
class SetNode
{
public:
    std::string key;
    T value;
    SetNode<T>* next;

    // Constructor
    SetNode(const std::string& k, const T& val)
        : key(k), value(val), next(nullptr)
    {
    }
};

// Set implementation using hash table
// Stores unique values indexed by string keys
template <typename T>
class Set
{
private:
    SetNode<T>** items;
    int size;
    int capacity;

    // Hash function for string keys
    int hash(const std::string& key) const
    {
        int hashValue = 0;
        for (size_t i = 0; i < key.length(); i++)
        {
            hashValue = (hashValue * 31 + key[i]) % capacity;
        }
        return (hashValue + capacity) % capacity;
    }

    // Helper function to resize hash table
    void resize()
    {
        int oldCapacity = capacity;
        SetNode<T>** oldItems = items;

        // Create new hash table with doubled capacity
        capacity = capacity * 2;
        items = new SetNode<T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        // Rehash all existing entries into the new table
        for (int i = 0; i < oldCapacity; i++)
        {
            SetNode<T>* current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->key, current->value);
                current = current->next;
            }
        }

        // Delete old hash table
        for (int i = 0; i < oldCapacity; i++)
        {
            SetNode<T>* current = oldItems[i];
            while (current != nullptr)
            {
                SetNode<T>* temp = current;
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
        items = new SetNode<T>*[capacity];
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
            SetNode<T>* current = items[i];
            while (current != nullptr)
            {
                SetNode<T>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] items;
    }

    // Copy constructor
    Set(const Set& other)
        : capacity(other.capacity), size(other.size)
    {
        items = new SetNode<T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        for (int i = 0; i < other.capacity; i++)
        {
            SetNode<T>* otherCurrent = other.items[i];
            while (otherCurrent != nullptr)
            {
                insert(otherCurrent->key, otherCurrent->value);
                otherCurrent = otherCurrent->next;
            }
        }
    }

    // Assignment operator
    Set& operator=(const Set& other)
    {
        if (this != &other)
        {
            for (int i = 0; i < capacity; i++)
            {
                SetNode<T>* current = items[i];
                while (current != nullptr)
                {
                    SetNode<T>* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] items;

            capacity = other.capacity;
            size = 0;
            items = new SetNode<T>*[capacity];
            for (int i = 0; i < capacity; i++)
            {
                items[i] = nullptr;
            }

            for (int i = 0; i < other.capacity; i++)
            {
                SetNode<T>* otherCurrent = other.items[i];
                while (otherCurrent != nullptr)
                {
                    insert(otherCurrent->key, otherCurrent->value);
                    otherCurrent = otherCurrent->next;
                }
            }
        }
        return *this;
    }

    // Insert value with key
    void insert(const std::string& key, const T& value)
    {
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = hash(key);
        SetNode<T>* current = items[index];

        // Check if key already exists
        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->value = value;
                return;
            }
            current = current->next;
        }

        // Key doesn't exist, insert at the beginning
        SetNode<T>* newNode = new SetNode<T>(key, value);
        newNode->next = items[index];
        items[index] = newNode;
        size++;
    }

    // Remove value by key
    bool remove(const std::string& key)
    {
        int index = hash(key);
        SetNode<T>* current = items[index];
        SetNode<T>* prev = nullptr;

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

    // Get value by key
    T get(const std::string& key) const
    {
        int index = hash(key);
        SetNode<T>* current = items[index];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                return current->value;
            }
            current = current->next;
        }

        throw std::runtime_error("Key not found in set");
    }

    // Check if key exists
    bool exists(const std::string& key) const
    {
        int index = hash(key);
        SetNode<T>* current = items[index];

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
            SetNode<T>* current = items[i];
            while (current != nullptr)
            {
                SetNode<T>* temp = current;
                current = current->next;
                delete temp;
            }
            items[i] = nullptr;
        }
        size = 0;
    }
};

#endif
