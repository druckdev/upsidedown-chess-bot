#include <stdlib.h>

#include "list.h"

struct list_elem*
list_get_first(struct list* list)
{
	return list ? list->first : NULL;
}

struct list_elem*
list_get_next(struct list_elem* elem)
{
	return elem ? elem->next : NULL;
}

struct list*
list_push(struct list* list, void* elem)
{
	// NOTE: generator.c depends on this break condition!
	if (!elem)
		return list;

	if (!list) {
		list = calloc(1, sizeof(struct list));
		if (!list)
			return NULL;
	}

	struct list_elem* list_elem = calloc(1, sizeof(struct list_elem));
	if (!list_elem) {
		if (!list->last)
			// list is empty so we free it as we probably have created it
			free(list);
		return NULL;
	}

	list->count++;

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
	if (!list || !list->last)
		return NULL;

	struct list_elem* list_elem = list->last;
	void* elem                  = list_elem->elem;

	list->last = list_elem->prev;
	if (list->last)
		list->last->next = NULL;

	if (list->first == list_elem)
		list->first = NULL;

	list->count--;
	free(list_elem);
	return elem;
}

void*
list_peek(struct list* list)
{
	if (!list)
		return NULL;
	return list->first->elem;
}

struct list_elem*
list_remove(struct list* list, struct list_elem* elem)
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

	struct list_elem* next = elem->next;
	free(elem->elem);
	free(elem);

	return next;
}

void
list_insert(struct list* list, struct list_elem* new_elem,
            struct list_elem* before)
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

struct list*
list_append_list(struct list* first, struct list* second)
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
list_count(struct list* list)
{
	return list ? list->count : 0;
}

void
list_free(struct list* list)
{
	if (!list)
		return;

	struct list_elem* cur = list->first;
	while (cur) {
		struct list_elem* tmp = cur->next;
		free(cur->elem);
		free(cur);
		cur = tmp;
	}
	free(list);
}