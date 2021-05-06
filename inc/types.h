#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

/**
 * A doubly linked list.
 */
struct list_elem {
	struct list_elem *prev, *next;
	void* object;
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
struct list* list_push(struct list* list, void* object);

/**
 * Pops the last element off of `list`, returning its element and updating the
 * list.
 *
 * *NOTE:* This function does *NOT* free the object.
 *
 * Runtime: O(1)
 */
void* list_pop(struct list* list);

/*
 * Removes `elem` from `list` and returns its successor.
 * Frees the element and it's object.
 *
 * Runtime: O(1)
 */
struct list_elem* list_remove(struct list* list, struct list_elem* elem);

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
int list_count(struct list* list);

/**
 * Frees all elements, their objects and the list itself.
 *
 * Runtime: O(n)
 */
void list_free(struct list* list);
#endif /* TYPES_H */
