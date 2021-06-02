#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "unity.h"
// #include "devel_types.h"

void
test_list_sort()
{
	printf("TEST: list_sort\t");

	int prios_unsorted[] = { 10, 3, 4, 23, 234, -1 };
	int prios_sorted[]   = { -1, 3, 4, 10, 23, 234 };
	size_t len           = sizeof(prios_unsorted) / sizeof(*prios_unsorted);

	struct list* list = NULL;
	int obj           = 0;
	for (size_t i = 0; i < len; ++i) {
		list             = list_push(list, &obj);
		list->last->prio = prios_unsorted[i];
	}

	list_sort(list);

	struct list_elem* cur = list->first;
	struct list_elem* old = NULL;
	struct list_elem *before, *after;
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

		TEST_ASSERT_EQUAL_INT(prios_sorted[i], cur->prio);

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
	RUN_TEST(test_list_sort);
}
