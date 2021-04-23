#include <stdio.h>
#include <stdlib.h>

#include "types.h"

struct list*
list_push(struct list* list, void* elem)
{
	if (!list)
		list = malloc(sizeof(struct list));
	if (!list)
		return NULL;

	struct list_elem* list_elem = malloc(sizeof(struct list_elem));
	if (!list_elem)
		return NULL;

	list_elem->prev = NULL;
	list_elem->next = NULL;
	list_elem->elem = elem;

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
list_pop(struct list* list)
{
	if (!list)
		return NULL;

	// empty list
	if (!list->last)
		return NULL;

	struct list_elem* list_elem = list->last;
	void* elem                  = list_elem->elem;

	list->last = list_elem->prev;
	if (list->last)
		list->last->next = NULL;

	free(list_elem);
	return elem;
}

struct list*
list_append_list(struct list* first, struct list* second)
{
	// if null or empty return other list
	if (!first || !first->first)
		return second;
	if (!second || !second->first)
		return first;

	// link the two lists together
	first->last->next   = second->first;
	second->first->prev = first->last;

	// update first and free second instance
	first->last = second->last;
	free(second);

	return first;
}
