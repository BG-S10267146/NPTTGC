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

    static std::string csvHeader();
    static Member fromCSVRow(const Vector<std::string>& row);
    static Vector<std::string> toCSVRow(const Member& member);
};

#endif
