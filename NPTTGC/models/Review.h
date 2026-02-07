#ifndef REVIEW_H
#define REVIEW_H

#include <string>
#include "../lib/Vector.h"

/// Review data structure
struct Review
{
    int reviewId;
    int userId;
    int gameId;
    int rating;
    std::string content;

    static std::string csvHeader();
    static Review fromCSVRow(const Vector<std::string>& row);
    static Vector<std::string> toCSVRow(const Review& review);
};

#endif
