#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "chess.h"
#include "move.h"
struct ht_entry {
	struct list* moves;
	size_t rating;
	size_t depth;
	size_t board_hash;
	enum color moving;
	struct piece* board;
	struct ht_entry* next;
};

/**
 * @arg size should always be a power of 2!
 */
struct ht* init_ht(struct ht* ht, size_t size);
void free_ht(struct ht* ht);

struct ht_entry* ht_update_entry(struct ht* ht, struct piece* board,
                                 enum color moving, struct list* moves,
                                 size_t rating, size_t depth);

struct ht_entry* ht_get_entry(struct ht* ht, struct piece* board,
                              enum color moving);

#endif /* HASHTABLE_H */
