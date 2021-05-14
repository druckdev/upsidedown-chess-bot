#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "types.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

void fen_to_game(char* fen, struct chess* game);
void print_bitboard(U64 board);

#endif /* BOARD_H */
