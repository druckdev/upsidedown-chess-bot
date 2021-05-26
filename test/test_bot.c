#include "board.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "chess.h"
#include "devel_bot.h"
#include "unity.h"

void
test_rate_board()
{
	printf("TEST: rate_board\t");

	struct chess game = { .board  = calloc(64, sizeof(*game.board)),
		                  .moving = WHITE };
	int rating;

	fen_to_chess("RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", &game);
	rating = rate_board(&game);
	TEST_ASSERT_EQUAL_INT(rating, 0);
	TEST_ASSERT_MESSAGE(game.rating == rating, "Rating not properly set.");

	char fen[] = " 7/8/8/8/8/8/8/8";
	for (enum COLOR c = BLACK; c <= WHITE; c += 2) {
		for (struct PIECE p = { PAWN, c }; p.type <= KING; ++(p.type)) {
			fen[0] = piece_to_chr(p);

			fen_to_chess(fen, &game);
			rating = rate_board(&game);

			if (rating != c * PIECE_VALUES[p.type] || game.rating != rating)
				free(game.board);

			TEST_ASSERT_EQUAL_INT(rating, c * PIECE_VALUES[p.type]);
			TEST_ASSERT_MESSAGE(game.rating == rating,
			                    "Rating not properly set.");
		}
	}

	free(game.board);
}

void
test_negamax()
{
	printf("TEST: negamax\t");

	struct chess game = { .board  = calloc(64, sizeof(*game.board)),
		                  .moving = WHITE };
	fen_to_chess("R1BQKBNR/PPPPPPPP/N7/8/8/8/pppppppp/rnbqkbnr", &game);

	struct negamax_return ret = negamax(&game, 1, INT_MIN + 1, INT_MAX);

	// clang-format off
	struct move expected = { .start        = B7,
	                         .target       = B8,
	                         .hit          = false,
	                         .is_checkmate = false,
	                         .promotes_to.type = QUEEN,
	                         .promotes_to.color = WHITE
	};

	// Only works in combination with `__attribute__((packed))` as there will be
	// random (when allocated with malloc or on stack) padding bytes if the
	// struct size is not dividable by 4.
	// TEST_ASSERT_EQUAL_MEMORY(&expected, ret.move, sizeof(expected));
	TEST_ASSERT_EQUAL_INT(expected.start, ret.move->start);
	TEST_ASSERT_EQUAL_INT(expected.target, ret.move->target);
	TEST_ASSERT_EQUAL_CHAR(expected.hit, ret.move->hit);
	TEST_ASSERT_EQUAL_CHAR(expected.is_checkmate, ret.move->is_checkmate);
	TEST_ASSERT_EQUAL_INT(expected.promotes_to.type, ret.move->promotes_to.type);
	TEST_ASSERT_EQUAL_INT(expected.promotes_to.color, ret.move->promotes_to.color);
	// clang-format on

	free(ret.move);
	free(game.board);
}

void
test_choose_move()
{
	printf("TEST: choose_move\t");
}

void
test_bot()
{
	RUN_TEST(test_rate_board);
	RUN_TEST(test_negamax);
	RUN_TEST(test_choose_move);
}
