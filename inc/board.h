#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

enum PIECE { PAWN = 1, BISHOP = 2, KNIGHT = 3, ROOK = 4, QUEEN = 5, KING = 6 };
// TODO(Aurel): Check if theses values are correct.

enum POS {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
};

enum PIECE board[64];

char* pos_to_str(enum POS pos);

void print_board(enum PIECE board[]);

#endif /* BOARD_H */
