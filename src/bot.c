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
#include "pst.h"
#include "timer.h"
#include "types.h"
#include "pst.h"

size_t MAX_NEGAMAX_DEPTH = 3;

struct negamax_return {
	int val;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct list* moves;
#else  /* DEBUG_NEGAMAX_USE_LIST */
	struct move* move;
#endif /* DEBUG_NEGAMAX_USE_LIST */
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

/**
 * Rates a move from the point of view of the moving player.
 *
 * NOTE(Aurel): Currently it only calculates the difference in piece-value on
 * the board the move would make.
 */
int
rate_move(struct chess* game, struct move* move)
{
	int rating = 0;

	if (move->hit) {
		// add the value of the hit piece to the rating
		struct PIECE to = game->board[move->target];
		rating += PIECE_VALUES[to.type];
	}

	if (move->is_checkmate)
		// checkmate is like hitting the king, so add the kings value to the
		// rating
		rating += PIECE_VALUES[KING];

	struct PIECE promotes_to = move->promotes_to;
	if (promotes_to.type) {
		// add the difference in value between the old and new piece to the
		// rating
		struct PIECE from = game->board[move->start];
		rating += PIECE_VALUES[promotes_to.type] - PIECE_VALUES[from.type];
	}

	rating += get_pst_diff(game, move, game->board[move->start].type);

	return rating;
}

void
register_prio(struct chess* game, struct list* list)
{
	if (!list)
		return;

	struct list_elem* cur = list_get_first(list);
	while (cur) {
		struct move* move = cur->object;
		cur->prio         = rate_move(game, move);

		cur = list_get_next(cur);
	}
}

struct negamax_return
negamax(struct chess* game, size_t depth, int a, int b)
{
	// max depth reached
	if (!depth)
		return (struct negamax_return){ 0, NULL };

	struct list* moves = generate_moves(game, true, false);

	// draw by stalemate - terminal node in tree
	// NOTE: If the list is empty because of a checkmate move, we will recognize
	//       that by checking move->is_checkmate later and overwrite
	//       ret.mate_for there.
	//       TODO(Aurel): This is not true anymore.
	if (!list_count(moves)) {
		list_free(moves);
		return (struct negamax_return){ 0, NULL };
	}

#ifdef ENABLE_ALPHA_BETA_CUTOFFS
	register_prio(game, moves);
	list_sort(moves);
#endif

	game->moving *= -1;
	struct negamax_return best = { INT_MIN + 1, NULL };

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

	while (list_count(moves)) {
		struct move* move = list_pop(moves);

		struct negamax_return ret;
		if (move->is_checkmate)
			// If we know it will checkmate, there are no more moves left for
			// the enemy to do and thus we already know what ret should look
			// like.
			ret = (struct negamax_return){ 0, NULL };
		else {
			// execute move and see what happens down the tree - dfs
			struct PIECE old = do_move(game, move);
			ret              = negamax(game, depth - 1, -b, -a);
			undo_move(game, move, old);
		}

#ifdef ENABLE_ALPHA_BETA_CUTOFFS
		// without ab-pruning this happens at the end of the function
		ret.val = -ret.val;
#endif /* ENABLE_ALPHA_BETA_CUTOFFS */

		// include this moves rating in the score
		int rating = val_depth_factor * rate_move(game, move);
		ret.val += rating;

		// replace the current best move, if move guarantees a better score.
		if (ret.val > best.val) {
#ifdef DEBUG_NEGAMAX_USE_LIST
			list_free(best.moves);
			best       = ret;
			best.moves = list_push(best.moves, move);
		} else {
			list_free(ret.moves);
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

#ifdef ENABLE_ALPHA_BETA_CUTOFFS
		if (best.val > a)
			a = best.val;
		if (a >= b)
			break;
#endif /* ENABLE_ALPHA_BETA_CUTOFFS */
	}
	list_free(moves);

	game->moving *= -1;
#ifndef ENABLE_ALPHA_BETA_CUTOFFS
	// using ab-pruning this needs to happen earlier
	best.val *= -1;
#endif /* ENABLE_ALPHA_BETA_CUTOFFS */

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

#ifdef DEBUG_PRINTS
		fprint_move_list(DEBUG_PRINT_STREAM, ret.moves);
#endif
		best = list_pop(ret.moves);
		list_free(ret.moves);
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
