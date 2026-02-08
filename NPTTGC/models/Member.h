#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include "../lib/Vector.h"

typedef int MemberID;

/// Member data structure
struct Member
{
    MemberID id;
    std::string username;
    bool isAdmin;

    Member() : id(0), username(""), isAdmin(false) {}
    Member(MemberID id, const std::string& username, bool isAdmin) 
        : id(id), username(username), isAdmin(isAdmin) {}

    static std::string csvHeader();
    static Member fromCSVRow(const Vector<std::string>& row);
    static Vector<std::string> toCSVRow(const Member& member);
};

#endif
