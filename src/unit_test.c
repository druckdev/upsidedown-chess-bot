#include <stdio.h>

#include "board.h"
#include "generator.h"
#include "helper.h"
#include "types.h"
#include "unity.h"

// Necessary for Unity
// clang-format off
void setUp(void)    {}
void tearDown(void) {}
// clang-format on

static size_t test_idx = 0;

// runs generator with test boards and checks whether
// the correct amount of moves is generated
void
test_generator(void)
{
	printf("TEST: %s\n", test_boards[test_idx].fen);

	// init game
	struct chess chess;
	fen_to_chess(test_boards[test_idx].fen, &chess);

	// verify generator
	struct list* list = generate_moves(&chess, true, false);
	int list_length   = list_count(list);

	if (list_length != test_boards[test_idx].move_cnt) {
		printf("\n");
		print_board(chess.board, list);
	}

	TEST_ASSERT_EQUAL_INT(list_length, test_boards[test_idx].move_cnt);
}

// TODO(Luis): use generate_test_runner.rb
int
main(void)
{
	UNITY_BEGIN();

	// run test cases
	int tests_length = sizeof(test_boards) / sizeof(*test_boards);
	for (test_idx = 0; test_idx < tests_length; test_idx++) {
		RUN_TEST(test_generator);
	}
	return UNITY_END();
}
