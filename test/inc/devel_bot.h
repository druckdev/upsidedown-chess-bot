#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "hashtable.h"
#include "move.h"
#include "timer.h"

struct negamax_return {
	int val;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct move_list* moves;
#else
	struct move* move;
#endif
};

int rate_board(struct chess* chess);
struct negamax_return negamax(struct chess* game, struct ht* ht, size_t depth,
                              int a, int b);
struct move* choose_move(struct chess* game, struct chess_timer* timer);

#endif /* BOT_H */
