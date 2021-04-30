#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include <stdint.h>

#define WIDTH 8
#define HEIGHT 8

enum PIECE_E {
	EMPTY  = 0,
	PAWN   = 1,
	BISHOP = 2,
	KNIGHT = 3,
	ROOK   = 4,
	QUEEN  = 5,
	KING   = 6
};
int PIECE_VALUES[] = { 0, 1, 4, 4, 5, 9, 1000000 };

/*
 * TODO: These are just stubs to get a general idea of what we need.
 * Change and move them as u wish.
 */
enum COLOR { WHITE, BLACK };
// clang-format off
enum POS {
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
};
// clang-format on

struct PIECE {
	enum PIECE_E type;
	enum COLOR color;
};

struct chess {
	struct PIECE board[64];
	enum COLOR moving;
	uint32_t checkmate;
	int rating;
};

struct move {
	enum POS start, target;
	bool hit;
	enum PIECE_E promotes_to;
};
struct chess init_chess();

void run_chess();

int get_piece_value(enum PIECE_E piece);

#endif /* CHESS_H */
