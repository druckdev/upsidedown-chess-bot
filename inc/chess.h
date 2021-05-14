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
	MIN=-1,
	H1, G1, F1, E1, D1, C1, B1, A1,
	H2, G2, F2, E2, D2, C2, B2, A2,
	H3, G3, F3, E3, D3, C3, B3, A3,
	H4, G4, F4, E4, D4, C4, B4, A4,
	H5, G5, F5, E5, D5, C5, B5, A5,
	H6, G6, F6, E6, D6, C6, B6, A6,
	H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8,
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

typedef unsigned long long U64; // TODO(luis): check portability, keywords : LL suffix, int64_t


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
