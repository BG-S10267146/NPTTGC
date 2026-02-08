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

    Review() : reviewId(0), userId(0), gameId(0), rating(0), content("") {}

    Review(int reviewId, int userId, int gameId, int rating, const std::string &content)
        : reviewId(reviewId), userId(userId), gameId(gameId),
          rating(rating), content(content) {}

    static std::string csvHeader();
    static Review fromCSVRow(const Vector<std::string> &row);
    static Vector<std::string> toCSVRow(const Review &review);
};

#endif
