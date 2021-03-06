#ifndef TIMER_H
#define TIMER_H

#include <time.h>

struct chess_timer {
	struct timespec t_end, t_cur_move_end;
};

enum t_move_distribution_function {
	UNIFORM_DISTRIBUTION,
	PYRAMID_DISTRIBUTION
};

// NOTE(Aurel): This needs to included after the declaration of the struct.
#include "chess.h"

/**
 * Constructs the timer object for a chess game.
 */
struct chess_timer* start_timer(float t_total_s);

/**
 * Call this function once, when the game server tells us it's this bots time to
 * move.
 * Set game->t_remaining_ns to a value <= 0 to skip updating t_end value.  This
 * is useful for testing, when the game server does not yet send a value, i.e.
 * because its replaced by a simpler script, for testing purposes.
 *
 * NOTE(Aurel): Update `(struct chess).t_remaining_ns` with the value the server
 * sends, before calling this function!
 */
struct chess_timer* update_timer(struct chess_timer* timer, struct chess* game);

/**
 * Returns the remaining time for a specific move.
 */
double get_remaining_move_time(struct chess_timer* timer);

#endif /* TIMER_H */
