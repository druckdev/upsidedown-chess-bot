#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include <stdint.h>

#define WIDTH 8
#define HEIGHT 8

#define DEFAULT_BOARD "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr"


enum COLOR { WHITE, BLACK };

// clang-format off
enum POS {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    MAX
};
// clang-format on

enum PIECE_E {
	EMPTY  = 0,
	PAWN   = 1,
	BISHOP = 2,
	KNIGHT = 3,
	ROOK   = 4,
	QUEEN  = 5,
	KING   = 6
};
static int PIECE_VALUES[] = { 0, 1, 4, 4, 5, 9, 1000000 };

struct PIECE {
	enum PIECE_E type;
	enum COLOR color;
};
static struct PIECE empty_piece = { EMPTY, WHITE };

typedef unsigned long long U64;


// bitboards to represent an entire board
// lsb represents A1, msb H8
struct board {
	U64 white_pieces;
	U64 black_pieces;
	U64 knights;
	U64 pawns;
	U64 bishops;
	U64 kings;
	U64 queens;
	U64 rooks;
};

struct chess {
	struct board board;
	enum COLOR moving;
	uint32_t checkmate;
	int rating;
};

// holds precomputed move masks for every piece, for every position
struct move_masks {
	U64 knights[64];
	U64 pawns[64];
	U64 bishops[64];
	U64 kings[64];
	U64 queens[64];
	U64 rooks[64];
};

struct move {
	enum POS start, target;
	bool hit;
	struct PIECE promotes_to;
};

int get_piece_value(enum PIECE_E piece);
struct move* opponent_move(struct move*);
struct chess init_chess(enum COLOR c);
void run_chess(struct chess* game);

#endif /* CHESS_H */
