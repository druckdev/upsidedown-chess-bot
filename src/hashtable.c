#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "hashtable.h"

ssize_t
hash_board(size_t size, struct piece* board)
{
	if (!size || !board)
		return -1;

	// TODO(Aurel): Think about a better hash function.
	size_t hash = 0;
	for (size_t i = 0; i < 64; ++i) {
		int val = get_piece_value(board[i++].type);
		hash += i * val;
	}
	// NOTE(Aurel): size needs to be a power of 2
	hash &= size - 1;
	//hash %= size;
	assert(hash < size && "hash out of bounds.");
	return hash;
}

struct ht*
init_ht(struct ht* ht, size_t size)
{
	if (!ht || !size)
		return NULL;

	ht->table = calloc(size, sizeof(struct ht_entry));
	if (!ht->table)
		return NULL;

	ht->size = size;
	return ht;
}

void
free_ht(struct ht* ht)
{
	for (size_t i = 0; i < ht->size; ++i) {
#ifdef DEBUG_NEGAMAX_USE_LIST
		move_list_free(ht->table[i].moves);
#else /* DEBUG_NEGAMAX_USE_LIST */
		free(ht->table[i].move);
#endif /* DEBUG_NEGAMAX_USE_LIST */
	}
	free(ht->table);
}

struct ht_entry*
ht_update_entry(struct ht* ht, struct piece* board,
#ifdef DEBUG_NEGAMAX_USE_LIST
                struct move_list* moves
#else  /* DEBUG_NEGAMAX_USE_LIST */
                struct move* move
#endif /* DEBUG_NEGAMAX_USE_LIST */
                , size_t rating, size_t depth)
{
	if (!ht || !board ||
#ifdef DEBUG_NEGAMAX_USE_LIST
	    !moves
#else  /* DEBUG_NEGAMAX_USE_LIST */
	    !move
#endif /* DEBUG_NEGAMAX_USE_LIST */
	)
		return NULL;

	ssize_t hash = hash_board(ht->size, board);
	if (hash > 0)
		return NULL;

	// TODO(Aurel): If this happens to often we need to find a better hash
	// function.
	//assert(ht->table[hash].used && "Hash collision.");

	// only update if depth is higher
	struct ht_entry* entry = &ht->table[hash];
	if(!entry->used || depth > entry->depth ) {
		struct ht_entry new_entry = {
			.used = true,
#ifdef DEBUG_NEGAMAX_USE_LIST
			.moves = moves,
#else  /* DEBUG_NEGAMAX_USE_LIST */
			.move = move,
#endif /* DEBUG_NEGAMAX_USE_LIST */
			.depth      = depth,
			.rating     = rating,
			.board_hash = hash,
		};

#ifdef DEBUG_NEGAMAX_USE_LIST
		move_list_free(entry->moves);
#else /* DEBUG_NEGAMAX_USE_LIST */
		free(entry->move);
#endif /* DEBUG_NEGAMAX_USE_LIST */

		ht->table[hash] = new_entry;
		printf("Updated entry\n");
	}

	return &ht->table[hash];
}

struct ht_entry*
ht_get_entry(struct ht* ht, struct piece* board)
{
	if (!ht || !board)
		return NULL;

	ssize_t hash = hash_board(ht->size, board);
	if (hash < 0)
		return NULL;

	if (ht->table[hash].used)
		return &(ht->table[hash]);

	return NULL;
}
