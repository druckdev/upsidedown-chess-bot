#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "types.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

struct board* fen_to_bitboard();

#endif /* BOARD_H */
