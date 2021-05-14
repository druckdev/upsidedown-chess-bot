#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "types.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

void fen_to_game(char* fen, struct chess* game);
void print_bitboard(U64 board);
void print_board(struct board* board);
bool is_set_at(U64 bitboard, enum POS pos);

#endif /* BOARD_H */
