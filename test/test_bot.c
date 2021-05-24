#include "board.h"
#include <stdlib.h>

#include "chess.h"
#include "devel_bot.h"
#include "unity.h"

void
test_rate_board()
{
	struct chess game = { .board  = calloc(64, sizeof(*game.board)),
		                  .moving = WHITE };
	int rating;

	fen_to_chess("RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", &game);
	rating = rate_board(&game);
	TEST_ASSERT_EQUAL_INT(rating, 0);
	TEST_ASSERT_MESSAGE(game.rating == rating, "Rating not properly set.");

	char fen[] = " 7/8/8/8/8/8/8/8";
	for (enum COLOR c = BLACK; c <= WHITE; c += 2 ) {
		for (struct PIECE p = { PAWN, c }; p.type <= KING; ++(p.type)) {
			fen[0] = piece_to_chr(p);

			fen_to_chess(fen, &game);
			rating = rate_board(&game);

			TEST_ASSERT_EQUAL_INT(rating, c * PIECE_VALUES[p.type]);
			TEST_ASSERT_MESSAGE(game.rating == rating, "Rating not properly set.");
		}
	}
}

void
test_negamax()
{
}

void
test_choose_move()
{
}

void
test_bot()
{
	RUN_TEST(test_rate_board);
	RUN_TEST(test_negamax);
	RUN_TEST(test_choose_move);
}
