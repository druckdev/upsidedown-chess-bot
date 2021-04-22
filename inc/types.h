#ifndef TYPES_H
#define TYPES_H

#define bool int

/**
 * A linked list for integer values.
 */
struct list {
	struct list* next;
	void* elem;
};

/**
 * Prepends one element to `list`. If `list` is `NULL` it creates a new `list`
 * with that one element.
 * Runtime: O(1)
 */
struct list* list_push(struct list* list, void* elem);

/**
 * Pops the first element off of `list`, *NOT* returning its element, but the
 * new `list`.
 * *NOTE:* This function does *NOT* free `elem`.
 * Runtime: O(1)
 */
struct list* list_pop(struct list* list);

/**
 * Appends `second` to `first`.
 * Runtime: O(n)
 */
struct list* list_append_list(struct list* first, struct list* second);

#endif /* TYPES_H */
