#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "types.h"

struct negamax_return {
	int val;
	struct move* move;
};

int rate_board(struct chess* chess);
struct move* choose_move(struct chess* game, struct list* moves);
struct negamax_return negamax(struct chess* game, size_t depth);

#endif /* BOT_H */
