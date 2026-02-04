#ifndef REVIEW_H
#define REVIEW_H

#include <string>
#include <cstdio>
#include <cstring>
#include "Vector.h"
#include "Dictionary.h"

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

// Function to add a review for a game
void addReview(int userId, int gameId, Vector<Review>& reviews, Dictionary<int, int>& reviewsByGame)
{
    printf("\n=== Write Review ===\n");

    // Get current review ID
    int reviewId = reviews.getSize();

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
    printf("%-10s %-8s %-40s\n", "User ID", "Rating", "Content");
    printf("%-10s %-8s %-40s\n", "-------", "------", "----------------------------------------");

    for (int i = 0; i < gameReviewsList.getSize(); i++)
    {
        Review review = gameReviewsList.get(i);
        printf("%-10d %-8d %-40s\n", review.userId, review.rating, review.content.c_str());
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
