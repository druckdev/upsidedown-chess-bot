#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "move.h"

struct ht {
	struct ht_entry* table;
	size_t size;
};

struct ht_entry {
	bool used;
#ifdef DEBUG_NEGAMAX_USE_LIST
	struct move_list* moves;
#else  /* DEBUG_NEGAMAX_USE_LIST */
	struct move* move;
#endif /* DEBUG_NEGAMAX_USE_LIST */
	size_t rating;
	size_t depth;
	size_t board_hash;
};

struct ht* init_ht(struct ht* ht, size_t size);

struct ht_entry* ht_update_entry(struct ht* ht, struct piece* board,
#ifdef DEBUG_NEGAMAX_USE_LIST
                                 struct move_list* moves
#else  /* DEBUG_NEGAMAX_USE_LIST */
                                 struct move* move
#endif /* DEBUG_NEGAMAX_USE_LIST */
                                 ,
                                 size_t rating, int depth);

struct ht_entry* ht_get_entry(struct ht* ht, struct piece* board);

#endif /* HASHTABLE_H */
