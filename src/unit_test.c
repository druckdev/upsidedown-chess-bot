#include "unit_test.h"
#include "board.h"
#include "generator.h"
#include "types.h"
#include "unity.h"

void
setUp(void)
{
	// set stuff up here
}

void
tearDown(void)
{
	// clean stuff up here
}

// runs generator with test boards and checks whether
// the correct amount of moves is generated
void
test_generator(void)
{
	struct test_case {
		char* fen;
		enum COLOR moving;
		int move_cnt;
	};

	// define test cases
	struct test_case tests[] = {
		{ "8/8/8/8/8/3k4/8/8", BLACK, 8}, // test king
		{ "8/8/8/8/3r4/8/8/8", BLACK, 14}, // test rook
		{ "8/8/8/8/3b4/8/8/8", BLACK, 13}, // test bishop
		{ "8/8/8/8/8/8/6p1/8", BLACK, 4}, // test pawn (black)
		{ "8/6P1/8/8/8/8/8/8", WHITE, 4}, // test pawn (white)
		{ "8/8/8/8/3q4/8/8/8", BLACK, 27}, // test queen
		{ "8/8/8/8/3n4/8/8/8", BLACK, 8}, // test knight

		{ "RNBKQB1R/PPPPPPPP/8/8/4N3/2n5/pppppppp/r1bkqbnr", BLACK, 12 },
		{ "RqBQKB1R/P1PPPPQP/2N2N2/8/8/4n3/p1pppppp/r1bqkbnr", WHITE, 29 },
		{ "RNBQKB1R/PPPPPPPP/5N2/8/8/5n2/pppppppp/rnbqkb1r", WHITE, 12 },
		{ "RNBQKBQR/PPPPPP1P/5N2/8/8/5n2/pppppp1p/rnbqkbqr", WHITE, 15 },
		{ "RQBQKB1R/P1PPPPPP/2N2N2/8/5n2/n7/pppppppp/r1bqkb1r", WHITE, 22 },
		{ "RNBQK2R/PPPPPPBP/7N/3N4/8/n1nn1n2/pbqppppp/r3kb1r", BLACK, 38 },

		{ "RNBKQB1R/PPPPPPPP/8/3N4/8/5n2/pppppp1p/rnbkqbrr", WHITE, 12 },
		{ "qRQQK1QR/3PPP1P/5N1B/4Q3/1b6/1Bn5/3ppp1p/1rqqkbqr", WHITE, 61 },
		{ "4K2R/4P2P/8/q1N5/8/6B1/p1k3q1/r1bR4", WHITE, 32 },
		{ "RNQQKB1r/PP1PPPP1/8/8/8/8/pp1ppp2/r1qkb1q1", WHITE, 18 },
		{ "3QK3/4Q1P1/8/8/8/1rq1n3/2n5/2k5", WHITE, 33 },
		/*{ "r1bqkb1r/pppppppp/2n2n2/8/8/2N2N2/PPPPPPPP/R1BQKB1R", WHITE, 36 },
		{ "r1bqkbqr/pppppp1p/2n2n2/8/8/2N2N2/P1PPPPPP/RQBQKB1R", WHITE, 38 },
		{ "r1bqk1nr/pppppp1p/2k4b/8/8/2N2N2/P1PPPP1P/RQBQKB1R", WHITE, 36 },
		{ "r1bqkqnr/ppppp2p/2n4b/3N4/8/5N2/P1PPPP1P/RQBQKB1R", WHITE, 42 },*/
		{ "3R4/3R4/8/7K/7B/8/3r2n1/3k4", BLACK, 10 },
		{ "6KR/8/1N6/8/8/3q4/4b3/6kr", WHITE, 15 },
	};

	// run test cases
	int tests_length = sizeof(tests) / sizeof(*tests);
	for (int i = 0; i < tests_length; i++) {
		printf("TEST : %s \n", tests[i].fen);

		// init game
		struct chess chess;
		chess.moving       = tests[i].moving;
		board_from_fen(tests[i].fen, chess.board);

		// verify generator
		struct list* list = generate_moves(&chess, true);
		int list_length   = list_count(list);

		// pretty print board
		print_board(chess.board, list);

		// pretty print moves
		printf("Expected : %d , Got : %d \n\n", tests[i].move_cnt, list_length);

		TEST_ASSERT(list_length == tests[i].move_cnt);

		printf("\n-------------------------------------------\n");
	}
}

// TODO(Luis): use generate_test_runner.rb
int
main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_generator);
	return UNITY_END();
}
