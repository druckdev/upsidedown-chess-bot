#include <stdio.h>
#include <stdlib.h>

#include "move.h"

struct move_list*
move_list_push(struct move_list* list, void* move)
{
	// NOTE: generator.c depends on this break condition!
	if (!move)
		return list;

	if (!list) {
		list = calloc(1, sizeof(struct move_list));
		if (!list)
			return NULL;
	}

	struct move_list_elem* list_elem = calloc(1, sizeof(struct move_list_elem));
	if (!list_elem) {
		if (!list->last)
			// list is empty so we free it as we probably have created it
			free(list);
		return NULL;
	}

	list->count++;

	list_elem->move = move;

	if (!list->last) {
		list->first = list_elem;
		list->last  = list_elem;
		return list;
	}

	list->last->next = list_elem;
	list_elem->prev  = list->last;
	list->last       = list_elem;
	return list;
}

void*
move_list_pop(struct move_list* list)
{
	if (!list || !list->last)
		return NULL;

	struct move_list_elem* list_elem = list->last;
	void* move                       = list_elem->move;

	list->last = list_elem->prev;
	if (list->last)
		list->last->next = NULL;

	if (list->first == list_elem)
		list->first = NULL;

	list->count--;
	free(list_elem);
	return move;
}

struct move_list_elem*
move_list_remove(struct move_list* list, struct move_list_elem* elem)
{
	if (elem->prev)
		elem->prev->next = elem->next;
	if (elem->next)
		elem->next->prev = elem->prev;

	if (list->first == elem)
		list->first = elem->next;
	if (list->last == elem)
		list->last = elem->prev;

	list->count--;

	struct move_list_elem* next = elem->next;
	free(elem->move);
	free(elem);

	return next;
}

void
move_list_insert(struct move_list* list, struct move_list_elem* new_elem,
                 struct move_list_elem* before)
{
	if (!list || !new_elem)
		return;

	new_elem->prev = before;
	if (before) {
		new_elem->next = before->next;
		before->next   = new_elem;
	} else {
		new_elem->next = list->first;
		list->first    = new_elem;
	}
	if (new_elem->next)
		new_elem->next->prev = new_elem;
	else
		list->last = new_elem;

	list->count++;
}

struct move_list*
move_list_append_move_list(struct move_list* first, struct move_list* second)
{
	// if null or empty return other list
	if (!first || !first->first) {
		free(first);
		return second;
	}
	if (!second || !second->first) {
		free(second);
		return first;
	}

	// link the two lists together
	first->last->next   = second->first;
	second->first->prev = first->last;

	// update first and free second instance
	first->last = second->last;
	first->count += second->count;

	free(second);

	return first;
}

size_t
move_list_count(struct move_list* list)
{
	return list ? list->count : 0;
}

void
move_list_free(struct move_list* list)
{
	if (!list)
		return;

	struct move_list_elem* cur = list->first;
	while (cur) {
		struct move_list_elem* tmp = cur->next;
		free(cur->move);
		free(cur);
		cur = tmp;
	}
	free(list);
}

struct move_list_elem*
move_list_get_first(struct move_list* list)
{
	return list ? list->first : NULL;
}

struct move_list_elem*
move_list_get_next(struct move_list_elem* elem)
{
	return elem ? elem->next : NULL;
}

// Sort a list inplace.
// This uses insertion sort as the lists that we typically use are rather small
// in length.
void
move_list_sort(struct move_list* list)
{
	if (!list || !list->count)
		return;

	struct move_list_elem* cur = list->first->next;
	struct move_list_elem *before, *next;
	while (cur) {
		before = cur->prev;
		while (before && before->prio > cur->prio)
			before = before->prev;

		if (before == cur->prev) {
			// cur should stay at its place
			cur = cur->next;
			continue;
		}

		// Backup next before reordering
		next = cur->next;

		// Move `cur` from current position behind `before`
		// This is a hybrid form of `list_remove` and `move_list_insert` but without
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
fprint_move_list(FILE* stream, struct move_list* list)
{
	if (!list)
		return;

	struct move_list_elem* cur_elem = list->last;
	int i                           = 0;
	while (cur_elem) {
		struct move* cur_move = cur_elem->move;
		fprintf(stream, "%i ", i++);
		fprint_move(stream, cur_move);
		cur_elem = cur_elem->prev;
	}
}
