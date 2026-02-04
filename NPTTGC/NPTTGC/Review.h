#ifndef REVIEW_H
#define REVIEW_H

#include <string>
#include <cstdio>
#include <cstring>
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
};

// Global data structures
Vector<Review> reviews;
Dictionary<int, int> reviewsByGame;  // Mapping from gameId to first reviewId for that game

// Function to load reviews from CSV
void loadReviewsFromCSV(Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame, const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        printf("Warning: Could not open %s. Starting with empty reviews list.\n", filename.c_str());
        return;
    }

    std::string line;
    // Skip header line
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        Vector<std::string> fields = splitCSVLine(line);
        if (fields.getSize() >= 5)
        {
            Review review;
            review.reviewId = atoi(fields.get(0).c_str());
            review.userId = atoi(fields.get(1).c_str());
            review.gameId = atoi(fields.get(2).c_str());
            review.rating = atoi(fields.get(3).c_str());
            review.content = trim(fields.get(4));
            
            reviews.append(review);
            reviewsByGame.insert(review.gameId, review.reviewId);
        }
    }
    file.close();

    printf("Loaded %d reviews from %s\n", reviews.getSize(), filename.c_str());
}

// Function to save reviews to CSV
void saveReviewsToCSV(Vector<Review>& reviews, const std::string& filename)
{
    std::ofstream file(filename.c_str());
    if (!file.is_open())
    {
        printf("Error: Could not open %s for writing.\n", filename.c_str());
        return;
    }

    // Write header
    file << "reviewId,userId,gameId,rating,content\n";

    // Write reviews
    for (int i = 0; i < reviews.getSize(); i++)
    {
        Review review = reviews.get(i);
        file << review.reviewId << ","
             << review.userId << ","
             << review.gameId << ","
             << review.rating << ","
             << escapeCSVField(review.content) << "\n";
    }

    file.close();
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
    scanf("%d", &rating);
    getchar();

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

    // Check if game has reviews
    if (!reviewsByGame.exists(gameId))
    {
        printf("No reviews found for this game.\n");
        return;
    }

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
    if (!reviewsByGame.exists(gameId))
    {
        printf("No reviews found for this game.\n");
        return;
    }

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
        printf("No reviews found for this game.\n");
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
