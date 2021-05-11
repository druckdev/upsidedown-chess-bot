#ifndef TIMER_H
#define TIMER_H

#include <time.h>

struct chess_timer {
	struct timespec t_end, t_cur_move_start, t_cur_move;
};

enum t_move_distribution_function {
	UNIFORM_DISTRIBUTION,
};

#include "chess.h"

/**
 * Constructs the timer object for a chess game.
 */
struct chess_timer* start_timer(long t_total);

/**
 * Call this function once, when the game server tells us it's this bots time to
 * move.
 *
 * NOTE(Aurel): Update `(struct chess).t_remaining_ns` with the value the server
 * sends, before calling this function!
 */
struct chess_timer* update_timer(struct chess_timer* timer, struct chess* game);

/**
 * Returns the remaining time for a specific move.
 */
long get_remaining_move_time(struct chess_timer* timer);

#endif /* TIMER_H */
