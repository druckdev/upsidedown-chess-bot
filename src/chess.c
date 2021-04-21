#include <assert.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "bot.h"

uint32_t value[] = {0,1,4,4,5,9,1000000};

int
get_piece_value(enum PIECE piece)
{
    return value[piece];
}

void
run_chess(struct chess* game)
{
    while (!game->checkmate) {
        struct list moves = generate_moves(game);
        struct move move = choose_move(game, moves);

    }
}
