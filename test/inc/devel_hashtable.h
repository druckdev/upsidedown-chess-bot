#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>

#include "chess.h"
#include "move.h"
struct ht_entry {
	bool used;
	struct list* moves;
	size_t rating;
	size_t depth;
	size_t board_hash;
	struct piece* board;
};

ssize_t hash_board(size_t size, struct piece* board, enum color moving);

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
