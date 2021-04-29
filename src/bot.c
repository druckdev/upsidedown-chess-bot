#include <assert.h>

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
