#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
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
rate_board(struct chess* chess, struct move move)
{
	int rating = 0;
	for (size_t i = 0; i < sizeof(chess->board) / sizeof(*chess->board); ++i) {
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
