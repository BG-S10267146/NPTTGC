#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include "../lib/Vector.h"

/// Member data structure
struct Member
{
    int id;
    std::string username;
    bool isAdmin;

    Member() : id(0), username(""), isAdmin(false) {}
    Member(int id, const std::string& username, bool isAdmin) 
        : id(id), username(username), isAdmin(isAdmin) {}

    static std::string csvHeader();
    static Member fromCSVRow(const Vector<std::string>& row);
    static Vector<std::string> toCSVRow(const Member& member);
};

#endif
