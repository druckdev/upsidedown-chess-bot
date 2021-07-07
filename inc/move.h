#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "list.h"

#include "chess.h"
struct move {
	enum pos start, target;
	bool hit, is_checkmate;
	struct piece promotes_to;
	int rating;
};

int rate_move(struct chess* game, struct move* move);

void fprint_move(FILE* stream, struct move* move);

/**
 * Wrappers around the actual functions to avoid casting and improve
 * readability.
 */
struct move* move_list_pop(struct list* list);
struct move* move_list_peek(struct list* list);
struct list* move_list_cpy(struct list* dest, struct list* src);

/**
 * Sorts the list using insertion sort based on `list_elem.prio`.
 *
 * Runtime: O(n^2)
 */
void move_list_sort(struct list* list);

/**
 * Prints a move list to stream.
 *
 * NOTE(Aurel): This only iterates over the list and does not free anything.
 */
void fprint_move_list(FILE* stream, struct list* list);
#endif /* TYPES_H */
