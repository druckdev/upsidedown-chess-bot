#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "types.h"

#define DEBUG_PRINTS true

/**
 * @arg board - the current board state
 * @arg mate_move - the move on `board` that has the king as target.
 * Returns: If board is a checkmate position or if the color of the attacked
 *          king can still get out of check.
 */
bool
is_checkmate(struct PIECE board[], struct move* mate_move)
{
	if (board[mate_move->target].type != KING &&
	    board[mate_move->start].color != board[mate_move->target].color) {
		return false;
	}

	struct chess game = { .moving = !board[mate_move->start].color };
	memcpy(game.board, board, 64 * sizeof(*board));

	// Generate moves for the king
	struct list* counter_moves =
			generate_moves_piece(game.board, mate_move->target, 1, false);
	if (counter_moves->first) {
		// The king can still move out of check.
		list_free(counter_moves);
		return false;
	}
	list_free(counter_moves);

	// check all possible moves if they can prevent the checkmate
	// TODO: Add bitmask to check everything but the kings moves
	counter_moves = generate_moves(&game, 0, false);
	while (counter_moves->last) {
		struct move* cur_counter_move = (struct move*)list_pop(counter_moves);

		if (cur_counter_move->start == mate_move->target) {
			// Already checked the king moves as they need check_checkless
			free(cur_counter_move);
			continue;
		}

		if (cur_counter_move->target == mate_move->start) {
			// The counter_move hits the attacking piece
			free(cur_counter_move);
			list_free(counter_moves);
			return false;
		}

		// Backup piece for undo
		struct PIECE old = game.board[cur_counter_move->target];
		execute_move(game.board, cur_counter_move);

		// Check if mate_move is still doable or was declined
		struct list* moves =
				generate_moves_piece(game.board, mate_move->start, 0, false);
		bool mate_declined = true;
		while (moves->last) {
			struct move* cur_move = (struct move*)list_pop(moves);
			if (cur_move->target == mate_move->target) {
				// mate_move was not declined, try the next counter_move
				free(cur_move);
				mate_declined = false;
				break;
			}

			free(cur_move);
		}
		list_free(moves);

		if (mate_declined) {
			// counter_move successful
			free(cur_counter_move);
			list_free(counter_moves);
			return false;
		}

		// Undo move
		board[cur_counter_move->start]  = board[cur_counter_move->target];
		board[cur_counter_move->target] = old;

		free(cur_counter_move);
	}

	list_free(counter_moves);
	return true;
}

/**
 * Execute move and check if the same piece from the new positions can hit the
 * opponent's king.
 */
bool
is_checkless_move(struct PIECE board[], struct move* move)
{
	struct PIECE new_board[64];
	memcpy(new_board, board, 64 * sizeof(*board));

	assert(execute_move(new_board, move));

	struct list* new_moves;

	new_moves = generate_moves_piece(new_board, move->target, 0, false);
	assert(new_moves);

	while (new_moves->last) {
		struct move* cur_move = (struct move*)list_pop(new_moves);
		if (!cur_move->hit) {
			free(cur_move);
			continue;
		}

		if (new_board[cur_move->target].type == KING &&
		    !is_checkmate(new_board, cur_move)) {
			// Found checkless move
			free(cur_move);
			list_free(new_moves);
			return false;
		}
		free(cur_move);
	}
	list_free(new_moves);

	return true;
}

bool
is_valid_pos(enum POS pos)
{
	if (pos >= 0 && pos < 64)
		return true;
	return false;
}

bool
is_occupied(struct PIECE board[], enum POS target)
{
	return board[target].type != EMPTY;
}

/**
 * @return true if the specified target is occupied by the enemy.
 * NOTE: If this function returns false, that might mean that target is not
 * occupied at all or that it's occupied by an ally.
 */
bool
is_occupied_by_enemy(struct PIECE board[], enum POS pos, enum POS target)
{
	if (is_occupied(board, target) && board[pos].color != board[target].color)
		return true;
	return false;
}

/*-----------------------------
 * Helpers for move generation
 * ----------------------------*/

/**
 * @arg range: Use this parameter to only calculate orthogonal moves up to a
 * certain range, i.e. the king who may only walk one tile. Use -1 for
 * "unlimited" range, meaning until the end of the board is reached.
 */
struct list*
generate_orthogonal_moves(struct PIECE board[], enum POS pos, int range,
                          int check_checkless, bool hit_allies)
{
	struct list* moves = calloc(1, sizeof(*moves));
	int offsets[]      = { +1, -1, +8, -8 };

	bool hit;
	for (int i = 0; i < 4; ++i) {
		enum POS prev_target = pos;
		int prev_target_col  = pos % 8;
		hit                  = false;

		int counter = 0;
		while (range == -1 || counter < range) {
			enum POS target = prev_target + offsets[i];

			if (!is_valid_pos(target))
				break;

			if (is_occupied(board, target)) {
				if (is_occupied_by_enemy(board, pos, target))
					hit = true; // in this move we will hit an enemy position
				else if (!hit_allies)
					break; // occupied by ally
			}

			int target_col = target % 8;

			if (prev_target_col != target_col &&
			    prev_target_col - 1 != target_col &&
			    prev_target_col + 1 != target_col)
				break; // we must have wrapped around a border

			/*
			 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
			 * be the last check!
			 */
			struct move test_move = { pos, target, hit, EMPTY };
			if (!check_checkless || is_checkless_move(board, &test_move)) {
				// Move passed all tests

				struct move* move = malloc(sizeof(*move));
				// TODO(Aurel): Should we cleanup the list moves?
				if (!move)
					return NULL;
				move->start       = pos;
				move->target      = target;
				move->hit         = hit;
				move->promotes_to = empty_piece;

				moves = list_push(moves, move);
				if (!moves)
					return NULL;
			}

			prev_target     = target;
			prev_target_col = target_col;
			if (hit)
				break;
			++counter;
		}
	}
	return moves;
}

/**
 * @arg range: Use this parameter to only calculate diagonal moves up to a
 * certain range, i.e. the king who may only walk one tile. Use -1 for
 * "unlimited" range, meaning until the end of the board is reached.
 */
struct list*
generate_diagonal_moves(struct PIECE board[], enum POS pos, int range,
                        int check_checkless, bool hit_allies)
{
	if (range < -1) {
		fprintf(stderr, "Parameter `range` can't be lower than -1.\n");
		return NULL;
	}

	struct list* moves = calloc(1, sizeof(*moves));
	int offsets[]      = { 7, 9, -7, -9 };

	bool hit;
	for (int i = 0; i < 4; ++i) {
		enum POS prev_target = pos;
		int prev_target_col  = pos % 8;
		hit                  = false;

		int counter = 0;
		while (range == -1 || counter < range) {
			enum POS target = prev_target + offsets[i];

			if (!is_valid_pos(target))
				break;

			if (board[target].type != EMPTY) {
				if (is_occupied_by_enemy(board, pos, target))
					hit = true;
				else if (!hit_allies)
					break; // occupied by ally
			}

			int target_col = target % 8;

			if (prev_target_col - 1 != target_col &&
			    prev_target_col + 1 != target_col)
				break; // we must have wrapped around the border

			/*
			 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
			 * be the last check!
			 */
			struct move test_move = { pos, target, hit, EMPTY };
			if (!check_checkless || is_checkless_move(board, &test_move)) {
				// Move passed all tests

				struct move* move = malloc(sizeof(*move));
				// TODO(Aurel): Should we cleanup the list moves?
				if (!move)
					return NULL;
				move->start       = pos;
				move->target      = target;
				move->hit         = hit;
				move->promotes_to = empty_piece;

				moves = list_push(moves, move);
				if (!moves)
					return NULL;
			}

			prev_target     = target;
			prev_target_col = target_col;
			if (hit)
				break;
			++counter;
		}
	}
	return moves;
}

/**
 * @arg factor: Use this parameter to decide whther to subtract (factor = -1) or add
 * (factor = 1) values.
 */
struct list*
generate_moves_pawn_helper(struct PIECE board[], enum POS pos,
                           int check_checkless, bool hit_allies)
{
	struct list* moves = calloc(1, sizeof(*moves));

	int factor = board[pos].color == WHITE ? -1 : 1;

	for (int i = -1; i <= 1; i++) {
		// value the start position needs to be modified by
		int move_by = (WIDTH + i) * factor;

		int target = pos + move_by;

		// valid y pos
		if (!is_valid_pos(target))
			continue;

		// valid x, rows should not vary by more than one, on wrap around this is more
		int start_col  = pos % 8;
		int target_col = target % 8;
		bool valid_x = start_col == target_col || start_col - 1 == target_col ||
					   start_col + 1 == target_col;

		if (!valid_x)
			continue;

		// valid by pawn rules
		bool occupied          = is_occupied(board, target);
		bool occupied_by_enemy = is_occupied_by_enemy(board, pos, target);

		// NOTE: We cannot use `i != occupied` to combine the two statements, as
		// you could do in normal boolean algebra, as we have -1 as possible
		// value that acts as true as well as 1
		if (i && !occupied || !i && occupied)
			// diagonally not occupied or vertically blocked
			continue;

		// Skip if there is an ally blocking and we are not building `targets`
		if (occupied && !occupied_by_enemy && !hit_allies)
			continue;

		// We do not want to add the step forward, if we are building a
		// `targets` map, as we are not able to hit in that direction and thus
		// it is safe to move there.
		if (!i && hit_allies)
			continue;

		/*
		 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
		 * be the last check!
		 */
		struct PIECE promotes_to = { EMPTY, board[pos].color };
		if (target < 8 || target > 55)
			promotes_to.type = QUEEN;
		while (promotes_to.type <= QUEEN && promotes_to.type != PAWN) {
			struct move test = { pos, target, occupied_by_enemy && i,
				                 promotes_to };
			promotes_to.type--;

			if (check_checkless && !is_checkless_move(board, &test)) {
				continue;
			}

			// add move if it passed all tests
			struct move* move = malloc(sizeof(*move));
			if (!move)
				return NULL;
			memcpy(move, &test, sizeof(test));

			moves = list_push(moves, move);

			// When hit_allies is set, we are building the `targets` map. In that
			// case it is completely irrelevant to which piece we could promote
			// and can break here to only 'record' the target.
			if (hit_allies)
				break;
		}
	}

	return moves;
}

struct move*
generate_moves_knight_helper(struct PIECE board[], enum POS pos,
                             enum POS target, int check_checkless,
                             bool hit_allies)
{
	if (!is_valid_pos(target))
		return NULL;

	bool occupied_by_enemy = is_occupied_by_enemy(board, pos, target);
	bool occupied_by_ally  = is_occupied(board, target) && !occupied_by_enemy;

	// rows should not vary by more than two, on wrap around this is more
	int start_col  = pos % 8;
	int target_col = target % 8;

	if (!(start_col - 1 == target_col || start_col + 1 == target_col ||
	      start_col - 2 == target_col || start_col + 2 == target_col))
		return NULL;

	if (occupied_by_ally && !hit_allies)
		return NULL;

	/*
	 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
	 * be the last check!
	 */
	struct move test_move = { pos, target, occupied_by_enemy, EMPTY };
	if (check_checkless && !is_checkless_move(board, &test_move))
		return NULL;

	struct move* move = malloc(sizeof(*move));
	if (!move)
		return NULL;
	move->start       = pos;
	move->target      = target;
	move->hit         = occupied_by_enemy;
	move->promotes_to = empty_piece;

	return move;
}

/*-----------------------------
 * Piece based move generators
 * ----------------------------*/

struct list*
generate_moves_queen(struct PIECE board[], enum POS pos, int check_checkless,
                     bool hit_allies)
{
	struct list* vertical_moves = generate_orthogonal_moves(
			board, pos, -1, check_checkless, hit_allies);
	struct list* diagonal_moves = generate_diagonal_moves(
			board, pos, -1, check_checkless, hit_allies);
	return list_append_list(vertical_moves, diagonal_moves);
}

struct list*
generate_moves_king(struct PIECE board[], enum POS pos, int check_checkless,
                    bool hit_allies)
{
	struct list* vertical_moves =
			generate_orthogonal_moves(board, pos, 1, 0, hit_allies);
	struct list* diagonal_moves =
			generate_diagonal_moves(board, pos, 1, 0, hit_allies);

	struct list* all_moves = list_append_list(vertical_moves, diagonal_moves);

	if (!all_moves)
		return NULL;

	if (!all_moves->first)
		return all_moves;

	if (!check_checkless)
		return all_moves;

	struct chess game = { .moving = !board[pos].color };
	memcpy(game.board, board, 64 * sizeof(*board));

	struct list* possible_hit_moves = generate_moves(&game, 0, true);
	// TODO: use bitboard and & with king moves bitboard
	bool targets[64] = { 0 };
	are_attacked(possible_hit_moves, targets);

	// Remove all hittable fields.
	struct list_elem* cur = all_moves->first;
	while (cur) {
		struct move* cur_move = (struct move*)cur->object;
		if (targets[cur_move->target]) {
			cur = list_remove(all_moves, cur);
			continue;
		}

		cur = cur->next;
	}

	return all_moves;
}

struct list*
generate_moves_rook(struct PIECE board[], enum POS pos, int check_checkless,
                    bool hit_allies)
{
	return generate_orthogonal_moves(board, pos, -1, check_checkless,
	                                 hit_allies);
}

struct list*
generate_moves_knight(struct PIECE board[], enum POS pos, int check_checkless,
                      bool hit_allies)
{
	struct list* moves = calloc(1, sizeof(*moves));
	int offsets[]      = { 6, 10, 15, 17 };
	for (int i = 0; i < 4; i++) {
		// downwards
		list_push(moves,
		          generate_moves_knight_helper(board, pos, pos + offsets[i],
		                                       check_checkless, hit_allies));
		// upwards
		list_push(moves,
		          generate_moves_knight_helper(board, pos, pos - offsets[i],
		                                       check_checkless, hit_allies));
	}

	return moves;
}

struct list*
generate_moves_pawn(struct PIECE board[], enum POS pos, int check_checkless,
                    bool hit_allies)
{
	return generate_moves_pawn_helper(board, pos, check_checkless, hit_allies);
}

struct list*
generate_moves_bishop(struct PIECE board[], enum POS pos, int check_checkless,
                      bool hit_allies)
{
	return generate_diagonal_moves(board, pos, -1, check_checkless, hit_allies);
}

enum POS
get_king_pos(struct PIECE board[], enum COLOR c)
{
	for (enum POS i = 0; i < 64; i++) {
		if (board[i].type == KING && board[i].color == c)
			return i;
	}

	return 64;
}

/*-------------------
 * Inteface
 * ------------------*/

struct list*
generate_moves_piece(struct PIECE board[], enum POS pos, int check_checkless,
                     bool hit_allies)
{
	struct list* moves;
	// clang-format off
	switch (board[pos].type) {
	case QUEEN:  moves = generate_moves_queen (board, pos, check_checkless, hit_allies); break;
	case KING:   moves = generate_moves_king  (board, pos, check_checkless, hit_allies); break;
	case ROOK:   moves = generate_moves_rook  (board, pos, check_checkless, hit_allies); break;
	case KNIGHT: moves = generate_moves_knight(board, pos, check_checkless, hit_allies); break;
	case PAWN:   moves = generate_moves_pawn  (board, pos, check_checkless, hit_allies); break;
	case BISHOP: moves = generate_moves_bishop(board, pos, check_checkless, hit_allies); break;
	default:
		printf("Invalid piece at %i: %i\n", pos, board[pos].type);
		assert(false); return NULL;
	}
	// clang-format on

	if (!moves)
		return NULL;

	if (!check_checkless)
		return moves;

	enum POS king_pos = get_king_pos(board, board[pos].color);
	if (king_pos == 64 || king_pos == pos)
		// No king, or not needed as we moved the king itself.
		return moves;

	struct chess game = { .moving = !board[pos].color };
	memcpy(game.board, board, 64 * sizeof(*board));

	// Remove all moves that leave the king hittable.
	struct list_elem* cur = moves->first;
	while (cur) {
		struct move* cur_move = (struct move*)cur->object;
		bool opens_king       = false;

		// Backup piece for undo
		struct PIECE old = board[cur_move->target];
		execute_move(game.board, cur_move);

		// TODO: use bitboard
		bool targets[64] = { 0 };

		// Populate targets array
		struct list* possible_hit_moves = generate_moves(&game, 0, false);
		// Undo move
		game.board[cur_move->start]  = game.board[cur_move->target];
		game.board[cur_move->target] = old;

		// Check if hitting moves target the king's field
		while (possible_hit_moves->last) {
			struct move* hit_move = list_pop(possible_hit_moves);
			if (hit_move->target == king_pos) {
				cur        = list_remove(moves, cur);
				opens_king = true;
				break;
			}

			free(hit_move);
		}
		list_free(possible_hit_moves);

		if (opens_king)
			continue;

		cur = cur->next;
	}

	return moves;
}

struct list*
generate_moves(struct chess* game, int check_checkless, bool hit_allies)
{
	struct list* moves = calloc(1, sizeof(*moves));

	struct PIECE* board = game->board;
	for (enum POS pos = 0; pos < 64; ++pos) {
		// if the `pos` is not occupied there are no moves to generate
		if (!is_occupied(board, pos))
			continue;

		// only the player moving can actually move pieces
		if (board[pos].color != game->moving)
			continue;

		moves = list_append_list(moves, generate_moves_piece(board, pos,
		                                                     check_checkless,
		                                                     hit_allies));
	}
	return moves;
}

#ifdef BUILD_TEST
#include "unity.h"

#define test_generated_moves_count(fen, cur_moving_player, expected_move_cnt)  \
	{                                                                          \
		printf("\nTEST : %s \n", fen);                                         \
		struct chess chess;                                                    \
		chess.moving = cur_moving_player;                                      \
		board_from_fen(fen, chess.board);                                      \
                                                                               \
		struct list* list = generate_moves(&chess, 1, false);                  \
		int list_length   = list_count(list);                                  \
                                                                               \
		print_board(chess.board, list);                                        \
		printf("Expected:\t%d\nGot:\t\t%d\n", expected_move_cnt, list_length); \
		TEST_ASSERT(list_length == expected_move_cnt);                         \
                                                                               \
		printf("-------------------------------------------\n");               \
	}

void
test_basic_king_move_generation()
{
	test_generated_moves_count("8/8/8/8/8/3k4/8/8", BLACK, 8);
	test_generated_moves_count("k7/8/8/8/8/8/8/8", BLACK, 3);
}

void
test_basic_rook_move_generation()
{
	test_generated_moves_count("8/8/8/8/3r4/8/8/8", BLACK, 14);
}

void
test_basic_bishop_move_generation()
{
	test_generated_moves_count("8/8/8/8/3b4/8/8/8", BLACK, 13);
}

void
test_basic_black_pawn_move_generation()
{
	test_generated_moves_count("8/8/8/8/8/8/6p1/8", BLACK, 4);
	test_generated_moves_count("8/8/8/8/8/6p1/8/8", BLACK, 1);
}

void
test_basic_white_pawn_move_generation()
{
	test_generated_moves_count("8/6P1/8/8/8/8/8/8", WHITE, 4);
	test_generated_moves_count("8/8/6P1/8/8/8/8/8", WHITE, 1);
}

void
test_basic_queen_move_generation()
{
	test_generated_moves_count("8/8/8/8/3q4/8/8/8", BLACK, 27);
}

void
test_basic_knight_move_generation()
{
	test_generated_moves_count("8/8/8/8/3n4/8/8/8", BLACK, 8);
}

void
test_start_board_move_generation()
{
	// clang-format off
	test_generated_moves_count("RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", WHITE, 4);
	test_generated_moves_count("RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", BLACK, 4);
	// clang-format on
}

void
test_community_boards_move_generation()
{
	// clang-format off
	test_generated_moves_count("RNBKQB1R/PPPPPPPP/8/8/4N3/2n5/pppppppp/r1bkqbnr", BLACK, 12);
	test_generated_moves_count("RqBQKB1R/P1PPPPQP/2N2N2/8/8/4n3/p1pppppp/r1bqkbnr", WHITE, 29);
	test_generated_moves_count("RNBQKB1R/PPPPPPPP/5N2/8/8/5n2/pppppppp/rnbqkb1r", WHITE, 12);
	test_generated_moves_count("RNBQKBQR/PPPPPP1P/5N2/8/8/5n2/pppppp1p/rnbqkbqr", WHITE, 15);
	test_generated_moves_count("RQBQKB1R/P1PPPPPP/2N2N2/8/5n2/n7/pppppppp/r1bqkb1r", WHITE, 22);
	test_generated_moves_count("RNBQK2R/PPPPPPBP/7N/3N4/8/n1nn1n2/pbqppppp/r3kb1r", BLACK, 38);
	test_generated_moves_count("RNBKQB1R/PPPPPPPP/8/3N4/8/5n2/pppppp1p/rnbkqbrr", WHITE, 12);
	test_generated_moves_count("qRQQK1QR/3PPP1P/5N1B/4Q3/1b6/1Bn5/3ppp1p/1rqqkbqr", WHITE, 61);
	test_generated_moves_count("4K2R/4P2P/8/q1N5/8/6B1/p1k3q1/r1bR4", WHITE, 32);
	test_generated_moves_count("RNQQKB1r/PP1PPPP1/8/8/8/8/pp1ppp2/r1qkb1q1", WHITE, 18);
	test_generated_moves_count("3QK3/4Q1P1/8/8/8/1rq1n3/2n5/2k5", WHITE, 33);
	test_generated_moves_count("3R4/3R4/8/7K/7B/8/3r2n1/3k4", BLACK, 10);
	test_generated_moves_count("6KR/8/1N6/8/8/3q4/4b3/6kr", WHITE, 15);

	test_generated_moves_count("R1QQKBNR/PP1PPPPP/2N5/3b1B2/4q3/2n1n3/pppppppp/r1b1k2r", WHITE, 22);
	// clang-format on
}

void
test_checkmate_move_generation()
{
	test_generated_moves_count("K7/2r5/1q6/8/8/8/8/8", BLACK, 31);
	test_generated_moves_count("8/8/8/8/8/8/8/3K1k2", WHITE, 3);
	test_generated_moves_count("8/8/8/8/8/4R3/8/3K1k2", WHITE, 15);
}

void
test_pawn_promotion_move_generation()
{
	test_generated_moves_count("8/8/8/8/8/8/2p3p1/3k4", BLACK, 12);
	test_generated_moves_count("3k4/4P3/8/2Q5/3R4/8/8/8", BLACK, 1);
	test_generated_moves_count("8/8/8/8/8/8/3p4/2RRR3", BLACK, 8);
}

void
test_generator()
{
	// basic movement
	RUN_TEST(test_basic_king_move_generation);
	RUN_TEST(test_basic_rook_move_generation);
	RUN_TEST(test_basic_bishop_move_generation);
	RUN_TEST(test_basic_black_pawn_move_generation);
	RUN_TEST(test_basic_white_pawn_move_generation);
	RUN_TEST(test_basic_queen_move_generation);
	RUN_TEST(test_basic_knight_move_generation);

	RUN_TEST(test_start_board_move_generation);

	RUN_TEST(test_community_boards_move_generation);

	// features
	RUN_TEST(test_checkmate_move_generation);
	RUN_TEST(test_pawn_promotion_move_generation);
}
#endif /* BUILD_TEST */

#ifdef BUILD_BENCHMARK
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "benchmark.h"

void
benchmark_generator()
{
	struct game_samples {
		char* fen;
		enum COLOR moving;
		int move_cnt;
	};
	// clang-format off
	struct game_samples test_boards[] = {
		// Test single figures
		{ "8/8/8/8/3n4/8/8/8", BLACK, 8 }, // test knight

		// Test start board
		{ "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", WHITE, 4 },
		{ "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", BLACK, 4 },

		// Test different boards
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
		{ "3R4/3R4/8/7K/7B/8/3r2n1/3k4", BLACK, 10 },
		{ "6KR/8/1N6/8/8/3q4/4b3/6kr", WHITE, 15 },

		{ "R1QQKBNR/PP1PPPPP/2N5/3b1B2/4q3/2n1n3/pppppppp/r1b1k2r", WHITE, 22 },

		{ "K7/2r5/1q6/8/8/8/8/8", BLACK, 31 }, // check checkmate
		{ "8/8/8/8/8/8/8/3K1k2", WHITE, 3 },   // check weird king interaction 1
		{ "8/8/8/8/8/4R3/8/3K1k2", WHITE, 15 }, // check weird king interaction 2

		{ "8/8/8/8/8/8/2p3p1/3k4", BLACK, 12 }, // Check correct pawn promotion
		{ "3k4/4P3/8/2Q5/3R4/8/8/8", BLACK, 1 }, // Check correct `targets` buildup
		{ "8/8/8/8/8/8/3p4/2RRR3", BLACK, 8 }, // Check that pawn hits all fields

		/* Invalid boards
		{ "r1bqkb1r/pppppppp/2n2n2/8/8/2N2N2/PPPPPPPP/R1BQKB1R", WHITE, 36 },
		{ "r1bqkbqr/pppppp1p/2n2n2/8/8/2N2N2/P1PPPPPP/RQBQKB1R", WHITE, 38 },
		{ "r1bqk1nr/pppppp1p/2k4b/8/8/2N2N2/P1PPPP1P/RQBQKB1R", WHITE, 36 },
		{ "r1bqkqnr/ppppp2p/2n4b/3N4/8/5N2/P1PPPP1P/RQBQKB1R", WHITE, 42 },
		*/
	};
	// clang-format on
	printf("Benchmarking function generate_moves()...\n");

	size_t len = sizeof(test_boards) / sizeof(*test_boards);

	double* cpu_secs[len];
	size_t* cpu_nsecs[len];
	double* wall_secs[len];
	size_t* wall_nsecs[len];

	for (size_t i = 0; i < len; ++i) {
		printf("BENCHMARK: %s \n", test_boards[i].fen);
		// Reserve for average too
		cpu_secs[i]   = calloc((N_FOR_AVG + 1), sizeof(**cpu_secs));
		cpu_nsecs[i]  = calloc((N_FOR_AVG + 1), sizeof(**cpu_nsecs));
		wall_secs[i]  = calloc((N_FOR_AVG + 1), sizeof(**wall_secs));
		wall_nsecs[i] = calloc((N_FOR_AVG + 1), sizeof(**wall_nsecs));

		// Init game
		struct chess chess;
		chess.moving = test_boards[i].moving;
		board_from_fen(test_boards[i].fen, chess.board);

		print_board(chess.board, NULL);

		struct list* moves;
		struct timespec t_start_cpu, t_end_cpu, t_start_wall, t_end_wall;

		for (size_t j = 0; j < N_FOR_AVG; ++j) {
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu); // CPU time
			clock_gettime(CLOCK_MONOTONIC, &t_start_wall); // "actual" time

			for (size_t k = 0; k < ITERATIONS; ++k) {
				/* functions to benchmark */

				moves = generate_moves(&chess, 1, false);

				/* \functions to benchmark */
			}

			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end_cpu);
			clock_gettime(CLOCK_MONOTONIC, &t_end_wall);

			double cpu_sec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) +
							 (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec) * 1e-9;
			size_t cpu_nsec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) * 1e9 +
							  (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec);
			cpu_secs[i][j]  = cpu_sec;
			cpu_nsecs[i][j] = cpu_nsec;
			cpu_secs[i][N_FOR_AVG] += cpu_sec;
			cpu_nsecs[i][N_FOR_AVG] += cpu_nsec;

			double wall_sec =
					(t_end_wall.tv_sec - t_start_wall.tv_sec) +
					(t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
			size_t wall_nsec = (t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
							   (t_end_wall.tv_nsec - t_start_wall.tv_nsec);
			wall_secs[i][j]  = wall_sec;
			wall_nsecs[i][j] = wall_nsec;
			wall_secs[i][N_FOR_AVG] += wall_sec;
			wall_nsecs[i][N_FOR_AVG] += wall_nsec;

			printf("Generated moves: %li\n", moves->count);
			printf("Elapsed CPU-time over %i iterations:  %lf sec, %li nsec\n",
			       ITERATIONS, cpu_sec, cpu_nsec);
			printf("Elapsed wall-time over %i iterations: %lf sec, %li nsec\n",
			       ITERATIONS, wall_sec, wall_nsec);
		}

		cpu_secs[i][N_FOR_AVG] /= N_FOR_AVG;
		cpu_nsecs[i][N_FOR_AVG] /= N_FOR_AVG;
		wall_secs[i][N_FOR_AVG] /= N_FOR_AVG;
		wall_nsecs[i][N_FOR_AVG] /= N_FOR_AVG;

		printf("Average CPU-time (%ix):  %lf sec, %li nsec\n", N_FOR_AVG,
		       cpu_secs[i][N_FOR_AVG], cpu_nsecs[i][N_FOR_AVG]);
		printf("Average wall-time (%ix): %lf sec, %li nsec\n", N_FOR_AVG,
		       wall_secs[i][N_FOR_AVG], wall_nsecs[i][N_FOR_AVG]);

		printf("\n-------------------------------------------\n\n");
	}

	printf("Summary:\n");
	fprintf(CSV_STREAM, "FEN;cpu secs;cpu nsecs;wall secs; wall nsecs\n");
	for (size_t i = 0; i < len; ++i) {
		fprintf(CSV_STREAM, "%s;%lf;%li;%lf;%li\n", test_boards[i].fen,
		        cpu_secs[i][N_FOR_AVG], cpu_nsecs[i][N_FOR_AVG],
		        wall_secs[i][N_FOR_AVG], wall_nsecs[i][N_FOR_AVG]);

		free(cpu_secs[i]);
		free(cpu_nsecs[i]);
		free(wall_secs[i]);
		free(wall_nsecs[i]);
	}
}
#endif /* BUILD_BENCHMARK */
