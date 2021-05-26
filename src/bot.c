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

size_t MAX_NEGAMAX_DEPTH = 3;

struct negamax_return {
	int val;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct list* moves;
#else
	struct move* move;
#endif
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

int
rate_move(struct PIECE* board, struct move* move)
{
	int rating = 0;

	if (move->hit) {
		struct PIECE to = board[move->target];
		rating -= to.color * PIECE_VALUES[to.type];
	}

	if (move->is_checkmate)
		rating += board[move->start].color * PIECE_VALUES[KING];

	struct PIECE promotes_to = move->promotes_to;
	if (promotes_to.type) {
		struct PIECE from = board[move->start];
		int diff = PIECE_VALUES[promotes_to.type] - PIECE_VALUES[from.type];
		rating += promotes_to.color * diff;
	}

	return rating;
}

void
register_prio(struct PIECE* board, struct list* list)
{
	if (!list)
		return;

	struct list_elem* cur = list_get_first(list);
	while (cur) {
		struct move* move = cur->object;
		cur->prio         = rate_move(board, move);

		cur = list_get_next(cur);
	}
}

struct negamax_return
negamax(struct chess* game, size_t depth, int a, int b)
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
			                            UNDEFINED, depth + 1 };
	}

#ifndef NO_ALPHA_BETA_CUTOFFS
	register_prio(game->board, moves);
	list_sort(moves);
#endif

	game->moving *= -1;
	struct negamax_return best = { INT_MIN + 1, NULL, UNDEFINED, 0 };

	// iterate over all moves (child nodes) to find best move
	while (list_count(moves)) {
		struct move* move = list_pop(moves);

		// execute move and see what happens down the tree - dfs
		struct PIECE old          = do_move(game->board, move);
		struct negamax_return ret = negamax(game, depth - 1, -b, -a);
		undo_move(game->board, move, old);

		// Set checkmate details if appropriate.
		if (move->is_checkmate) {
			ret.mate_for   = -game->moving;
			ret.mate_depth = depth;
		}

		/*
		 * NOTE(Aurel): game->moving is the opponent.
		 *
		 * In following cases do we want to overwrite our current best move with
		 * the new move:
		 *  1. The observed move leads directly to a checkmate.
		 *  2. The observed move leads potentially later to a checkmate and our
		 *     current best move does not as far as we know.
		 *  3. The observed move leads potentially later to a checkmate and does
		 *     it faster than the current best move.
		 *  4. The observed move leads potentially later to a checkmate in
		 *     equally many steps, but with a better rating for us.
		 *
		 *  5. The observed move leads to an opponents checkmate, but we
		 *     currently have no other move at all.
		 *  6. The observed move leads to an opponents checkmate but does it
		 *     slower than our current best move.
		 *  7. The observed move leads to an opponents checkmate in equally many
		 *     steps, but with a better rating for us.
		 *
		 *  8. The observed move leads to a stalemate, and the current best move
		 *     leads to a checkmate against us.
		 *  9. The observed move leads to a stalemate but does it slower than
		 *     our current best move.
		 * 10. The observed move leads to a stalemate in equally many steps, but
		 *     with a better rating for us.
		 *
		 * 11. The observed move leads to a better rating.
		 * 12. The observed move saves us from a checkmate or stalemate that out
		 *     current best move would lead to potentially.
		 */
		if (ret.mate_for == -game->moving) {
			// I will checkmate the opponent

			if (ret.mate_depth == depth) {
				// Current move checkmates the opponent
				// Freeing moves and setting it to null breaks the loop
				list_free(moves);
				moves = NULL;
				goto overwrite_best_move;
			}

			if (best.mate_for != -game->moving) {
				// Current best move is not a checkmate move
				goto overwrite_best_move;
			} else if (best.mate_depth < ret.mate_depth) {
				// Best move needs a longer path until checkmate
				goto overwrite_best_move;
			} else if (best.mate_depth == ret.mate_depth &&
			           ret.val > best.val) {
				// Both take equally many steps, but the rating is better making
				// it a potential better choice if the opponent does not behave
				// as we expect.
				goto overwrite_best_move;
			}
		} else if (ret.mate_for == game->moving) {
			// The opponent will checkmate me

			if (best.val == INT_MIN + 1) {
				// I currently have no other move and need to use this for now.
				goto overwrite_best_move;
			} else if (best.mate_for == game->moving) {
				// Current best move also checkmates me.

				if (ret.mate_depth < best.mate_depth) {
					// Current best move also checkmates me, but in less steps.
					goto overwrite_best_move;
				} else if (ret.mate_depth == best.mate_depth &&
				           ret.val > best.val) {
					// Both take equally many steps, but the rating is better
					// making it a potential better choice if we see an escape
					// with a deeper tree for example.
					goto overwrite_best_move;
				}
			}
		} else if (ret.mate_for == UNDEFINED && ret.mate_depth) {
			// Stalemate

			if (best.mate_for == game->moving) {
				// Current best move checkmates me
				goto overwrite_best_move;
			} else if (best.mate_for == UNDEFINED && best.mate_depth) {
				// Current best move also leads to stalemate

				if (ret.mate_depth < best.mate_depth) {
					// Current leads to stalemate in less steps
					goto overwrite_best_move;
				} else if (ret.mate_depth == best.mate_depth &&
				           ret.val > best.val) {
					// Both take equally many steps, but the rating is better
					// making it a potential better choice if we see an escape
					// with a deeper tree for example.
					goto overwrite_best_move;
				}
			}
		} else if (best.mate_for != -game->moving &&
		           (ret.val > best.val || best.mate_depth)) {
			// move leads to a better rating or can save me from checkmate or
			// the game from stalemate
			goto overwrite_best_move;
		}

		free(move);
#ifdef DEBUG_NEGAMAX_USE_LIST
		list_free(ret.moves);
#else
		free(ret.move);
#endif

		goto cutoffs;

overwrite_best_move:
#ifdef DEBUG_NEGAMAX_USE_LIST
		list_free(best.moves);
		best       = ret;
		best.moves = list_push(best.moves, move);
#else
		free(best.move);
		best      = ret;
		best.move = move;
		free(ret.move);
#endif

cutoffs:
#ifndef NO_ALPHA_BETA_CUTOFFS
		if (best.val > a)
			a = best.val;

		if (a >= b)
			break;
#endif
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
	for (size_t i = 1; i <= MAX_NEGAMAX_DEPTH; ++i) {
		double t_remaining = get_remaining_move_time(timer);
		double min_t_remaining =
				3 * (t_prev_move.tv_sec + t_prev_move.tv_nsec * 1e-9);
		if (t_remaining < min_t_remaining)
			break;

		// take beginning time
		struct timespec t_beg;
		if (clock_gettime(CLOCK_MONOTONIC, &t_beg))
			return NULL;

		free(best);

		struct negamax_return ret = negamax(game, i, INT_MIN + 1, INT_MAX);

#ifdef DEBUG_NEGAMAX_USE_LIST
		if (!ret.moves)
			return NULL;

		fprint_move_list(DEBUG_PRINT_STREAM, ret.moves);

		best = list_pop(ret.moves);
		list_free(ret.moves);
#else
		best = ret.move;
#endif
		if (!best)
			return NULL;

		fprintf(DEBUG_PRINT_STREAM, "depth: %lu\n", i);
		fprintf(DEBUG_PRINT_STREAM, "cur best move: ");
		fprint_move(DEBUG_PRINT_STREAM, best);
		fprintf(DEBUG_PRINT_STREAM, "\n");

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
	}
	return best;
}
