#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "timer.h"
#include "types.h"

struct negamax_return {
	int val;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct list* moves;
#else
	struct move* move;
#endif
};

int rate_board(struct chess* chess);
struct negamax_return negamax(struct chess* game, size_t depth, int a, int b);
struct move* choose_move(struct chess* game, struct chess_timer* timer);

#endif /* BOT_H */
