#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "pst.h"

#include "list.h"

/**
 * Rates a move from the point of view of the moving player.
 *
 * NOTE(Aurel): Currently it only calculates the difference in piece-value on
 * the board the move would make.
 */
int
rate_move(struct chess* game, struct move* move)
{
	int rating = 0;

	if (move->hit) {
		// add the value of the hit piece to the rating
		struct piece to = game->board[move->target];
		rating += PIECE_VALUES[to.type];
	}

	if (move->is_checkmate)
		// checkmate is like hitting the king, so add the kings value to the
		// rating
		rating += PIECE_VALUES[KING];

	struct piece from        = game->board[move->start];
	enum piece_type promo_to = move->promotes_to.type;
	if (promo_to)
		// add the difference in value between the old and new piece to the
		// rating
		rating += PIECE_VALUES[promo_to] - PIECE_VALUES[from.type];

	// piece square tables
#ifdef PIECE_SQUARE_TABLES
	// Add difference between position weights. Use promotes_to on target when
	// promoting.
	rating += get_pst_val(game, move->target, promo_to ? promo_to : from.type);
	rating -= get_pst_val(game, move->start, from.type);
#endif /* PIECE_SQUARE_TABLES */

	return rating;
}

void
fprint_move(FILE* stream, struct move* move)
{
	char start[3], target[3];
	pos_to_str(move->start, start);
	pos_to_str(move->target, target);

	fprintf(stream, "%s,%s,", start, target);

	char promotes_to_char = piece_to_chr(move->promotes_to);
	fprintf(stream, "%c", promotes_to_char);
	fprintf(stream, "\t%i", move->rating);

	fprintf(stream, " %s", move->hit ? "hits" : "");
	fprintf(stream, " %s", move->is_checkmate ? "checkmates" : "");
	fprintf(stream, "\n");
	//fflush(stream);
}

struct move*
move_list_pop(struct list* list)
{
	return list_pop(list);
}
struct move*
move_list_peek(struct list* list)
{
	return list_peek(list);
}

struct list*
move_list_cpy(struct list* dest, struct list* src)
{
	if (!src)
		return NULL;

	if (!dest)
		dest = malloc(sizeof(*dest));

	dest->count = src->count;
	dest->first = NULL;
	dest->last  = NULL;

	struct list_elem* cur_elem = src->last;
	struct list_elem* last     = NULL;
	while (cur_elem) {
		struct move* move      = malloc(sizeof(*move));
		struct list_elem* elem = malloc(sizeof(*elem));
		if (!move || !elem)
			return NULL;

		memcpy(move, cur_elem->elem, sizeof(*move));
		elem->elem = move;
		elem->prev = NULL;
		elem->next = last;
		if (last)
			last->prev = elem;

		dest->first = elem;
		if (!dest->last)
			dest->last = elem;

		cur_elem = cur_elem->prev;
	}
	return dest;
}

// Sort a list inplace.
// This uses insertion sort as the lists that we typically use are rather small
// in length.
void
move_list_sort(struct list* list)
{
	if (!list || !list->count)
		return;

	struct list_elem* cur = list->first->next;
	struct list_elem *before, *next;
	while (cur) {
		before = cur->prev;
		while (before && ((struct move*)before->elem)->rating >
		                         ((struct move*)cur->elem)->rating)
			before = before->prev;

		if (before == cur->prev) {
			// cur should stay at its place
			cur = cur->next;
			continue;
		}

		// Backup next before reordering
		next = cur->next;

		// Move `cur` from current position behind `before`
		// This is a hybrid form of `list_remove` and `list_insert` but without
		// unnecessary instructions.

		// Remove --------------------------------------------------------------
		// cur->prev can never be NULL as we are starting at first->next
		cur->prev->next = cur->next;
		if (cur->next)
			cur->next->prev = cur->prev;
		else
			list->last = cur->prev;
		// cur can never be list->first as we are starting at first->next

		// Insert --------------------------------------------------------------
		cur->prev = before;
		if (before) {
			cur->next    = before->next;
			before->next = cur;
		} else {
			cur->next   = list->first;
			list->first = cur;
		}
		if (cur->next)
			cur->next->prev = cur;

		cur = next;
	}
}

void
fprint_move_list(FILE* stream, struct list* list)
{
	if (!list)
		return;

	struct list_elem* cur_elem = list->last;
	int i                      = 0;
	while (cur_elem) {
		struct move* cur_move = cur_elem->elem;
		fprintf(stream, "%i ", i++);
		fprint_move(stream, cur_move);
		cur_elem = cur_elem->prev;
	}
}
