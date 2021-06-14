#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "chess.h"
#include "move.h"
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

/**
 * @arg size should always be a power of 2!
 */
struct ht* init_ht(struct ht* ht, size_t size);
void free_ht(struct ht* ht);

struct ht_entry* ht_update_entry(struct ht* ht, struct piece* board, enum color moving,
#ifdef DEBUG_NEGAMAX_USE_LIST
                                 struct move_list* moves
#else  /* DEBUG_NEGAMAX_USE_LIST */
                                 struct move* move
#endif /* DEBUG_NEGAMAX_USE_LIST */
                                 ,
                                 size_t rating, size_t depth);

struct ht_entry* ht_get_entry(struct ht* ht, struct piece* board, enum color moving);

#endif /* HASHTABLE_H */
