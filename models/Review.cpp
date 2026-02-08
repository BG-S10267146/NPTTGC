#include "Review.h"
#include "../lib/CSVHelper.h"
#include <cstdlib>

std::string Review::csvHeader()
{
    return "reviewId,userId,gameId,rating,content";
}

Review Review::fromCSVRow(const Vector<std::string>& row)
{
    Review review;
    review.reviewId = atoi(row.get(0).c_str());
    review.userId = atoi(row.get(1).c_str());
    review.gameId = atoi(row.get(2).c_str());
    review.rating = atoi(row.get(3).c_str());
    review.content = trim(row.get(4));
    return review;
}

Vector<std::string> Review::toCSVRow(const Review& review)
{
    Vector<std::string> row;
    row.append(std::to_string(review.reviewId));
    row.append(std::to_string(review.userId));
    row.append(std::to_string(review.gameId));
    row.append(std::to_string(review.rating));
    row.append(review.content);
    return row;
}
