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
#include "hashtable.h"
#include "move.h"
#include "param_config.h"
#include "pst.h"
#include "timer.h"

size_t MAX_NEGAMAX_DEPTH = 99;

struct negamax_return {
	int val;
	struct move_list* moves;
};

int
rate_board(struct chess* chess)
{
	int rating = 0;
	for (size_t i = 0; i < 64; ++i) {
		struct piece p = chess->board[i];
		rating += p.color * PIECE_VALUES[p.type];
	}
	chess->rating = rating;

	return rating;
}

void
rate_move_list(struct chess* game, struct move_list* list)
{
	if (!list)
		return;

	struct move_list_elem* cur = move_list_get_first(list);
	while (cur) {
		struct move* move = cur->move;
		move->rating      = rate_move(game, move);

		cur = move_list_get_next(cur);
	}
}

/*
 * Currently implemented and activated:
 * - Principle Variation Search
 * - Piece Square Tables
 * - Transposition Tables
 */
struct negamax_return
negamax(struct chess* game, size_t depth, int a, int b)
{
	// max depth reached
	if (!depth)
		return (struct negamax_return){ 0, NULL };

	// check hash map if best move has already been calculated
	struct ht_entry* entry =
			ht_get_entry(&game->trans_table, game->board, game->moving);
	if (entry && entry->depth >= depth) {
		struct move_list* ret_moves = malloc(sizeof(*ret_moves));
		move_list_cpy(ret_moves, entry->moves);
		return (struct negamax_return){ entry->rating, ret_moves };
	}

	struct move_list* moves = generate_moves(game, true, false);

	// draw by stalemate - terminal node in tree
	if (!move_list_count(moves)) {
		move_list_free(moves);
		return (struct negamax_return){ 0, NULL };
	}

	rate_move_list(game, moves);
	move_list_sort(moves);
	game->moving *= -1;
	struct negamax_return best = { INT_MIN + 1, NULL };
	bool b_search_pv = true;

	/*
	 * This factor should increase the rating of moves higher up the tree,
	 * hopefully making the AI choose good moves first instead of at a later
	 * stage.
	 */
	size_t val_depth_factor = ((depth * depth) >> 3) + 1;

	while (move_list_count(moves)) {
		struct move* move = move_list_pop(moves);

		struct negamax_return ret;
		if (move->is_checkmate)
			/*
			 * If we know it will checkmate, there are no more moves left for
			 * the enemy to do and thus we already know what ret should look
			 * like. This saves at least one iteration over the board looking
			 * for valid moves.
			 */
			ret = (struct negamax_return){ 0, NULL };
		else {
			// execute move and see what happens down the tree - dfs
			struct piece old = do_move(game, move);

			if (b_search_pv) {
				ret = negamax(game, depth - 1, -b, -a);
			} else {
				ret = negamax(game, depth - 1, -a - 1, -a);
				if (-ret.val > a)
					ret = negamax(game, depth - 1, -b, -a); // re-search
			}
			undo_move(game, move, old);
		}

		ret.val *= -1;
		// include this moves rating in the score
		ret.val += val_depth_factor * move->rating;

		// replace the current best move, if move guarantees a better score.
		if (ret.val > best.val) {
			move_list_free(best.moves);
			best       = ret;
			best.moves = move_list_push(best.moves, move);
		} else {
			move_list_free(ret.moves);
			free(move);
		}

		if (best.val >= b) {
			best.val = b;
			break;
		} else if (best.val > a) {
			a           = best.val;
			b_search_pv = false;
		}
	}
	move_list_free(moves);
	game->moving *= -1;

	// if this fails no entry is created - ignore that case
	struct move_list* tp_moves = malloc(sizeof(*tp_moves));
	move_list_cpy(tp_moves, best.moves);
	struct ht_entry* ret =
			ht_update_entry(&game->trans_table, game->board, game->moving,
	                        tp_moves, best.val, depth);
	if (!ret || ret->moves != tp_moves)
		move_list_free(tp_moves);

	return best;
}

struct move*
choose_move(struct chess* game, struct chess_timer* timer)
{
	struct timespec t_prev_move = { 0 };
	struct move* best           = NULL;

	/*
	 * TODO(Aurel): Think about the calculation for the time a little more.
	 * Currently the remaining move time must be larger than 3 times the time
	 * the previous move's calculations took.
	 */
	for (size_t i = 1; i <= MAX_NEGAMAX_DEPTH; ++i) {
		// take beginning time
		struct timespec t_beg;
		if (clock_gettime(CLOCK_MONOTONIC, &t_beg))
			return NULL;

		free(best);

		struct negamax_return ret = negamax(game, i, INT_MIN + 1, INT_MAX);

		if (!ret.moves)
			return NULL;

		best = move_list_pop(ret.moves);
		move_list_free(ret.moves);
		if (!best)
			return NULL;

		if (best->is_checkmate) {
			// ret.move leads to checkmate
			game->checkmate = true;
			break;
		}

		// update the clock
		struct timespec t_end;
		if (clock_gettime(CLOCK_MONOTONIC, &t_end))
			return NULL;

		t_prev_move = t_end;
		t_prev_move.tv_sec -= t_beg.tv_sec;
		t_prev_move.tv_nsec -= t_beg.tv_nsec;

		double t_remaining = get_remaining_move_time(timer);
		double min_t_remaining =
				config.remaining_time_factor *
				(t_prev_move.tv_sec + t_prev_move.tv_nsec * 1e-9);
		if (t_remaining < min_t_remaining)
			break;
	}
	return best;
}
