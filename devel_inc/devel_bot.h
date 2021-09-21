#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "hashtable.h"
#include "move.h"
#include "timer.h"

struct negamax_return {
	int val;
	struct move_list* moves;
};

int rate_board(struct chess* chess);
struct negamax_return negamax(struct chess* game, size_t depth, int a, int b);
struct move* choose_move(struct chess* game);

#endif /* BOT_H */
