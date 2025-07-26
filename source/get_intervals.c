#include <math.h>
#include "defs.h"

/*
 * Define a portable MIN macro for floats.
 * It uses the C99 fminf() function if the compiler supports it (GCC, Clang, modern MSVC).
 * Otherwise, it falls back to a C89-compatible ternary operator.
 *
 * __STDC_VERSION__ >= 199901L checks for C99 or newer standards.
 * _MSC_VER checks for the Microsoft Visual C++ compiler.
 */
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(_MSC_VER)
    #define GAME_MIN_F(a, b) fminf(a, b)
#else
    /* C89 fallback. Parentheses prevent operator precedence issues. */
    #define GAME_MIN_F(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* --- A Common Optimization Trick --- */
/* Pre-calculate the reciprocal to replace division with multiplication. */
#define ONE_OVER_SCORE_CAP (1.0f / SCORE_CAP)
#define INTERVAL_RANGE (MAX_INTERVAL - MIN_INTERVAL)

/**
 * @brief Calculates the update interval using a simple linear mapping.
 * @param score The player's current score.
 * @return The calculated update interval in seconds.
 */
float get_interval_linear(int score) {
	float capped_score;
    float progress;

	/* Clamp the score to the maximum value and cast to float */
	capped_score = GAME_MIN_F((float)score, SCORE_CAP);

	/* Calculate progress from 0.0 to 1.0 using multiplication (faster than division) */
	progress = capped_score * ONE_OVER_SCORE_CAP;

	return MAX_INTERVAL - progress * INTERVAL_RANGE;
}

/**
 * @brief Calculates the update interval using a quadratic "ease-in" curve.
 * This makes the difficulty ramp up more slowly at first, then much faster.
 * @param score The player's current score.
 * @return The calculated update interval in seconds.
 */
float get_interval_quadratic(int score) {
	float capped_score;
    float progress;
    float eased_progress;

	capped_score = GAME_MIN_F((float)score, SCORE_CAP);
	progress = capped_score * ONE_OVER_SCORE_CAP;

	/* The key step: square the progress to create the curve */
	eased_progress = progress * progress;

	return MAX_INTERVAL - eased_progress * INTERVAL_RANGE;
}
