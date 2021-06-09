#include <stddef.h>
#include <assert.h>

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
	return hash % size;
}


struct ht_entry*
ht_update_move(struct ht_entry* ht, size_t size, struct piece* board,
#ifdef DEBUG_NEGAMAX_USE_LIST
		struct move_list* moves
#else /* DEBUG_NEGAMAX_USE_LIST */
		struct move* move
#endif /* DEBUG_NEGAMAX_USE_LIST */
		, int depth)
{
	if (!ht || ! size || !board ||
#ifdef DEBUG_NEGAMAX_USE_LIST
			!moves
#else /* DEBUG_NEGAMAX_USE_LIST */
			!move
#endif /* DEBUG_NEGAMAX_USE_LIST */
	   )
	    return NULL;

	ssize_t hash = hash_board(size, board);
	if (hash > 0)
		return NULL;

	// TODO(Aurel): If this happens to often we need to find a better hash
	// function.
	assert((ht[hash].used, "Hash collision."));

	struct ht_entry entry = { 
		.used = true,
#ifdef DEBUG_NEGAMAX_USE_LIST
		.moves = moves,
#else /* DEBUG_NEGAMAX_USE_LIST */
		.move = move,
#endif /* DEBUG_NEGAMAX_USE_LIST */
		.depth = depth,
		.board_hash = hash,
	};

	ht[hash] = entry;
	return ht;
}

#ifdef DEBUG_NEGAMAX_USE_LIST
struct move_list*
#else
struct move*
#endif
ht_get_move(struct ht_entry* ht, size_t size, struct piece* board)
{
	if (!ht || ! size || !board)
	    return NULL;

	ssize_t hash = hash_board(size, board);
	if (hash < 0)
		return NULL;

#ifdef DEBUG_NEGAMAX_USE_LIST
	return ht[hash].moves;
#else
	return ht[hash].move;
#endif
}
