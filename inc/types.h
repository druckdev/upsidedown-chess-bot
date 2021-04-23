#ifndef TYPES_H
#define TYPES_H

#define bool int

/**
 * A doubly linked list.
 */
struct list_elem {
	struct list_elem *prev, *next;
	void* object;
};

struct list {
	struct list_elem *first, *last;
};

/**
 * Appends one element to `list`. If `list` is `NULL` it creates a new `list`
 * with that one new element as its object.
 * Runtime: O(1)
 */
struct list* list_push(struct list* list, void* object);

/**
 * Pops the last element off of `list`, returning its element and updating the
 * list.
 * *NOTE:* This function does *NOT* free the object.
 * Runtime: O(1)
 */
void* list_pop(struct list* list);

/**
 * Appends `second` to `first`.
 * Runtime: O(1)
 */
struct list* list_append_list(struct list* first, struct list* second);

#endif /* TYPES_H */
