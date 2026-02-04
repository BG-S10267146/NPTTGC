#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include "Vector.h"
#include "Dictionary.h"

// Type definitions
typedef int MemberID;

struct Member
{
    MemberID id;
    std::string username;
};

// Global data structures
Vector<Member> members;
Dictionary<std::string, MemberID> membersByUsername;

// Function to add a new member
void addMember(Vector<Member>& members, Dictionary<std::string, MemberID>& membersByUsername, const std::string& username)
{
    // Check if username already exists
    if (membersByUsername.exists(username))
    {
        printf("Error: Username '%s' already exists.\n", username.c_str());
        return;
    }

    // Create new member
    MemberID memberId = members.getSize();  // Use current size as ID
    Member newMember;
    newMember.id = memberId;
    newMember.username = username;

    // Add to members vector
    members.append(newMember);

    // Add to membersByUsername dictionary
    membersByUsername.insert(username, memberId);

    printf("Member '%s' added successfully with ID %d.\n", username.c_str(), memberId);
}

#endif
