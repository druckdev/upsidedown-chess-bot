#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// clang-format off
// NOTE(Aurel): these need to be included in this order
#include "timer.h"
#include "chess.h"
// clang-format on

#define CLOCK CLOCK_MONOTONIC
#define FUNC_UNIFORM_DISTRIBUTION

struct chess_timer*
start_timer(float t_total_s)
{
	struct timespec t_cur;
	if (clock_gettime(CLOCK, &t_cur))
		return NULL;

	struct chess_timer* timer = malloc(sizeof(*timer));
	if (!timer)
		return NULL;

	// offset timer by the total time
	timer->t_end = t_cur;
	timer->t_end.tv_sec += t_total_s;
	return timer;
}

/* Different functions to calculate the time the current move has to spend. */
// NOTE(Aurel): When adding new functions, don't forget to add them to the enum.

// TODO(Aurel): Benchmark if passing by reference and malloc is faster than
// passing by value.
struct timespec
uniform_distribution(struct chess_timer* timer, struct chess* game)
{
	struct timespec t_cur;
	if (clock_gettime(CLOCK, &t_cur) != 0)
		return (struct timespec){ 0 };

	// clang-format off
	struct timespec t_remaining = {
		timer->t_end.tv_sec - t_cur.tv_sec,
		timer->t_end.tv_nsec - t_cur.tv_nsec
	};
	// clang-format on

	// uniformly distribute the remaining time over the remaining moves
	int remaining_moves = game->max_moves - game->move_count;
	if(!remaining_moves)
		return (struct timespec){ 0 };
	// clang-format off
	struct timespec t_move = {
		t_remaining.tv_sec / remaining_moves,
		t_remaining.tv_nsec / remaining_moves
	};
	// clang-format on

	return t_move;
}

// helper that defines the function (in the mathmatical sense)
// which distributes time for moves
float
f(float uniform, int n, int x)
{
	float uniform_percent = uniform * 0.01;
	if (x < n * 0.5) {
		// first half, function rise

		if (x < n * 0.25) {
			// first quater, stays below uniform value, to 'allocate' time
			return uniform - (uniform_percent * (n * 0.25 - x));
		}

		// second quater, goes above uniform value, uses 'allocated' time
		return uniform + (uniform_percent * (x - n * 0.25));
	}

	// second half, function fall
	if (x < n * 0.75) {
		// third quater, still above uniform value
		return uniform + (uniform_percent * (n * 0.75 - x));
	}

	// fourth quater, goes below uniform value,
	return uniform - (uniform_percent * (x - n * 0.75));
}

// distributes time in a pyramid like manner (rising until the middle
// and then falling)
struct timespec
pyramid_distribution(struct chess_timer* timer, struct chess* game)
{
	// return if no time is left
	struct timespec t_cur;
	if (clock_gettime(CLOCK, &t_cur) != 0)
		return (struct timespec){ 0 };

	// get remaining time and remaining moves
	// clang-format off
	struct timespec t_remaining = {
		timer->t_end.tv_sec - t_cur.tv_sec,
		timer->t_end.tv_nsec - t_cur.tv_nsec
	};
	// clang-format on

	int remaining_moves = game->max_moves - game->move_count;

	// get the uniform distribution of the remaining time
	// over the remaining moves
	time_t uniform_sec = t_remaining.tv_sec / remaining_moves;
	time_t uniform_nsec = t_remaining.tv_nsec / remaining_moves;

	// get the y (time) value for the given x (current move)
	time_t sec = f((float)(uniform_sec), game->max_moves, game->move_count);
	time_t nsec = f((float)(uniform_nsec), game->max_moves, game->move_count);

	// clang-format off
	struct timespec t_move = {
		sec,
		nsec
	};
	// clang-format on

	return t_move;
}

/* \ Different functions to calculate the time the current move has to spend. */

struct timespec
get_move_time(struct chess_timer* timer, struct chess* game,
              enum t_move_distribution_function method)
{
	struct timespec ts;
	switch (method) {
	case UNIFORM_DISTRIBUTION:
		ts = uniform_distribution(timer, game);
		break;
	case PYRAMID_DISTRIBUTION:
		ts = pyramid_distribution(timer, game);
		break;
	default:
		assert(false && "No timer function is being used");
	}
	return ts;
}

struct chess_timer*
update_timer(struct chess_timer* timer, struct chess* game)
{
	if (!timer)
		return NULL;

	if (!game)
		return timer;

	struct timespec t_cur;
	if (clock_gettime(CLOCK, &t_cur) != 0)
		return NULL;

	if (game->t_remaining_s > 0) {
		// offset timer by the remaining time
		timer->t_end = t_cur;
		timer->t_end.tv_sec += game->t_remaining_s;
	}

	// update time-per-move t_cur_move
	struct timespec t_cur_move_time =
			get_move_time(timer, game, PYRAMID_DISTRIBUTION);
	timer->t_cur_move_end.tv_sec  = t_cur.tv_sec + t_cur_move_time.tv_sec;
	timer->t_cur_move_end.tv_nsec = t_cur.tv_nsec + t_cur_move_time.tv_nsec;

	return timer;
}

double
get_remaining_move_time(struct chess_timer* timer)
{
	struct timespec t_cur;
	if (clock_gettime(CLOCK, &t_cur) != 0)
		return 0;

	// clang-format off
	struct timespec t_remaining = {
		timer->t_cur_move_end.tv_sec - t_cur.tv_sec,
		timer->t_cur_move_end.tv_nsec - t_cur.tv_nsec
	};
	// clang-format on

	// TODO(Aurel): Verify 1e9 is actually correct to transform seconds into
	// nanoseconds.
	return t_remaining.tv_sec + t_remaining.tv_nsec * 1e-9;
}
