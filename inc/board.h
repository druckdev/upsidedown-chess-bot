#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "move.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

struct PIECE do_move(struct PIECE* board, struct move* move);
void undo_move(struct PIECE* board, struct move* move, struct PIECE old);

char* pos_to_str(enum POS pos, char* str);
char piece_e_to_chr(enum PIECE_E piece);
char piece_to_chr(struct PIECE piece);
struct PIECE chr_to_piece(char fen_piece);
void fen_to_chess(char* fen, struct chess* game);
bool* are_attacked(struct move_list* moves, bool* targets);
void print_board(struct PIECE board[], struct move_list* moves);

#endif /* BOARD_H */
