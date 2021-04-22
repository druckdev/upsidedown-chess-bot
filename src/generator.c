#include <assert.h>

#include "board.h"
#include "chess.h"
#include "generator.h"

struct list*
generate_moves(struct chess* game)
{
	// TODO(Aurel): Stub. Fill this with code.
	assert(("Not implemented yet", 0 != 0));
}

struct list*
generate_moves_queen(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_king(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_rook(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_knight(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_pawn(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_bishop(struct PIECE* board[], enum POS pos)
{
}
