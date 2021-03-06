#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "chess.h"
#include "unity.h"

#include "devel_chess.h"
#include "devel_generator.h"

#include "helper.h"

void
test_checkmate()
{
	printf("TEST: is_checkmate field of moves\t");

	char* fen          = "8/8/8/8/8/5n2/pppppppN/rnbqkbr1 w";
	struct chess chess = init_chess(' ', 9999, 99);
	fen_to_chess(fen, &chess);

	struct move_list* moves = generate_moves(&chess, true, false);

	TEST_ASSERT_EQUAL_INT(3, move_list_count(moves));

	while (move_list_count(moves)) {
		struct move* move = move_list_pop(moves);

		if (move->start != H2 || move->target != F3) {
			free(move);
			continue;
		}

		TEST_ASSERT_MESSAGE(move->is_checkmate,
		                    "Move should be a checkmate move.");
		free(move);
	}
	move_list_free(moves);
	free(chess.board);
}

/* THIS IS AN EXAMPLE. */
void
test_generate_moves_piece()
{
	printf("TEST: %s\n", test_boards[0].fen);

	// init game
	struct chess chess = init_chess(' ', 9999, 99);
	fen_to_chess(test_boards[0].fen, &chess);

	// verify generator
	struct move_list* list = generate_moves_piece(&chess, D3, true, false);
	int list_len           = move_list_count(list);

	if (list_len != test_boards[0].move_cnt) {
		printf("\n");
		fprint_board(stdout, chess.board, list);
	} else {
		move_list_free(list);
	}

	free(chess.board);
	TEST_ASSERT_EQUAL_INT(test_boards[0].move_cnt, list_len);
}

static size_t test_idx = 0;
void
test_game_samples()
{
	printf("TEST: %s ", test_boards[test_idx].fen);
	size_t len = strnlen(test_boards[test_idx].fen, 73);
	for (size_t i = 73; i > len; --i)
		putchar(' ');
	fflush(stdout);

	// init game
	struct chess chess = { 0 };
	chess.board        = calloc(64, sizeof(*chess.board));
	fen_to_chess(test_boards[test_idx].fen, &chess);

	// verify generator
	struct move_list* list;
	int old_len = 0;
	for (size_t i = 0; i < TEST_SAMPLES_ITERATIONS; i++) {
		list = generate_moves(&chess, true, false);

		int list_len = move_list_count(list);
		if ((i && list_len != old_len) ||
		    list_len != test_boards[test_idx].move_cnt) {
			printf("\n");
			fprint_board(stdout, chess.board, list);
		} else {
			move_list_free(list);
		}

		if (i)
			TEST_ASSERT_EQUAL_INT(old_len, list_len);
		TEST_ASSERT_EQUAL_INT(test_boards[test_idx].move_cnt, list_len);

		old_len = list_len;
	}
	free(chess.board);
}

void
test_generator()
{
	// run test cases
	size_t tests_length = sizeof(test_boards) / sizeof(*test_boards);
	for (test_idx = 0; test_idx < tests_length; test_idx++) {
		RUN_TEST(test_game_samples);
	}
	RUN_TEST(test_generate_moves_piece);
	RUN_TEST(test_checkmate);
}
