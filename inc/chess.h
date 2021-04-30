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

// clang-format off
static enum PIECE DEFAULT_BOARD[64] = {
	       ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,
	       PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN,

	[48] = PAWN, PAWN,   PAWN,   PAWN,  PAWN, PAWN,   PAWN,   PAWN,
	       ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
};
// clang-format on

int get_piece_value(enum PIECE piece);
struct move opponent_move();
struct chess init_chess();
void run_chess(struct chess* game);

#endif /* CHESS_H */
