#include <assert.h>
#include <string.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
#include "generator.h"

int
get_piece_value(enum PIECE_E piece)
{
	return PIECE_VALUES[piece];
}

struct chess
init_chess()
{
	struct chess chess = { 0 };
	memcpy(chess.board, DEFAULT_BOARD, sizeof(chess.board));
	chess.moving = WHITE;
	return chess;
}

void
run_chess(struct chess* game)
{
	while (!game->checkmate) {
		struct list* moves = generate_moves(game);
		struct move move   = choose_move(game, *moves);
	}
}
