#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "move.h"
#include "timer.h"

extern size_t MAX_NEGAMAX_DEPTH;

int rate_board(struct chess* chess);
struct move* choose_move(struct chess* game, struct chess_timer* timer);

#endif /* BOT_H */
