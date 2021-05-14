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

/*
struct list
evaluate_moves(struct chess* game, struct list moves)
{
    // TODO(Aurel): Stub. Fill this with code.
    assert(("Not implemented yet", 0 != 0));
}
*/

int
rate_board(struct chess* chess)
{
	int rating = 0;
	for (size_t i = 0; i < 64; ++i) {
		rating += PIECE_VALUES[chess->board[i].type];
	}
	chess->rating = rating;

	return rating;
}

struct move*
choose_move(struct chess* game, struct list* moves)
{
	if (!moves)
		return NULL;

	struct list_elem* cur = moves->first;
	if (!cur)
		return NULL;

	// Choose random move
	for (size_t i = 0; i < rand() % list_count(moves); ++i) {
		if (cur->next)
			cur = cur->next;
		else
			return (struct move*)cur->object;
	}
	return (struct move*)cur->object;
}

struct negamax_return
negamax(struct chess* game, size_t depth)
{
	if (!depth) /* or checkmate */
		return (struct negamax_return){ game->moving * rate_board(game), NULL };

	struct list* moves = generate_moves(game, true, false);

	game->moving *= -1;
	int val = INT_MIN + 1;
	struct move* best_move = NULL;

	while (list_count(moves)) {
		struct move* move = list_pop(moves);
		struct PIECE old  = game->board[move->target];
		do_move(game->board, move);

		struct negamax_return ret = negamax(game, depth - 1);

		undo_move(game->board, move, old);

		if (ret.val > val) {
			free(best_move);
			best_move = move;
			val       = ret.val;
		} else {
			free(move);
		}
		free(ret.move);

	}
	list_free(moves);

	game->moving *= -1;

	return (struct negamax_return){ -val, best_move };
}
