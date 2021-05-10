#include "board.h"
#include "chess.h"
#include "generator.h"
#include "unity.h"


#include "helper.h"
/* THIS IS AN EXAMPLE. */
void
test_generate_moves_piece()
{
	printf("TEST: %s\n", test_boards[0].fen);

	// init game
	struct chess chess;
	fen_to_chess(test_boards[0].fen, &chess);

	// verify generator
	struct list* list = generate_moves_piece(chess.board, D3, true, false);
	int list_length   = list_count(list);

	if (list_length != test_boards[0].move_cnt) {
		printf("\n");
		print_board(chess.board, list);
	}

	TEST_ASSERT_EQUAL_INT(list_length, test_boards[0].move_cnt);
}



static size_t test_idx = 0;
void
test_game_samples()
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

void
test_generator()
{
	// run test cases
	int tests_length = sizeof(test_boards) / sizeof(*test_boards);
	for (test_idx = 0; test_idx < tests_length; test_idx++) {
		RUN_TEST(test_game_samples);
	}
	RUN_TEST(test_generate_moves_piece);
}
