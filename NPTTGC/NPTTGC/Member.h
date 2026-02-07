#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "Vector.h"
#include "Dictionary.h"
#include "CSVHelper.h"

// Type definitions
typedef int MemberID;

struct Member
{
    MemberID id;
    std::string username;
    bool isAdmin;

    static std::string csvHeader() {
        return "id,username,isAdmin";
    }

    static Member fromCSVRow(const Vector<std::string>& row) {
        Member member;
        member.id = atoi(row.get(0).c_str());
        member.username = trim(row.get(1));
        member.isAdmin = (atoi(row.get(2).c_str()) == 1);
        return member;
    }

    static Vector<std::string> toCSVRow(const Member& member) {
        Vector<std::string> row;
        row.append(std::to_string(member.id));
        row.append(member.username);
        row.append(std::to_string(member.isAdmin ? 1 : 0));
        return row;
    }
};

// Global data structures
Vector<Member> members;
Dictionary<std::string, MemberID> membersByUsername;

// Function to load members from CSV
void loadMembersFromCSV(Vector<Member>& members, Dictionary<std::string, MemberID>& membersByUsername, const std::string& filename)
{
    members = buildFromFile<Member>(filename, Member::fromCSVRow);

    if (members.isEmpty()) {
        printf("Starting with default admin account.\n");
        Member admin;
        admin.id = 1;
        admin.username = "admin";
        admin.isAdmin = true;
        members.append(admin);
    }

    for (int i = 0; i < members.getSize(); i++) {
        Member member = members.get(i);
        membersByUsername.insert(member.username, member.id);
    }

    printf("Loaded %d members from %s\n", members.getSize(), filename.c_str());
}

// Function to save members to CSV
void saveMembersToCSV(Vector<Member>& members, const std::string& filename)
{
    saveToFile<Member>(filename, Member::csvHeader(), members, Member::toCSVRow);
}

// Function to get next member ID
int getNextMemberId(Vector<Member>& members)
{
    int maxId = 0;
    for (int i = 0; i < members.getSize(); i++)
    {
        if (members.get(i).id > maxId)
        {
            maxId = members.get(i).id;
        }
    }
    return maxId + 1;
}

// Function to add a new member
void addMember(Vector<Member>& members, Dictionary<std::string, MemberID>& membersByUsername, const std::string& username, bool isAdmin = false)
{
    // Check if username already exists
    if (membersByUsername.exists(username))
    {
        printf("Error: Username '%s' already exists.\n", username.c_str());
        return;
    }

    // Create new member
    MemberID memberId = getNextMemberId(members);
    Member newMember;
    newMember.id = memberId;
    newMember.username = username;
    newMember.isAdmin = isAdmin;

    // Add to members vector
    members.append(newMember);

    // Add to membersByUsername dictionary
    membersByUsername.insert(username, memberId);

    printf("Member '%s' added successfully with ID %d.\n", username.c_str(), memberId);
    
    // Save changes to CSV
    saveMembersToCSV(members, "members.csv");
}

// Function to authenticate a member
Member* authenticateMember(Vector<Member>& members, Dictionary<std::string, MemberID>& membersByUsername, const std::string& username)
{
    if (!membersByUsername.exists(username))
    {
        return nullptr;
    }

    MemberID memberId = membersByUsername.get(username);
    
    // Find member in vector
    for (int i = 0; i < members.getSize(); i++)
    {
        if (members[i].id == memberId)
        {
            return &members[i];
        }
    }
    
    return nullptr;
}

#endif
