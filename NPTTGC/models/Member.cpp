#include "Member.h"
#include "../lib/CSVHelper.h"
#include <cstdlib>

std::string Member::csvHeader()
{
    return "id,username,isAdmin";
}

Member Member::fromCSVRow(const Vector<std::string>& row)
{
    return Member(
        atoi(row.get(0).c_str()),
        trim(row.get(1)),
        atoi(row.get(2).c_str()) == 1
    );
}

Vector<std::string> Member::toCSVRow(const Member& member)
{
    Vector<std::string> row;
    row.append(std::to_string(member.id));
    row.append(member.username);
    row.append(std::to_string(member.isAdmin ? 1 : 0));
    return row;
}
