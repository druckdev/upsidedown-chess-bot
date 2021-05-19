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

// TODO(Aurel): All these functions assume the game server sends remaining time
// in nano seconds.
struct chess_timer*
start_timer(long t_total_s)
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
	// clang-format off
	struct timespec t_move = {
		t_remaining.tv_sec / remaining_moves,
		t_remaining.tv_nsec / remaining_moves
	};
	// clang-format on

	return t_move;
}

/* \ Different functions to calculate the time the current move has to spend. */

struct timespec
get_move_time(struct chess_timer* timer, struct chess* game,
              enum t_move_distribution_function method)
{
	switch (method) {
	case UNIFORM_DISTRIBUTION:
		return uniform_distribution(timer, game);
	default:
		assert(("No timer function is being used", false));
	}
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

	timer->t_cur_move_start = t_cur;

	if (game->t_remaining_s > 0) {
		// offset timer by the remaining time
		timer->t_end = t_cur;
		timer->t_end.tv_sec += game->t_remaining_s;
	}

	// update time-per-move t_cur_move
	struct timespec t_cur_move_time = get_move_time(timer, game, UNIFORM_DISTRIBUTION);
	timer->t_cur_move_end.tv_sec = t_cur.tv_sec + t_cur_move_time.tv_sec;
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
