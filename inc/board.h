#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "chess.h"
#include "move.h"

#define WHITE_TO_BLACK_OFF ('a' - 'A')

struct piece do_move(struct chess* game, struct move* move);
void undo_move(struct chess* game, struct move* move, struct piece old);

enum game_phase get_game_phase(struct chess* game);
char* pos_to_str(enum pos pos, char* str);
char piece_e_to_chr(enum piece_type piece);
char piece_to_chr(struct piece piece);
struct piece chr_to_piece(char fen_piece);
void fen_to_chess(char* fen, struct chess* game);
bool* are_attacked(struct move_list* moves, bool* targets);

void fprint_board(FILE* stream, struct piece board[], struct move_list* moves);

struct piece* board_cpy(struct piece* board);
bool is_same_board(struct piece* board_1, struct piece* board_2);

#endif /* BOARD_H */
