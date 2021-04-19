#ifndef CHESS_H
#define CHESS_H

#include "board.h"

/*
 * TODO: These are just stubs to get a general idea of what we need.
 * Change and move them as u wish.
 */
enum COLOR { WHITE, BLACK };
struct chess {
    enum PIECE board[64];
    enum COLOR moving;
    uint32_t checkmate;
};

struct chess init_chess();
void run_chess();

#endif /* CHESS_H */
