#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define WIDTH 8
#define HEIGHT 8

#define MAX_MOVE_COUNT 50

#define EG_MOVE_COUNT_MAX 20
#define MG_MOVE_COUNT_MAX 40

#define EG_PIECE_COUNT_MIN 30
#define MG_PIECE_COUNT_MIN 11

#define DEFAULT_BOARD "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr"

enum PIECE_E {
	EMPTY  = 0,
	PAWN   = 1,
	BISHOP = 2,
	KNIGHT = 3,
	ROOK   = 4,
	QUEEN  = 5,
	KING   = 6
};
extern int PIECE_VALUES[];

/*
 * TODO: These are just stubs to get a general idea of what we need.
 * Change and move them as u wish.
 */
enum COLOR { BLACK = -1, UNDEFINED = 0, WHITE = +1 };
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
    MAX
};
// clang-format on

struct PIECE {
	enum PIECE_E type;
	enum COLOR color;
};
extern struct PIECE empty_piece;

enum game_phase { EARLY_GAME, MID_GAME, LATE_GAME };

struct chess {
	struct PIECE* board;
	enum COLOR moving;
	enum game_phase phase;
	uint32_t checkmate;
	int rating;
	long t_remaining_s;
	int max_moves, move_count;
	int piece_count;
};

struct move {
	enum POS start, target;
	bool hit, is_checkmate;
	struct PIECE promotes_to;
};

enum game_phase get_game_phase(struct chess* game);
int get_piece_value(enum PIECE_E piece);
struct move* opponent_move(struct move*);
struct chess init_chess(char color, float total_time_s, size_t max_moves);
void run_chess(struct chess* game);

void fprint_move(FILE* stream, struct move* move);

#endif /* CHESS_H */
