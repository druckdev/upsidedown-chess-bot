#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "types.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

bool execute_move(struct PIECE* board, struct move* move);
char* pos_to_str(enum POS pos, char* str);
char piece_e_to_chr(enum PIECE_E piece);
char piece_to_chr(struct PIECE piece);
struct PIECE chr_to_piece(char fen_piece);
void fen_to_board(char* fen, struct PIECE board[]);
bool* are_attacked(struct list* moves, bool* targets);
void print_board(struct PIECE board[], struct list* moves);

#endif /* BOARD_H */
