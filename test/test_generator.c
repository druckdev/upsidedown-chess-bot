#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "chess.h"
#include "unity.h"

#include "devel_generator.h"

#include "helper.h"

void
test_checkmate()
{
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
	struct list* list = generate_moves_piece(chess.board, D3, true, false);
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
	printf("TEST: %s\n", test_boards[test_idx].fen);

	// init game
	struct chess chess;
	chess.board = calloc(64, sizeof(*chess.board));
	fen_to_chess(test_boards[test_idx].fen, &chess);

	// verify generator
	struct list* list = generate_moves(&chess, true, false);
	int list_length   = list_count(list);

	if (list_length != test_boards[test_idx].move_cnt) {
		printf("\n");
		print_board(chess.board, list);
	} else {
		list_free(list);
	}

	free(chess.board);
	TEST_ASSERT_EQUAL_INT(test_boards[test_idx].move_cnt, list_length);
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
	RUN_TEST(test_checkmate);
}
