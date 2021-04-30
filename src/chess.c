#include <assert.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
#include "generator.h"

int
get_piece_value(enum PIECE piece)
{
	return PIECE_VALUES[piece];
}

void
run_chess(struct chess* game)
{
	while (!game->checkmate) {
		struct list moves = generate_moves(game);
		struct move move  = choose_move(game, moves);
	}
}
