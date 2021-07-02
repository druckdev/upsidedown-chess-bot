#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/**
 * A doubly linked list.
 */
struct list_elem {
	struct list_elem *prev, *next;
	void* elem;
};

struct list {
	struct list_elem *first, *last;
	size_t count;
};

/**
 * Appends one element to `list`. If `list` is `NULL` it creates a new `list`
 * with that one new element as its object.
 *
 * *NOTE:* If this function is called with an empty list (not NULL, actually
 * empty) and an error occurs during the allocation of `list_elem`, it will free
 * the memory of the list, as it thinks it probably has allocated that memory.
 *
 * Runtime: O(1)
 */
struct list* list_push(struct list* list, void* elem);

/**
 * Pops the last element off of `list`, returning its element and updating the
 * list.
 *
 * *NOTE:* This function does *NOT* free the object.
 *
 * Runtime: O(1)
 */
void* list_pop(struct list* list);
void* list_peek(struct list* list);

/*
 * Removes `elem` from `list` and returns its successor.
 * Frees the element and it's object.
 *
 * Runtime: O(1)
 */
struct list_elem* list_remove(struct list* list, struct list_elem* elem);

/*
 * Insert `new_elem` after `before` in `list`.
 * If `before == NULL` it is inserted at the very beginning.
 *
 * Runtime: O(1)
 */
void list_insert(struct list* list, struct list_elem* new_elem,
                 struct list_elem* before);

/**
 * Appends `second` to `first`.
 *
 * Runtime: O(1)
 */
struct list* list_append_list(struct list* first, struct list* second);

/**
 * counts elements in `list`.
 *
 * Runtime: O(n)
 */
size_t list_count(struct list* list);

/**
 * Frees all elements, their objects and the list itself.
 *
 * Runtime: O(n)
 */
void list_free(struct list* list);

/*
 * Return first in `list` if exists.
 *
 * Runtime: O(1)
 */
struct list_elem* list_get_first(struct list* list);

/*
 * Return next in `list` if exists.
 *
 * Runtime: O(1)
 */
struct list_elem* list_get_next(struct list_elem* elem);

#endif /* LIST_H */