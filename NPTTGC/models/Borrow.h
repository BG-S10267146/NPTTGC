#ifndef BORROW_H
#define BORROW_H

#include <string>
#include "../lib/Vector.h"

/// Borrow record data structure
struct Borrow
{
    int borrowId;
    int memberId;
    int gameId;
    std::string dateBorrowed;
    std::string dateReturned;

    Borrow() : borrowId(0), memberId(0), gameId(0), dateBorrowed(""), dateReturned("") {}

    static std::string csvHeader();
    static Borrow fromCSVRow(const Vector<std::string>& row);
    static Vector<std::string> toCSVRow(const Borrow& borrow);
};

#endif
