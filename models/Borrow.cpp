#include "Borrow.h"
#include "../lib/CSVHelper.h"
#include <cstdlib>

std::string Borrow::csvHeader()
{
    return "borrowId,memberId,gameId,dateBorrowed,dateReturned";
}

Borrow Borrow::fromCSVRow(const Vector<std::string>& row)
{
    Borrow borrow;
    borrow.borrowId = atoi(row.get(0).c_str());
    borrow.memberId = atoi(row.get(1).c_str());
    borrow.gameId = atoi(row.get(2).c_str());
    int offset = (row.getSize() >= 6) ? 1 : 0;
    borrow.dateBorrowed = trim(row.get(3 + offset));
    borrow.dateReturned = trim(row.get(4 + offset));
    return borrow;
}

Vector<std::string> Borrow::toCSVRow(const Borrow& borrow)
{
    Vector<std::string> row;
    row.append(std::to_string(borrow.borrowId));
    row.append(std::to_string(borrow.memberId));
    row.append(std::to_string(borrow.gameId));
    row.append(borrow.dateBorrowed);
    row.append(borrow.dateReturned);
    return row;
}
