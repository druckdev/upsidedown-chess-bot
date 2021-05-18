#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
#include "generator.h"
#include "types.h"

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
		return (struct negamax_return){ -game->moving * rate_board(game),
			                            NULL, UNDEFINED, 0 };

	struct list* moves = generate_moves(game, true, false);

	// check checkmate - terminal node in tree
	if (!list_count(moves)) {
		list_free(moves);
		return (struct negamax_return){ -game->moving * rate_board(game),
			                            NULL, -game->moving, depth + 1 };
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

		/*
		 * NOTE(Aurel): game->moving is the opponent.
		 *
		 * Four cases:
		 *	1. The subtree will end in the opponent's king in checkmate.
		 *	2. The subtree will end in my king in checkmate.
		 *		- as we never actually reach the checkmate we need to check it
		 *		  differently.
		 *	3. The subtree simply leads to a better score.
		 *	4. The subtree does not improve the current best move.
		 */
		if (ret.mate_for == -game->moving) {
			// we will checkmate the opponent

			if (ret.mate_depth == depth) {
				// current move checkmates the opponent
				free(best.move);
				free(ret.move);
				list_free(moves);

				ret.move = move;
				return ret;
			}

			if (best.mate_for != -game->moving ||
			           best.mate_depth < ret.mate_depth) {
				// Either best_move is not checkamting the opponent or best_move is
				// deeper down the tree.
				free(best.move);

				best      = ret;
				best.move = move;
			} else {
				free(move);
			}
		} else if (ret.mate_for == game->moving) {
			// the opponent will checkmate me
			if (best.val == INT_MIN + 1 || (ret.mate_for == game->moving &&
			                                ret.mate_depth > best.mate_depth)) {
				// We currently have no other move or the currently best move
				// can set us also mate but in less steps.
				free(best.move);

				best      = ret;
				best.move = move;
			} else {
				free(move);
			}
		} else if (ret.val > best.val || best.mate_for == game->moving) {
			// move leads to a better rating or can save us from checkmate
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
choose_move(struct chess* game)
{
	struct move* best = NULL;
	for (size_t i = 1; i < 5 /* TODO: check time */; i++) {
		struct negamax_return ret = negamax(game, i);
		if (!ret.move)
			return NULL;

		free(best);
		best = ret.move;
		if (ret.mate_depth == i) {
			// ret.move leads to checkmate
			game->checkmate = true;
			break;
		}
	}
	return best;
}
