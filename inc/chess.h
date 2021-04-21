#ifndef CHESS_H
#define CHESS_H

#include <stdint.h>

#define WIDTH 8
#define HEIGHT 8

enum PIECE_E { PAWN = 1, BISHOP = 2, KNIGHT = 3, ROOK = 4, QUEEN = 5, KING = 6 };
// TODO(Aurel): Check if theses values are correct.

/*
 * TODO: These are just stubs to get a general idea of what we need.
 * Change and move them as u wish.
 */
enum COLOR { WHITE, BLACK };

struct PIECE {
	enum PIECE_E type;
	enum COLOR color;
};

struct chess {
	struct PIECE board[64];
	enum COLOR moving;
	uint32_t checkmate;
};

struct move {
};
struct chess init_chess();

void run_chess();

int get_piece_value(enum PIECE_E piece);

#endif /* CHESS_H */
