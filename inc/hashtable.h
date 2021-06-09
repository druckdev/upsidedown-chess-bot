#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdbool.h>

#include "move.h"

struct ht_entry {
	bool used;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct move_list* moves;
#else /* DEBUG_NEGAMAX_USE_LIST */
	struct move* move;
#endif /* DEBUG_NEGAMAX_USE_LIST */
	int depth;
	int board_hash;
};

struct ht_entry*
ht_update_move(struct ht_entry* ht, size_t size, struct piece* board,
#ifdef DEBUG_NEGAMAX_USE_LIST
		struct move_list* moves
#else /* DEBUG_NEGAMAX_USE_LIST */
		struct move* move
#endif /* DEBUG_NEGAMAX_USE_LIST */
		, int depth);

#ifdef DEBUG_NEGAMAX_USE_LIST
struct move_list*
#else
struct move*
#endif
ht_get_move(struct ht_entry* ht, size_t size, struct piece* board);

#endif /* HASHTABLE_H */
