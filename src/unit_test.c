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
	struct test_case tests[17] = {
		{ "RNBKQB1R/PPPPPPPP/8/3N4/8/5n2/pppppp1p/rnbkqbrr", WHITE, 12 },
		{ "RNBKQB1R/PPPPPPPP/8/8/4N3/2n5/pppppppp/r1bkqbnr", BLACK, 12 },
		{ "RqBQKB1R/P1PPPPQP/2n2N2/8/8/5N2/pbpppppp/rq2kbnr", BLACK, 2 },
		{ "RNBQKB1R/PPPPPPPP/5N2/8/8/5n2/pppppppp/rnbqkb1r", WHITE, 2 },
		{ "RNBQKBQR/PPPPPP1P/5N2/8/8/5n2/pppppp1p/rnbqkbqr", WHITE, 5 },
		{ "qRQQK1QR/3PPP1P/5N1B/4Q3/1b6/1Bn5/3ppp1p/1rqqkbqr", WHITE, 1 },
		{ "4K2R/4P2P/8/q1N5/8/6B1/p1k3q1/r1bR4", WHITE, 2 },
		{ "RNQQKB1r/PP1PPPP1/8/8/8/8/pp1ppp2/r1qkb1q1", WHITE, 8 },
		{ "3QK3/4Q1P1/8/8/8/1rq1n3/2n5/2k5", WHITE, 3 },
		{ "RQBQKB1R/P1PPPPPP/2N2N2/8/5n2/n7/pppppppp/r1bqkb1r", WHITE, 2 },
		{ "RNBQK2R/PPPPPPBP/7N/3N4/8/n1nn1n2/pbqppppp/r3kb1r", BLACK, 8 },
		{ "r1bqkb1r/pppppppp/2n2n2/8/8/2N2N2/PPPPPPPP/R1BQKB1R", WHITE, 6 },
		{ "r1bqkbqr/pppppp1p/2n2n2/8/8/2N2N2/P1PPPPPP/RQBQKB1R", WHITE, 8 },
		{ "r1bqk1nr/pppppp1p/2k4b/8/8/2N2N2/P1PPPP1P/RQBQKB1R", WHITE, 6 },
		{ "r1bqkqnr/ppppp2p/2n4b/3N4/8/5N2/P1PPPP1P/RQBQKB1R", WHITE, 2 },
		{ "6KR/8/1N6/8/8/3q4/4b3/6kr w", WHITE, 5 },
		{ "3R4/3R4/8/7K/7B/8/3r2n1/3k4 b", BLACK, 1 },
	};

	// run test cases
	int tests_length = sizeof(tests) / sizeof(*tests);
	for (int i = 0; i < tests_length; i++) {
		printf("\n\nTEST : %s", tests[i].fen);

		// init game
		struct chess chess = { 0 };
		chess.moving       = tests[i].moving;
		board_from_fen(tests[i].fen, chess.board);

		// verify generator
		struct list* list = generate_moves(&chess);
		int list_length   = list_count(list);
		TEST_ASSERT(list_length == tests[i].move_cnt);
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
