#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "chess.h"
#include "types.h"

bool execute_move(struct chess* game, struct move move);

char* pos_to_str(enum POS pos);

void print_board(struct PIECE board[]);

#endif /* BOARD_H */
