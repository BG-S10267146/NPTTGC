#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <stdexcept>
#include <cstring>

// Generic DictionaryNode for any key type
template <typename K, typename T>
class DictionaryNode
{
public:
    K key;
    T item;
    DictionaryNode<K, T>* next;

    // Constructor
    DictionaryNode(const K& k, const T& value)
        : key(k), item(value), next(nullptr)
    {
    }
};

// Generic Dictionary implementation
template <typename K, typename T>
class Dictionary
{
private:
    DictionaryNode<K, T>** items;  // Array of pointers to DictionaryNode
    int size;
    int capacity;

    // Hash function for generic types - hashes raw bytes
    int hash(const K& key) const
    {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&key);
        int hashValue = 0;
        
        for (size_t i = 0; i < sizeof(K); i++)
        {
            hashValue = (hashValue * 31 + bytes[i]) % capacity;
        }
        return (hashValue + capacity) % capacity;  // Ensure positive value
    }

    // Helper function to resize hash table
    void resize()
    {
        int oldCapacity = capacity;
        DictionaryNode<K, T>** oldItems = items;

        // Create new hash table with doubled capacity
        capacity = capacity * 2;
        items = new DictionaryNode<K, T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        // Rehash all existing entries into the new table
        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<K, T>* current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->key, current->item);
                current = current->next;
            }
        }

        // Delete old hash table
        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<K, T>* current = oldItems[i];
            while (current != nullptr)
            {
                DictionaryNode<K, T>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] oldItems;
    }

public:
    // Constructor
    Dictionary(int initialCapacity = 10)
        : capacity(initialCapacity), size(0)
    {
        items = new DictionaryNode<K, T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }
    }

    // Destructor
    ~Dictionary()
    {
        // Clean up all nodes
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<K, T>* current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<K, T>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] items;
    }

    // Copy constructor
    Dictionary(const Dictionary& other)
        : capacity(other.capacity), size(other.size)
    {
        items = new DictionaryNode<K, T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        // Deep copy all nodes
        for (int i = 0; i < other.capacity; i++)
        {
            DictionaryNode<K, T>* otherCurrent = other.items[i];
            while (otherCurrent != nullptr)
            {
                insert(otherCurrent->key, otherCurrent->item);
                otherCurrent = otherCurrent->next;
            }
        }
    }

    // Assignment operator
    Dictionary& operator=(const Dictionary& other)
    {
        if (this != &other)
        {
            // Clean up current data
            for (int i = 0; i < capacity; i++)
            {
                DictionaryNode<K, T>* current = items[i];
                while (current != nullptr)
                {
                    DictionaryNode<K, T>* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] items;

            // Copy from other
            capacity = other.capacity;
            size = 0;
            items = new DictionaryNode<K, T>*[capacity];
            for (int i = 0; i < capacity; i++)
            {
                items[i] = nullptr;
            }

            for (int i = 0; i < other.capacity; i++)
            {
                DictionaryNode<K, T>* otherCurrent = other.items[i];
                while (otherCurrent != nullptr)
                {
                    insert(otherCurrent->key, otherCurrent->item);
                    otherCurrent = otherCurrent->next;
                }
            }
        }
        return *this;
    }

    // Insert or update key-value pair
    void insert(const K& key, const T& item)
    {
        // Check load factor and resize if necessary
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = hash(key);
        DictionaryNode<K, T>* current = items[index];

        // Check if key already exists and update it
        while (current != nullptr)
        {
            if (memcmp(&current->key, &key, sizeof(K)) == 0)
            {
                current->item = item;
                return;
            }
            current = current->next;
        }

        // Key doesn't exist, insert at the beginning of the chain
        DictionaryNode<K, T>* newNode = new DictionaryNode<K, T>(key, item);
        newNode->next = items[index];
        items[index] = newNode;
        size++;
    }

    // Remove key-value pair by key
    bool remove(const K& key)
    {
        int index = hash(key);
        DictionaryNode<K, T>* current = items[index];
        DictionaryNode<K, T>* prev = nullptr;

        while (current != nullptr)
        {
            if (memcmp(&current->key, &key, sizeof(K)) == 0)
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

        return false;  // Key not found
    }

    // Get value by key
    T get(const K& key) const
    {
        int index = hash(key);
        DictionaryNode<K, T>* current = items[index];

        while (current != nullptr)
        {
            if (memcmp(&current->key, &key, sizeof(K)) == 0)
            {
                return current->item;
            }
            current = current->next;
        }

        throw std::runtime_error("Key not found in dictionary");
    }

    // Check if key exists
    bool exists(const K& key) const
    {
        int index = hash(key);
        DictionaryNode<K, T>* current = items[index];

        while (current != nullptr)
        {
            if (memcmp(&current->key, &key, sizeof(K)) == 0)
            {
                return true;
            }
            current = current->next;
        }

        return false;
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
            DictionaryNode<K, T>* current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<K, T>* temp = current;
                current = current->next;
                delete temp;
            }
            items[i] = nullptr;
        }
        size = 0;
    }
};

// Template specialization for std::string keys (optimized string hashing)
template <typename T>
class Dictionary<std::string, T>
{
private:
    DictionaryNode<std::string, T>** items;
    int size;
    int capacity;

    // Optimized hash function for std::string
    int hash(const std::string& key) const
    {
        int hashValue = 0;
        for (size_t i = 0; i < key.length(); i++)
        {
            hashValue = (hashValue * 31 + key[i]) % capacity;
        }
        return (hashValue + capacity) % capacity;
    }

    void resize()
    {
        int oldCapacity = capacity;
        DictionaryNode<std::string, T>** oldItems = items;

        capacity = capacity * 2;
        items = new DictionaryNode<std::string, T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<std::string, T>* current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->key, current->item);
                current = current->next;
            }
        }

        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<std::string, T>* current = oldItems[i];
            while (current != nullptr)
            {
                DictionaryNode<std::string, T>* temp = current;
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
        items = new DictionaryNode<std::string, T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }
    }

    ~Dictionary()
    {
        for (int i = 0; i < capacity; i++)
        {
            DictionaryNode<std::string, T>* current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<std::string, T>* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] items;
    }

    Dictionary(const Dictionary& other)
        : capacity(other.capacity), size(other.size)
    {
        items = new DictionaryNode<std::string, T>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        for (int i = 0; i < other.capacity; i++)
        {
            DictionaryNode<std::string, T>* otherCurrent = other.items[i];
            while (otherCurrent != nullptr)
            {
                insert(otherCurrent->key, otherCurrent->item);
                otherCurrent = otherCurrent->next;
            }
        }
    }

    Dictionary& operator=(const Dictionary& other)
    {
        if (this != &other)
        {
            for (int i = 0; i < capacity; i++)
            {
                DictionaryNode<std::string, T>* current = items[i];
                while (current != nullptr)
                {
                    DictionaryNode<std::string, T>* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] items;

            capacity = other.capacity;
            size = 0;
            items = new DictionaryNode<std::string, T>*[capacity];
            for (int i = 0; i < capacity; i++)
            {
                items[i] = nullptr;
            }

            for (int i = 0; i < other.capacity; i++)
            {
                DictionaryNode<std::string, T>* otherCurrent = other.items[i];
                while (otherCurrent != nullptr)
                {
                    insert(otherCurrent->key, otherCurrent->item);
                    otherCurrent = otherCurrent->next;
                }
            }
        }
        return *this;
    }

    void insert(const std::string& key, const T& item)
    {
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = hash(key);
        DictionaryNode<std::string, T>* current = items[index];

        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->item = item;
                return;
            }
            current = current->next;
        }

        DictionaryNode<std::string, T>* newNode = new DictionaryNode<std::string, T>(key, item);
        newNode->next = items[index];
        items[index] = newNode;
        size++;
    }

    bool remove(const std::string& key)
    {
        int index = hash(key);
        DictionaryNode<std::string, T>* current = items[index];
        DictionaryNode<std::string, T>* prev = nullptr;

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

    T get(const std::string& key) const
    {
        int index = hash(key);
        DictionaryNode<std::string, T>* current = items[index];

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

    bool exists(const std::string& key) const
    {
        int index = hash(key);
        DictionaryNode<std::string, T>* current = items[index];

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
            DictionaryNode<std::string, T>* current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<std::string, T>* temp = current;
                current = current->next;
                delete temp;
            }
            items[i] = nullptr;
        }
        size = 0;
    }
};

#endif
