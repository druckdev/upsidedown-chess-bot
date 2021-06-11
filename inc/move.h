#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "chess.h"

struct move {
	enum pos start, target;
	bool hit, is_checkmate;
	struct piece promotes_to;
	int rating;
};

int rate_move(struct piece* board, struct move* move);

void fprint_move(FILE* stream, struct move* move);

/**
 * A doubly linked list.
 */
struct move_list_elem {
	struct move_list_elem *prev, *next;
	struct move* move;
};

struct move_list {
	struct move_list_elem *first, *last;
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
struct move_list* move_list_push(struct move_list* list, struct move* move);

/**
 * Pops the last element off of `list`, returning its element and updating the
 * list.
 *
 * *NOTE:* This function does *NOT* free the object.
 *
 * Runtime: O(1)
 */
struct move* move_list_pop(struct move_list* list);
struct move* move_list_peek(struct move_list* list);

/*
 * Removes `elem` from `list` and returns its successor.
 * Frees the element and it's object.
 *
 * Runtime: O(1)
 */
struct move_list_elem* move_list_remove(struct move_list* list,
                                        struct move_list_elem* elem);

/*
 * Insert `new_elem` after `before` in `list`.
 * If `before == NULL` it is inserted at the very beginning.
 *
 * Runtime: O(1)
 */
void move_list_insert(struct move_list* list, struct move_list_elem* new_elem,
                      struct move_list_elem* before);

/**
 * Appends `second` to `first`.
 *
 * Runtime: O(1)
 */
struct move_list* move_list_append_move_list(struct move_list* first,
                                             struct move_list* second);

/**
 * counts elements in `list`.
 *
 * Runtime: O(n)
 */
size_t move_list_count(struct move_list* list);

/**
 * Frees all elements, their objects and the list itself.
 *
 * Runtime: O(n)
 */
void move_list_free(struct move_list* list);

struct move_list* move_list_cpy(struct move_list* dest, struct move_list* src);

struct move_list_elem* move_list_get_first(struct move_list* list);
struct move_list_elem* move_list_get_next(struct move_list_elem* elem);

/**
 * Sorts the list using insertion sort based on `list_elem.prio`.
 *
 * Runtime: O(n^2)
 */
void move_list_sort(struct move_list* list);

/**
 * Prints a move list to stream.
 *
 * NOTE(Aurel): This only iterates over the list and does not free anything.
 */
void fprint_move_list(FILE* stream, struct move_list* list);
#endif /* TYPES_H */
