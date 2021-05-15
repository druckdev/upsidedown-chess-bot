#ifndef BOT_H
#define BOT_H

#include "chess.h"
#include "types.h"

int rate_board(struct chess* chess);
struct move* choose_move(struct chess* game);

#endif /* BOT_H */
