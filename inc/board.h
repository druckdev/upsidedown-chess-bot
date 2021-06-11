#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "move.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

struct piece do_move(struct piece* board, struct move* move);
void undo_move(struct piece* board, struct move* move, struct piece old);

char* pos_to_str(enum pos pos, char* str);
char piece_e_to_chr(enum piece_type piece);
char piece_to_chr(struct piece piece);
struct piece chr_to_piece(char fen_piece);
void fen_to_chess(char* fen, struct chess* game);
bool* are_attacked(struct move_list* moves, bool* targets);
void print_board(struct piece board[], struct move_list* moves);

#endif /* BOARD_H */
