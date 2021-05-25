#include <stdlib.h>

#include "unity.h"
#include "types.h"
// #include "devel_types.h"

void
test_list_sort()
{
	int prios_unsorted[] = { 10, 3, 4, 23, 234, -1};
	int prios_sorted[] = { -1, 3, 4, 10, 23, 234 };

	struct list* list = NULL;
	int obj = 0;
	for (size_t i = 0; i < sizeof(prios_unsorted) / sizeof(*prios_unsorted); ++i) {
		list = list_push(list, &obj);
		list->last->prio = prios_unsorted[i];
	}

	list_sort(list);

	struct list_elem* cur = list->first;
	for (size_t i = 0; i < sizeof(prios_unsorted) / sizeof(*prios_unsorted); ++i) {
		TEST_ASSERT_EQUAL_INT(prios_sorted[i], cur->prio);
		cur = cur->next;
	}
}

void
test_types()
{
	RUN_TEST(test_list_sort);
}
