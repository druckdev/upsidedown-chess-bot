#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
#include "generator.h"
#include "timer.h"
#include "types.h"

#define MAX_NEGAMAX_DEPTH 3

struct negamax_return {
	int val;
	struct move* move;
	enum COLOR mate_for;
	size_t mate_depth;
};

int
rate_board(struct chess* chess)
{
	int rating = 0;
	for (size_t i = 0; i < 64; ++i) {
		struct PIECE p = chess->board[i];
		rating += p.color * PIECE_VALUES[p.type];
	}
	chess->rating = rating;

	return rating;
}

struct negamax_return
negamax(struct chess* game, size_t depth)
{
	// max depth reached
	if (!depth)
		return (struct negamax_return){ -game->moving * rate_board(game), NULL,
			                            UNDEFINED, 0 };

	struct list* moves = generate_moves(game, true, false);

	// draw by stalemate - terminal node in tree
	// NOTE: If the list is empty because of a checkmate move, we will recognize
	//       that over move->is_checkmate and override ret.mate_for.
	if (!list_count(moves)) {
		list_free(moves);
		return (struct negamax_return){ -game->moving * rate_board(game), NULL,
			                            UNDEFINED, depth };
	}

	game->moving *= -1;
	struct negamax_return best = { INT_MIN + 1, NULL, UNDEFINED, 0 };

	// iterate over all moves (child nodes) to find best move
	while (list_count(moves)) {
		struct move* move = list_pop(moves);
		struct PIECE old  = game->board[move->target];

		// execute move and see what happens down the tree - dfs
		do_move(game->board, move);
		struct negamax_return ret = negamax(game, depth - 1);
		undo_move(game->board, move, old);

		if (move->is_checkmate) {
			ret.mate_for   = -game->moving;
			ret.mate_depth = depth;
		}

		/*
		 * NOTE(Aurel): game->moving is the opponent.
		 *
		 * Four cases:
		 *	1. The subtree will end in the opponent's king in checkmate.
		 *	2. The subtree will end in my king in checkmate.
		 *		- As we never actually reach the checkmate its checked
		 *		  differently and thus needs to be checked for here as well.
		 *	3. The subtree simply leads to a better score.
		 *	4. The subtree does not improve the current best move.
		 */
		if (ret.mate_for == -game->moving) {
			// we will checkmate the opponent

			if (ret.mate_depth == depth) {
				// current move checkmates the opponent - this is the best move
				// possible - return
				free(best.move);
				free(ret.move);

				best      = ret;
				best.move = move;
				break;
			}

			if (best.mate_for != -game->moving ||
			    best.mate_depth < ret.mate_depth) {
				// Either best_move is not checkmating the opponent or best_move is
				// deeper down the tree.
				free(best.move);

				best      = ret;
				best.move = move;
			} else {
				free(move);
			}
		} else if (ret.mate_for == game->moving) {
			// the opponent will checkmate me
			if (best.val == INT_MIN + 1 || (best.mate_for == game->moving &&
			                                ret.mate_depth < best.mate_depth)) {
				// We currently have no other move or best_move also checkmates
				// me, but in less steps.
				free(best.move);

				best      = ret;
				best.move = move;
			} else {
				free(move);
			}
		} else if (ret.val > best.val || best.mate_for == game->moving) {
			// move leads to a better rating or can save me from checkmate
			free(best.move);

			best      = ret;
			best.move = move;
		} else {
			// no better move found
			free(move);
		}
		free(ret.move);
	}
	list_free(moves);

	game->moving *= -1;
	best.val *= -1;

	return best;
}

struct move*
choose_move(struct chess* game, struct chess_timer* timer)
{
	struct move* best = NULL;

	struct timespec t_prev_move = { 0 };

	/*
	 * TODO(Aurel): Think about the calculation for the time a little more.
	 * Currently the remaining move time must be larger than 3 times the time
	 * the previous move's calculations took.
	 */
	size_t i = 1;
	while (true) {
		if (i > MAX_NEGAMAX_DEPTH)
			break;

		double t_remaining = get_remaining_move_time(timer);
		double min_t_remaining =
				3 * (t_prev_move.tv_sec + t_prev_move.tv_nsec * 1e-9);
		if (t_remaining < min_t_remaining)
			break;

		// take beginning time
		struct timespec t_beg;
		if (clock_gettime(CLOCK_MONOTONIC, &t_beg))
			return NULL;

		struct negamax_return ret = negamax(game, i);
		if (!ret.move)
			return NULL;

		if (best)
			free(best);

		best = ret.move;
		if (ret.mate_depth == i) {
			// ret.move leads to checkmate
			game->checkmate = true;
			break;
		}
		struct timespec t_end;
		if (clock_gettime(CLOCK_MONOTONIC, &t_end))
			return NULL;

		t_prev_move = t_end;
		t_prev_move.tv_sec -= t_beg.tv_sec;
		t_prev_move.tv_nsec -= t_beg.tv_nsec;

		++i;
	}
	return best;
}
