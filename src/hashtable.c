#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "hashtable.h"

ssize_t
hash_board(size_t size, struct piece* board, enum color moving)
{
	if (!size || !board)
		return -1;

	// TODO(Aurel): Think about a better hash function.
	size_t hash = 0;
	for (size_t i = 0; i < 64;) {
		int val = get_piece_value(board[i++].type);
		hash += i * val;
		hash += (64 - i) * val;
	}
	hash += moving;
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

	memset(ht->table, 0, size * sizeof(void*));

	ht->size = size;
	return ht;
}

void
free_ht(struct ht* ht)
{
	if (!ht)
		return;

	for (size_t i = 0; i < ht->size; ++i) {
		struct ht_entry* cur = ht->table[i];
		while (cur) {
			struct ht_entry* tmp = cur->next;
			move_list_free(cur->moves);
			free(cur->board);
			free(cur);
			cur = tmp;
		}
		assert(!cur);
		ht->table[i] = NULL;
	}
}

struct ht_entry*
ht_update_entry(struct ht* ht, struct piece* board, enum color moving,
                struct move_list* moves, size_t rating, size_t depth)
{
	if (!ht || !board || !moves)
		return NULL;

	ssize_t hash = hash_board(ht->size, board, moving);
	if (hash < 0)
		return NULL;

	struct ht_entry* cur  = ht->table[hash];
	struct ht_entry* prev = NULL;
	while (cur) {
		if (moving == cur->moving && is_same_board(board, cur->board)) {
			// found entry
			if (depth > cur->depth) {
				// only update if depth is higher
				move_list_free(cur->moves);
				cur->moves  = moves;
				cur->depth  = depth;
				cur->rating = rating;
			}
			break;
		}
		prev = cur;
		cur  = cur->next;
	}
	if (!cur) {
		// no fitting entry found - create a new one
		struct ht_entry* new_entry = malloc(sizeof(*new_entry));
		if (!new_entry)
			return NULL;

		new_entry->moves  = moves;
		new_entry->depth  = depth;
		new_entry->rating = rating;
		new_entry->moving = moving;
		new_entry->board  = board_cpy(board);
		new_entry->next   = NULL;

		if (prev)
			prev->next = new_entry;
		else
			ht->table[hash] = new_entry;

		cur = new_entry;
	}

	return cur;
}

struct ht_entry*
ht_get_entry(struct ht* ht, struct piece* board, enum color moving)
{
	if (!ht || !board)
		return NULL;

	ssize_t hash = hash_board(ht->size, board, moving);
	if (hash < 0)
		return NULL;

	struct ht_entry* cur = ht->table[hash];
	while (cur) {
		if (moving == cur->moving && is_same_board(board, cur->board))
			return cur;
		cur = cur->next;
	}
	return NULL;
}
