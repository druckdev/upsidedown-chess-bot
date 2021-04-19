#include <assert.h>

#include "board.h"
#include "chess.h"

struct list {};
struct move {};

#define bool int

struct list
generate_moves(struct chess* game)
{
    // TODO(Aurel): Stub. Fill this with code.
    assert(("Not implemented yet", 0 != 0));
}

struct list
evaluate_moves(struct chess* game, struct list moves)
{
    // TODO(Aurel): Stub. Fill this with code.
    assert(("Not implemented yet", 0 != 0));
}

struct move
choose_move(struct chess* game, struct list moves_sorted)
{
    // TODO(Aurel): Stub. Fill this with code.
    assert(("Not implemented yet", 0 != 0));
}

bool
execute_move(struct chess* game, struct move move)
{
    // TODO(Aurel): Stub. Fill this with code.
    assert(("Not implemented yet", 0 != 0));
}


void
run_chess(struct chess* game)
{
    while (!game->checkmate) {
        struct list moves = generate_moves(game);
        struct list moves_sorted = evaluate_moves(game, moves);

    }
}
