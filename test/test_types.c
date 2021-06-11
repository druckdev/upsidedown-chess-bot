#include <stdio.h>
#include <stdlib.h>

#include "move.h"
#include "unity.h"

void
test_move_list_sort()
{
	printf("TEST: move_list_sort\t");

#define len 6
	int prios_unsorted[]   = { 10, 3, 4, 23, 234, -1 };
	int prios_sorted[]     = { -1, 3, 4, 10, 23, 234 };
	struct move moves[len] = { 0 };
	struct move_list* list = NULL;
	for (int i = 0; i < len; ++i) {
		moves[i].rating = prios_unsorted[i];
		list            = move_list_push(list, &moves[i]);
	}

	move_list_sort(list);

	struct move_list_elem* cur = list->first;
	struct move_list_elem* old = NULL;
	struct move_list_elem *before, *after;
	for (size_t i = 0; i < len; ++i) {
		TEST_ASSERT_NOT_NULL(cur);

		before = cur->prev;
		after  = cur->next;
		if (i)
			TEST_ASSERT_NOT_NULL(before);
		else
			TEST_ASSERT_NULL(before);

		if (i < len - 1)
			TEST_ASSERT_NOT_NULL(after);
		else
			TEST_ASSERT_NULL(after);

		TEST_ASSERT_EQUAL_PTR(old, cur->prev);
		TEST_ASSERT_EQUAL_INT(prios_sorted[i], cur->move->rating);

		free(old);

		old = cur;
		cur = cur->next;
	}

	TEST_ASSERT_NULL(cur);
	TEST_ASSERT_EQUAL_PTR(list->last, old);

	// We cannot use `list_free` as the object lays on the stack and is the same
	// for each `list_elem`.
	free(list->last);
	free(list);
}

void
test_types()
{
	RUN_TEST(test_move_list_sort);
}
