#ifndef CHESS_H
#define CHESS_H

#include <stdint.h>

enum PIECE { PAWN = 1, BISHOP = 2, KNIGHT = 3, ROOK = 4, QUEEN = 5, KING = 6 };
int PIECE_VALUES[] = { 0, 1, 4, 4, 5, 9, 1000000 };


/*
 * TODO: These are just stubs to get a general idea of what we need.
 * Change and move them as u wish.
 */
enum COLOR { WHITE, BLACK };
struct chess {
	enum PIECE board[64];
	enum COLOR moving;
	uint32_t checkmate;
	int rating;
};

struct chess init_chess();

void run_chess();

int get_piece_value(enum PIECE piece);

#endif /* CHESS_H */
