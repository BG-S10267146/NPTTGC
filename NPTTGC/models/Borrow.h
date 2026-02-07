#ifndef BORROW_H
#define BORROW_H

#include <string>
#include "../lib/Vector.h"

typedef int GameID;

/// Borrow record data structure
struct Borrow
{
    int borrowId;
    int memberId;
    int gameId;
    std::string dateBorrowed;
    std::string dateReturned;

    static std::string csvHeader();
    static Borrow fromCSVRow(const Vector<std::string>& row);
    static Vector<std::string> toCSVRow(const Borrow& borrow);
};

#endif
