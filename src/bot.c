#include <assert.h>
#include <sys/types.h>

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
rate_board (struct chess *chess, struct move move)
{
	int rating = 0;
	for (size_t i = 0; i < sizeof(chess->board) / sizeof(*chess->board); ++i) {
		rating += PIECE_VALUES[chess->board[i]];
	}
	chess->rating = rating;

	return rating;
}

struct move
choose_move(struct chess* game, struct list moves_sorted)
{
	struct move* move = (struct move*)moves_sorted.first->object;
	// TODO: return invalid move
	// if (move) {
	return *move;
	// } else {

	// }
}
