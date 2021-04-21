#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "types.h"

struct move choose_move(struct chess* game, struct list moves_sorted);

#endif /* BOT_H */
