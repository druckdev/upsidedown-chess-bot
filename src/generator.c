#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <stdio.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "types.h"

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

/**
 * @arg range: Use this parameter to only calculate orthogonal moves up to a
 * certain range, i.e. the king who may only walk one tile. Use -1 for
 * "unlimited" range, meaning until the end of the board is reached.
 */
struct list*
generate_orthogonal_moves(struct PIECE board[], enum POS pos, int range)
{
	struct list* moves = NULL;
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
				else
					break; // occupied by ally
			}

			int target_col = target % 8;

			if (prev_target_col != target_col &&
			    prev_target_col - 1 != target_col &&
			    prev_target_col + 1 != target_col)
				break; // we must have wrapped around a border

			struct move* move = malloc(sizeof(struct move));
			// TODO(Aurel): Should we cleanup the list moves?
			if (!move)
				return NULL;
			move->start  = pos;
			move->target = target;

			moves = list_push(moves, move);
			if (!moves)
				return NULL;

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
generate_diagonal_moves(struct PIECE board[], enum POS pos, int range)
{
	if (range < -1) {
		fprintf(stderr, "Parameter `range` can't be lower than -1.\n");
		return NULL;
	}

	struct list* moves = NULL;
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
				else
					break; // occupied by ally
			}

			int target_col = target % 8;

			if (prev_target_col - 1 != target_col &&
			    prev_target_col + 1 != target_col)
				break; // we must have wrapped around the border

			struct move* move = malloc(sizeof(struct move));
			// TODO(Aurel): Should we cleanup the list moves?
			if (!move)
				return NULL;
			move->start  = pos;
			move->target = target;

			moves = list_push(moves, move);
			if (!moves)
				return NULL;

			prev_target     = target;
			prev_target_col = target_col;
			if (hit)
				break;
			++counter;
		}
	}
	return moves;
}

struct list*
generate_moves_queen(struct PIECE board[], enum POS pos)
{
	struct list* vertical_moves = generate_orthogonal_moves(board, pos, -1);
	struct list* diagonal_moves = generate_diagonal_moves(board, pos, -1);
	return list_append_list(vertical_moves, diagonal_moves);
}

struct list*
generate_moves_king(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_rook(struct PIECE board[], enum POS pos)
{
	return generate_orthogonal_moves(board, pos, -1);
}

struct list*
generate_moves_knight(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_pawn(struct PIECE* board[], enum POS pos)
{
}

struct list*
generate_moves_bishop(struct PIECE board[], enum POS pos)
{
	return generate_diagonal_moves(board, pos, -1);
}

struct list*
generate_moves(struct chess* game)
{
	struct list* moves  = NULL;
	struct PIECE* board = game->board;
	for (enum POS pos = 0; pos < 64; ++pos) {
		// if the `pos` is not occupied there are no moves to generate
		if (!is_occupied(board, pos))
			continue;

		// only the player moving can actually move pieces
		if (board[pos].color != game->moving)
			continue;

		// TODO(Aurel): Change the signatures of the generator functions. We
		// don't want to have to pass &board as that is just incorrect and won't
		// work. The calls should all look like that of the bishop.
		switch (board[pos].type) {
		case PAWN:
			moves = list_append_list(moves, generate_moves_pawn(&board, pos));
			break;
		case BISHOP:
			moves = list_append_list(moves, generate_moves_bishop(board, pos));
			break;
		case KNIGHT:
			moves = list_append_list(moves, generate_moves_knight(&board, pos));
			break;
		case ROOK:
			moves = list_append_list(moves, generate_moves_rook(board, pos));
			break;
		case QUEEN:
			moves = list_append_list(moves, generate_moves_queen(board, pos));
			break;
		case KING:
			moves = list_append_list(moves, generate_moves_king(&board, pos));
			break;
		default:
			fprintf(stderr, "Unexpected piece type %i (This should not be 0!)",
			        board[pos].type);
		}
	}
	return moves;
}

/*-------------------
 * Tests
 * ------------------*/

void
test_generate_moves()
{
	printf("\n\nTesting move generator for entire board...\n");

	struct chess chess = { 0 };
	chess.moving       = WHITE;

	struct PIECE queen    = { QUEEN, WHITE };
	struct PIECE b_bishop = { BISHOP, BLACK };
	struct PIECE w_bishop = { BISHOP, WHITE };
	chess.board[F4]       = queen;
	chess.board[C6]       = b_bishop;
	chess.board[F5]       = w_bishop;
	print_board(chess.board);

	struct list* moves = generate_moves(&chess);
	if (!moves)
		return;

	struct list_elem* cur = moves->first;
	printf("Moves:\n");
	while (cur) {
		struct move* move = (struct move*)cur->object;
		printf("%s -> %s\n", pos_to_str(move->start), pos_to_str(move->target));
		cur = cur->next;
	}
}

void
test_moves_queen()
{
	printf("\n\nTesting queen moves...\n");
	enum POS pos           = E4;
	struct PIECE board[64] = { 0 };
	struct PIECE queen     = { QUEEN, WHITE };
	struct PIECE b_bishop  = { BISHOP, BLACK };
	struct PIECE w_bishop  = { BISHOP, WHITE };
	board[pos]             = queen;
	board[C6]              = b_bishop;
	board[F5]              = w_bishop;
	print_board(board);

	struct list* moves = generate_moves_queen(board, pos);
	if (!moves)
		return;

	struct list_elem* cur = moves->first;
	printf("Moves:\n");
	while (cur) {
		struct move* move = (struct move*)cur->object;
		printf("%s -> %s\n", pos_to_str(move->start), pos_to_str(move->target));
		cur = cur->next;
	}
}

void
test_moves_bishop()
{
	printf("\n\nTesting bishop moves...\n");
	enum POS pos           = E4;
	struct PIECE board[64] = { 0 };
	struct PIECE bishop    = { BISHOP, WHITE };
	struct PIECE b_bishop  = { BISHOP, BLACK };
	struct PIECE w_bishop  = { BISHOP, WHITE };
	board[pos]             = bishop;
	board[C2]              = b_bishop;
	board[H7]              = w_bishop;
	print_board(board);

	struct list* moves = generate_moves_bishop(board, pos);
	//struct list* moves = generate_vertical_moves(board, pos, -1);
	if (!moves)
		return;

	struct list_elem* cur = moves->first;
	printf("Moves:\n");
	while (cur) {
		struct move* move = (struct move*)cur->object;
		printf("%s -> %s\n", pos_to_str(move->start), pos_to_str(move->target));
		cur = cur->next;
	}
}

void
test_moves_rook()
{
	printf("\n\nTesting rook moves...\n");
	enum POS pos           = E4;
	struct PIECE board[64] = { 0 };
	struct PIECE rook      = { ROOK, WHITE };
	struct PIECE b_bishop  = { BISHOP, BLACK };
	struct PIECE w_bishop  = { BISHOP, WHITE };
	board[pos]             = rook;
	board[E8]              = b_bishop;
	board[H4]              = w_bishop;
	print_board(board);

	struct list* moves = generate_moves_rook(board, pos);
	if (!moves)
		return;

	struct list_elem* cur = moves->first;
	printf("Moves:\n");
	while (cur) {
		struct move* move = (struct move*)cur->object;
		printf("%s -> %s\n", pos_to_str(move->start), pos_to_str(move->target));
		cur = cur->next;
	}
}

void
test_move_generator()
{
	test_generate_moves();
	test_moves_queen();
	test_moves_bishop();
	test_moves_rook();
}
