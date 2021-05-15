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

struct negamax_return negamax(struct chess* game, size_t depth);

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

struct negamax_return
negamax(struct chess* game, size_t depth)
{
	// End of tree to calculate
	if (!depth)
		return (struct negamax_return){ -1 * game->moving * rate_board(game),
			                            NULL, UNDEFINED, 0 };

	struct list* moves = generate_moves(game, true, false);

	// Check mate
	if (!list_count(moves)) {
		list_free(moves);
		return (struct negamax_return){ -1 * game->moving * rate_board(game),
			                            NULL, -1 * game->moving, depth + 1 };
	}

	game->moving *= -1;
	struct negamax_return best = { INT_MIN + 1, NULL, UNDEFINED, 0 };

	while (list_count(moves)) {
		struct move* move = list_pop(moves);
		struct PIECE old  = game->board[move->target];

		do_move(game->board, move);
		struct negamax_return ret = negamax(game, depth - 1);
		undo_move(game->board, move, old);

		if (ret.mate_for == -1 * game->moving) {
			// We can set the opponent mate with a move down the tree.

			if (ret.mate_depth == depth) {
				// This moves sets opponent mate
				free(best.move);
				free(ret.move);
				list_free(moves);

				ret.move = move;
				return ret;
			} else if (best.mate_for != -1 * game->moving ||
			           best.mate_depth < ret.mate_depth) {
				// A move down the tree can set the opponent mate and we
				// currently do not have a better mate move
				free(best.move);

				best      = ret;
				best.move = move;
			} else {
				free(move);
			}
		} else if (ret.mate_for == game->moving) {
			// Opponent has somewhere down the tree the possibility to set us
			// mate.
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
			// Move can lead to better rating or can save us from checkmate
			free(best.move);

			best      = ret;
			best.move = move;
		} else {
			free(move);
		}
		free(ret.move);
	}
	list_free(moves);

	game->moving *= -1;
	best.val *= -1;

	return best;
}
