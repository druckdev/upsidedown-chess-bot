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
#include "pst.h"
#include "timer.h"

size_t MAX_NEGAMAX_DEPTH = 3;

struct negamax_return {
	int val;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct move_list* moves;
#else  /* DEBUG_NEGAMAX_USE_LIST */
	struct move* move;
#endif /* DEBUG_NEGAMAX_USE_LIST */
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

struct negamax_return
negamax(struct chess* game, struct ht* ht, size_t depth, int a, int b)
{
	// max depth reached
	if (!depth)
		return (struct negamax_return){ 0, NULL };

#ifdef ENABLE_TRANSPOSITION_TABLE
	// TODO(Aurel): check hash map if best move has already been calculated
	struct ht_entry* entry = ht_get_entry(ht, game->board);
	if (entry) {
		printf("Found transposition entry.\n");
		if (entry->depth >= depth) {
#ifdef DEBUG_NEGAMAX_USE_LIST
			return (struct negamax_return){ entry->rating, entry->moves };
#else
			return (struct negamax_return){ entry->rating, entry->move };
#endif
		}
	}
#endif /* ENABLE_TRANSPOSITION_TABLE */

	struct move_list* moves = generate_moves(game, true, false);

	// draw by stalemate - terminal node in tree
	// NOTE: If the list is empty because of a checkmate move, we will recognize
	//       that by checking move->is_checkmate later and overwrite
	//       ret.mate_for there.
	//       TODO(Aurel): This is not true anymore.
	if (!move_list_count(moves)) {
		move_list_free(moves);
		return (struct negamax_return){ 0, NULL };
	}

#ifndef VANILLA_MINIMAX
	rate_move_list(game, moves);
	move_list_sort(moves);
#endif

	game->moving *= -1;
	struct negamax_return best = { INT_MIN + 1, NULL };
#ifdef USE_PRINCIPAL_VARIATION_SEARCH
	bool b_search_pv = true;
#endif

	/*
	 * NOTE(Aurel): This factor should increase the rating of moves higher up
	 * the tree, hopefully making the AI choose good moves first instead of at a
	 * later stage.
	 * TODO(Aurel): Tinker around with the values.
	 * Factor:
	 *	- depth: difference between two levels is the same up and down the tree.
	 *		Does not work.
	 *	- depth^2: might be too steep a curve and cause other problems.
	 *
	 *  - Final thoughts: a * depth^2 + 1 , a = 1/(2^k)
	 *		((depth * depth) >> a) + 1;
	 */
	size_t val_depth_factor = ((depth * depth) >> 3) + 1;

	while (move_list_count(moves)) {
		struct move* move = move_list_pop(moves);

		struct negamax_return ret;
		if (move->is_checkmate)
			// If we know it will checkmate, there are no more moves left for
			// the enemy to do and thus we already know what ret should look
			// like. This saves at least one iteration over the board looking
			// for valid moves.
			ret = (struct negamax_return){ 0, NULL };
		else {
			// execute move and see what happens down the tree - dfs
			struct piece old = do_move(game, move);

#ifdef USE_PRINCIPAL_VARIATION_SEARCH
			if (b_search_pv) {
				ret = negamax(game, ht, depth - 1, -b, -a);
			} else {
				ret = negamax(game, ht, depth - 1, -a - 1, -a);
				if (-ret.val > a)
					ret = negamax(game, ht, depth - 1, -b, -a); // re-search
			}
#else
			ret = negamax(game, ht, depth - 1, -b, -a);
#endif

			undo_move(game, move, old);
		}

#ifndef VANILLA_MINIMAX
		// without ab-pruning this happens at the end of the function
		ret.val = -ret.val;
#else
		// the move has not yet been rated
		move->rating = rate_move(game, move);
#endif /* ENABLE_ALPHA_BETA_CUTOFFS */

		// include this moves rating in the score
		ret.val += val_depth_factor * move->rating;

		// replace the current best move, if move guarantees a better score.
		if (ret.val > best.val) {
#ifdef DEBUG_NEGAMAX_USE_LIST
			move_list_free(best.moves);
			best       = ret;
			best.moves = move_list_push(best.moves, move);
		} else {
			move_list_free(ret.moves);
#else  /* DEBUG_NEGAMAX_USE_LIST */
			free(best.move);
			best      = ret;
			best.move = move;
			free(ret.move);
		} else {
			free(ret.move);
#endif /* DEBUG_NEGAMAX_USE_LIST */
			free(move);
		}

#if defined(USE_PRINCIPAL_VARIATION_SEARCH)
		if (best.val >= b)
			break;
		if (best.val > a) {
			a           = best.val;
			b_search_pv = false;
		}
#elif defined(ENABLE_ALPHA_BETA_CUTOFFS)
		if (best.val > a)
			a = best.val;
		if (a >= b)
			break;
#endif /* ENABLE_ALPHA_BETA_CUTOFFS */
	}
	move_list_free(moves);

	game->moving *= -1;
#ifdef VANILLA_MINIMAX
	// using ab-pruning this needs to happen earlier
	best.val *= -1;
#endif /* ENABLE_ALPHA_BETA_CUTOFFS */

#ifdef ENABLE_TRANSPOSITION_TABLE
	// if this fails no entry is created - ignore that case
	ht_update_entry(ht, game->board, best.moves, best.val, depth);
#endif /* ENABLE_TRANSPOSITION_TABLE */

	return best;
}

struct move*
choose_move(struct chess* game, struct chess_timer* timer)
{
	struct timespec t_prev_move = { 0 };
	struct ht ht                = { 0 };
	if (!init_ht(&ht, 1024))
		return NULL;

	struct move* best = NULL;

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

		struct negamax_return ret = negamax(game, &ht, i, INT_MIN + 1, INT_MAX);

#ifdef DEBUG_NEGAMAX_USE_LIST
		if (!ret.moves)
			return NULL;

#ifdef DEBUG_PRINTS
		fprint_move_list(DEBUG_PRINT_STREAM, ret.moves);
#endif
		best = move_list_pop(ret.moves);
		move_list_free(ret.moves);
#else  /* DEBUG_NEGAMAX_USE_LIST */
		best = ret.move;
#endif /* DEBUG_NEGAMAX_USE_LIST */
		if (!best)
			return NULL;

#ifdef DEBUG_PRINTS
		fprintf(DEBUG_PRINT_STREAM, "cur best move: ");
		fprint_move(DEBUG_PRINT_STREAM, best);
		fprintf(DEBUG_PRINT_STREAM, "depth: %lu\n", i);
		fprintf(DEBUG_PRINT_STREAM, "value: %i\n", ret.val);
		fprintf(DEBUG_PRINT_STREAM, "\n");
#endif /* DEBUG_PRINTS */

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
	}
	return best;
}
