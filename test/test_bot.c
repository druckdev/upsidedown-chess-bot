#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "chess.h"
#include "devel_bot.h"
#include "unity.h"

#define TEST_ASSERT_EQUAL_MOVE(expected, move)                                 \
	TEST_ASSERT_EQUAL_INT((expected)->start, (best)->start);                   \
	TEST_ASSERT_EQUAL_INT((expected)->target, (best)->target);                 \
	TEST_ASSERT_EQUAL_CHAR((expected)->hit, (best)->hit);                      \
	TEST_ASSERT_EQUAL_CHAR((expected)->is_checkmate, (best)->is_checkmate);    \
	TEST_ASSERT_EQUAL_INT((expected)->promotes_to.type,                        \
	                      (best)->promotes_to.type);                           \
	TEST_ASSERT_EQUAL_INT((expected)->promotes_to.color,                       \
	                      (best)->promotes_to.color)

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
	for (enum color c = BLACK; c <= WHITE; c += 2) {
		for (struct piece p = { PAWN, c }; p.type <= KING; ++(p.type)) {
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
	struct chess game = init_chess(' ', 9999, 99);
	fen_to_chess("R1BQKBNR/PPPPPPPP/N7/8/8/8/pppppppp/rnbqkbnr w", &game);

	struct negamax_return ret = negamax(&game, 1, INT_MIN + 1, INT_MAX);
	struct move* best;
#ifdef DEBUG_NEGAMAX_USE_LIST
	best = move_list_pop(ret.moves);
	move_list_free(ret.moves);
#else
	best = ret.move;
#endif

	// clang-format off
	struct move expected = {
		.start        = B7,
		.target       = B8,
		.hit          = false,
		.is_checkmate = false,
		.promotes_to.type = QUEEN,
		.promotes_to.color = WHITE
	};
#if 0 // TODO(Aurel): only here because of the bug description - (re)move it
	struct move expected = {
		.start        = A6,
		.target       = B4,
		.hit          = false,
		.is_checkmate = false,
		.promotes_to.type = EMPTY,
		// TODO(Aurel): is 1 for some reason. Should be UNDEFINED (0).
		// Check this out on another branch.
		.promotes_to.color = 1
	};
#endif

	// Only works in combination with `__attribute__((packed))` as there will be
	// random (when allocated with malloc or on stack) padding bytes if the
	// struct size is not dividable by 4.
	// TEST_ASSERT_EQUAL_MEMORY(&expected, best, sizeof(expected));
	TEST_ASSERT_EQUAL_MOVE(&expected, best);
	// clang-format on

	free(best);
	free(game.board);
	free_ht(&game.trans_table);
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
