#include <stdio.h>
#include <stdlib.h>

#include "types.h"

struct list*
list_push(struct list* list, void* object)
{
	if (!object)
		return list;

	if (!list) {
		list = malloc(sizeof(struct list));
		if (!list)
			return NULL;
		list->first = NULL;
		list->last  = NULL;
		list->count = 0;
	}

	struct list_elem* list_elem = malloc(sizeof(struct list_elem));
	if (!list_elem) {
		if (!list->last)
			// list is empty so we free it as we probably have created it
			free(list);
		return NULL;
	}

	list->count++;

	list_elem->prev   = NULL;
	list_elem->next   = NULL;
	list_elem->object = object;

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
	void* object                = list_elem->object;

	list->last = list_elem->prev;
	if (list->last)
		list->last->next = NULL;

	if (list->first == list_elem)
		list->first = NULL;

	list->count--;
	free(list_elem);
	return object;
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
	first->count += second->count;
	free(second);

	return first;
}

int
list_count(struct list* list)
{
	struct list_elem* next = list->first;
	int count              = 0;
	while (next) {
		next = next->next;
		count++;
	}
	return count;
}

void
free_list(struct list* list)
{
	if (!list)
		return;

	struct list_elem* cur = list->first;
	while (cur) {
		struct list_elem* tmp = cur->next;
		free(cur->object);
		free(cur);
		cur = tmp;
	}
	free(list);
}
