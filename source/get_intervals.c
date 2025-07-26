#include <math.h>
#include "defs.h"

// --- A Common Optimization Trick ---
// Pre-calculate the reciprocal to replace division with multiplication.
#define ONE_OVER_SCORE_CAP (1.0f / SCORE_CAP)
#define INTERVAL_RANGE (MAX_INTERVAL - MIN_INTERVAL)

/**
 * @brief Calculates the update interval using a simple linear mapping.
 * @param score The player's current score.
 * @return The calculated update interval in seconds.
 */
float get_interval_linear(int score) {
	// Clamp the score to the maximum value and cast to float
	float capped_score = fminf((float)score, SCORE_CAP);

	// Calculate progress from 0.0 to 1.0 using multiplication (faster than division)
	float progress = capped_score * ONE_OVER_SCORE_CAP;

	return MAX_INTERVAL - progress * INTERVAL_RANGE;
}

/**
 * @brief Calculates the update interval using a quadratic "ease-in" curve.
 * This makes the difficulty ramp up more slowly at first, then much faster.
 * @param score The player's current score.
 * @return The calculated update interval in seconds.
 */
float get_interval_quadratic(int score) {
	float capped_score = fminf((float)score, SCORE_CAP);
	float progress = capped_score * ONE_OVER_SCORE_CAP;

	// The key step: square the progress to create the curve
	float eased_progress = progress * progress;

	return MAX_INTERVAL - eased_progress * INTERVAL_RANGE;
}
