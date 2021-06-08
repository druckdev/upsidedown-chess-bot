#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "chess.h"
#include "unity.h"

#include "devel_generator.h"

#include "helper.h"

void
test_checkmate()
{
	printf("TEST: is_checkmate field of moves\t");

	char* fen = "8/8/8/8/8/5n2/pppppppN/rnbqkbr1 w";
	struct chess chess;
	chess.board = calloc(64, sizeof(*chess.board));
	fen_to_chess(fen, &chess);

	struct list* moves = generate_moves(&chess, true, false);

	TEST_ASSERT_EQUAL_INT(3, list_count(moves));

	while (list_count(moves)) {
		struct move* move = list_pop(moves);

		if (move->start != H2 || move->target != F3) {
			free(move);
			continue;
		}

		TEST_ASSERT_MESSAGE(move->is_checkmate,
		                    "Move should be a checkmate move.");
		free(move);
	}
	list_free(moves);
	free(chess.board);
}

/* THIS IS AN EXAMPLE. */
void
test_generate_moves_piece()
{
	printf("TEST: %s\n", test_boards[0].fen);

	// init game
	struct chess chess;
	chess.board = calloc(64, sizeof(*chess.board));
	fen_to_chess(test_boards[0].fen, &chess);

	// verify generator
	struct list* list = generate_moves_piece(&chess, D3, true, false);
	int list_length   = list_count(list);

	if (list_length != test_boards[0].move_cnt) {
		printf("\n");
		print_board(chess.board, list);
	} else {
		list_free(list);
	}

	free(chess.board);
	TEST_ASSERT_EQUAL_INT(test_boards[0].move_cnt, list_length);
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
	struct list* list;
	int old_len = 0;
	for (size_t i = 0; i < TEST_SAMPLES_ITERATIONS; i++) {
		list = generate_moves(&chess, true, false);

		int list_len = list_count(list);
		if ((i && list_len != old_len) ||
		    list_len != test_boards[test_idx].move_cnt) {
			printf("\n");
			print_board(chess.board, list);
		} else {
			list_free(list);
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
