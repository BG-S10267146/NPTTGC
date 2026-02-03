#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <stdexcept>

// Generic DictionaryNode for any key type
template <typename K, typename V>
class DictionaryNode
{
public:
    K key;
    V item;
    DictionaryNode<K, V>* next;

    // Constructor
    DictionaryNode(const K& k, const V& value)
        : key(k), item(value), next(nullptr)
    {
    }
};

// Dictionary implementation with generic key support
// Note: String keys are handled specially (character-by-character hashing)
// All other data types (like int, float, etc.) are hashed by their raw byte values
template <typename K, typename V>
class Dictionary
{
private:
    DictionaryNode<K, V>** items;  // Array of pointers to DictionaryNode
    int size;
    int capacity;

    // Hash function for std::string - hashes character content
    int hash(const std::string& key) const
    {
        int hashValue = 0;
        for (size_t i = 0; i < key.length(); i++)
        {
            hashValue = (hashValue * 31 + key[i]) % capacity;
        }
        return (hashValue + capacity) % capacity;  // Ensure positive value
    }

    // Hash function for generic types - hashes raw bytes
    template <typename T>
    int hash(const T& key) const
    {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&key);
        int hashValue = 0;
        
        for (size_t i = 0; i < sizeof(T); i++)
        {
            hashValue = (hashValue * 31 + bytes[i]) % capacity;
        }
        return (hashValue + capacity) % capacity;  // Ensure positive value
    }

    // Helper function to resize hash table
    void resize()
    {
        int oldCapacity = capacity;
        DictionaryNode<K, V>** oldItems = items;

        // Create new hash table with doubled capacity
        capacity = capacity * 2;
        items = new DictionaryNode<K, V>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        size = 0;

        // Rehash all existing entries into the new table
        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<K, V>* current = oldItems[i];
            while (current != nullptr)
            {
                insert(current->key, current->item);
                current = current->next;
            }
        }

        // Delete old hash table
        for (int i = 0; i < oldCapacity; i++)
        {
            DictionaryNode<K, V>* current = oldItems[i];
            while (current != nullptr)
            {
                DictionaryNode<K, V>* temp = current;
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
        items = new DictionaryNode<K, V>*[capacity];
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
            DictionaryNode<K, V>* current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<K, V>* temp = current;
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
        items = new DictionaryNode<K, V>*[capacity];
        for (int i = 0; i < capacity; i++)
        {
            items[i] = nullptr;
        }

        // Deep copy all nodes
        for (int i = 0; i < other.capacity; i++)
        {
            DictionaryNode<K, V>* otherCurrent = other.items[i];
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
                DictionaryNode<K, V>* current = items[i];
                while (current != nullptr)
                {
                    DictionaryNode<K, V>* temp = current;
                    current = current->next;
                    delete temp;
                }
            }
            delete[] items;

            // Copy from other
            capacity = other.capacity;
            size = 0;
            items = new DictionaryNode<K, V>*[capacity];
            for (int i = 0; i < capacity; i++)
            {
                items[i] = nullptr;
            }

            for (int i = 0; i < other.capacity; i++)
            {
                DictionaryNode<K, V>* otherCurrent = other.items[i];
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
    void insert(const K& key, const V& item)
    {
        // Check load factor and resize if necessary
        if (size >= capacity / 2)
        {
            resize();
        }

        int index = hash(key);
        DictionaryNode<K, V>* current = items[index];

        // Check if key already exists and update it
        while (current != nullptr)
        {
            if (current->key == key)
            {
                current->item = item;
                return;
            }
            current = current->next;
        }

        // Key doesn't exist, insert at the beginning of the chain
        DictionaryNode<K, V>* newNode = new DictionaryNode<K, V>(key, item);
        newNode->next = items[index];
        items[index] = newNode;
        size++;
    }

    // Remove key-value pair by key
    bool remove(const K& key)
    {
        int index = hash(key);
        DictionaryNode<K, V>* current = items[index];
        DictionaryNode<K, V>* prev = nullptr;

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

        return false;  // Key not found
    }

    // Get value by key
    V get(const K& key) const
    {
        int index = hash(key);
        DictionaryNode<K, V>* current = items[index];

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

    // Check if key exists
    bool exists(const K& key) const
    {
        int index = hash(key);
        DictionaryNode<K, V>* current = items[index];

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
            DictionaryNode<K, V>* current = items[i];
            while (current != nullptr)
            {
                DictionaryNode<K, V>* temp = current;
                current = current->next;
                delete temp;
            }
            items[i] = nullptr;
        }
        size = 0;
    }
};

#endif
