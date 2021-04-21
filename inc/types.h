#ifndef TYPES_H
#define TYPES_H

#define bool int

/**
 * A linked list for integer values.
 */
struct int_list {
    struct int_list* next;
    int elem;
};

/**
 * Prepends one integer element to `list`. If `list` is `NULL` it creates a new
 * `int_list` with that one element.
 * Runtime: O(1)
 */
struct int_list* int_list_push(struct int_list* list, int elem);

/**
 * Pops the first element off of `list`, *NOT* returning its element, but the
 * new `int_list`.
 * Runtime: O(1)
 */
struct int_list* int_list_pop(struct int_list* list);

/**
 * Appends `second` to `first`.
 * Runtime: O(n)
 */
struct int_list* int_list_append_int_list(struct int_list* first, struct int_list* second);

#endif /* TYPES_H */
