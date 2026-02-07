#ifndef REVIEW_H
#define REVIEW_H

#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include "Vector.h"
#include "Dictionary.h"
#include "CSVHelper.h"
#include "Member.h"

struct Review
{
    int reviewId;
    int userId;
    int gameId;
    int rating;
    std::string content;

    static std::string csvHeader() {
        return "reviewId,userId,gameId,rating,content";
    }

    static Review fromCSVRow(const Vector<std::string>& row) {
        Review review;
        review.reviewId = atoi(row.get(0).c_str());
        review.userId = atoi(row.get(1).c_str());
        review.gameId = atoi(row.get(2).c_str());
        review.rating = atoi(row.get(3).c_str());
        review.content = trim(row.get(4));
        return review;
    }

    static Vector<std::string> toCSVRow(const Review& review) {
        Vector<std::string> row;
        row.append(std::to_string(review.reviewId));
        row.append(std::to_string(review.userId));
        row.append(std::to_string(review.gameId));
        row.append(std::to_string(review.rating));
        row.append(escapeCSVField(review.content));
        return row;
    }
};

// Global data structures
Vector<Review> reviews;
Dictionary<int, int> reviewsByGame;  // Mapping from gameId to first reviewId for that game

// Helper function to safely read an integer from input
inline bool readIntegerReview(int& value)
{
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) == nullptr)
        return false;
    
    char* endPtr;
    value = strtol(buffer, &endPtr, 10);
    
    while (*endPtr && isspace(*endPtr))
        endPtr++;
    
    return (*endPtr == '\0');
}

// Function to load reviews from CSV
void loadReviewsFromCSV(Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame, const std::string& filename)
{
    reviews = buildFromFile<Review>(filename, Review::fromCSVRow);

    for (int i = 0; i < reviews.getSize(); i++) {
        Review review = reviews.get(i);
        reviewsByGame.insert(review.gameId, review.reviewId);
    }

    printf("Loaded %d reviews from %s\n", reviews.getSize(), filename.c_str());
}

// Function to save reviews to CSV
void saveReviewsToCSV(Vector<Review>& reviews, const std::string& filename)
{
    saveToFile<Review>(filename, Review::csvHeader(), reviews, Review::toCSVRow);
}

// Function to get next review ID
int getNextReviewId(Vector<Review>& reviews)
{
    int maxId = 0;
    for (int i = 0; i < reviews.getSize(); i++)
    {
        if (reviews.get(i).reviewId > maxId)
        {
            maxId = reviews.get(i).reviewId;
        }
    }
    return maxId + 1;
}

// Function to add a review for a game
void addReview(int userId, int gameId, Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame)
{
    printf("\n=== Write Review ===\n");

    // Get next review ID
    int reviewId = getNextReviewId(reviews);

    // Prompt for rating
    printf("Enter rating (1-5): ");
    int rating;
    if (!readIntegerReview(rating))
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    if (rating < 1 || rating > 5)
    {
        printf("Invalid rating. Please enter a value between 1 and 5.\n");
        return;
    }

    // Prompt for review content
    char contentBuf[500];
    printf("Enter review content: ");
    fgets(contentBuf, sizeof(contentBuf), stdin);
    contentBuf[strcspn(contentBuf, "\n")] = 0;

    // Create review
    Review newReview;
    newReview.reviewId = reviewId;
    newReview.userId = userId;
    newReview.gameId = gameId;
    newReview.rating = rating;
    newReview.content = contentBuf;

    // Add to reviews vector
    reviews.append(newReview);

    // Update reviewsByGame mapping - map gameId to reviewId
    // Store the reviewId in the dictionary
    reviewsByGame.insert(gameId, reviewId);

    printf("Review submitted successfully!\n");
    
    // Save changes to CSV
    saveReviewsToCSV(reviews, "reviews.csv");
}

// Function to display all reviews for a game
void displayGameReviews(int gameId, Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame)
{
    printf("\n=== Reviews for Game ID %d ===\n\n", gameId);

    // Collect all reviews for this game by scanning the reviews vector
    Vector<Review> gameReviewsList;
    for (int i = 0; i < reviews.getSize(); i++)
    {
        Review review = reviews.get(i);
        if (review.gameId == gameId)
        {
            gameReviewsList.append(review);
        }
    }

    if (gameReviewsList.isEmpty())
    {
        printf("No reviews found for this game.\n");
        return;
    }

    // Display each review
    printf("%-15s %-8s %-40s\n", "Username", "Rating", "Content");
    printf("%-15s %-8s %-40s\n", "--------", "------", "----------------------------------------");

    for (int i = 0; i < gameReviewsList.getSize(); i++)
    {
        Review review = gameReviewsList.get(i);
        std::string username = "Unknown";
        for (int j = 0; j < members.getSize(); j++)
        {
            if (members[j].id == review.userId)
            {
                username = members[j].username;
                break;
            }
        }
        printf("%-15s %-8d %-40s\n", username.c_str(), review.rating, review.content.c_str());
    }
    printf("\n");
}

// Function to display average rating for a game
void displayGameAverageRating(int gameId, Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame)
{
    // Collect all reviews for this game
    Vector<Review> gameReviewsList;
    for (int i = 0; i < reviews.getSize(); i++)
    {
        Review review = reviews.get(i);
        if (review.gameId == gameId)
        {
            gameReviewsList.append(review);
        }
    }

    if (gameReviewsList.isEmpty())
    {
        return;
    }

    // Calculate average rating
    int totalRating = 0;
    for (int i = 0; i < gameReviewsList.getSize(); i++)
    {
        Review review = gameReviewsList.get(i);
        totalRating += review.rating;
    }

    float averageRating = (float)totalRating / gameReviewsList.getSize();
    printf("Average rating for Game ID %d: %.2f/5 (%d reviews)\n", gameId, averageRating, gameReviewsList.getSize());
}

#endif
