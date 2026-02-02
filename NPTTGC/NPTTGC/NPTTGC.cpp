// NPTTGC.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cstdio>
#include <string>
#include "Vector.h"
#include "Dictionary.h"

int main()
{
    // Example usage of Vector<int>
    printf("=== Vector<int> Example ===\n");
    Vector<int> intVector(5);
    
    // Append elements
    for (int i = 1; i <= 10; i++)
    {
        intVector.append(i * 10);
    }
    
    printf("Vector size: %d\n", intVector.getSize());
    printf("Vector capacity: %d\n", intVector.getCapacity());
    printf("Vector contents: ");
    for (int i = 0; i < intVector.getSize(); i++)
    {
        printf("%d ", intVector.get(i));
    }
    printf("\n\n");

    // Insert at specific index
    intVector.insert(0, 5);
    printf("After insert(0, 5): ");
    for (int i = 0; i < intVector.getSize(); i++)
    {
        printf("%d ", intVector.get(i));
    }
    printf("\n\n");

    // Example usage of Dictionary<std::string>
    printf("=== Dictionary<std::string> Example ===\n");
    Dictionary<std::string> userDict(5);

    // Insert key-value pairs
    userDict.insert("user1", "Alice");
    userDict.insert("user2", "Bob");
    userDict.insert("user3", "Charlie");
    userDict.insert("user4", "Diana");
    userDict.insert("user5", "Eve");

    printf("Dictionary size: %d\n", userDict.getSize());
    printf("Dictionary capacity: %d\n\n", userDict.getCapacity());

    // Get values
    printf("user1: %s\n", userDict.get("user1").c_str());
    printf("user3: %s\n", userDict.get("user3").c_str());

    // Check existence
    printf("Does 'user2' exist? %s\n", userDict.exists("user2") ? "Yes" : "No");
    printf("Does 'user99' exist? %s\n\n", userDict.exists("user99") ? "Yes" : "No");

    // Remove entry
    printf("Removing 'user2'...\n");
    userDict.remove("user2");
    printf("Dictionary size after removal: %d\n", userDict.getSize());
    printf("Does 'user2' exist? %s\n", userDict.exists("user2") ? "Yes" : "No");

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to add/manage files
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
