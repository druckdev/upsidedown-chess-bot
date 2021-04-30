#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "types.h"

bool execute_move(struct chess* game, struct move move);

char* pos_to_str(enum POS pos, char* str);

void print_board(struct PIECE board[], struct list* moves);

void board_from_fen(char* fen, struct PIECE board[]);

#endif /* BOARD_H */
