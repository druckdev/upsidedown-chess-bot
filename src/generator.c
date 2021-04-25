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
generate_moves(struct chess* game)
{
	// TODO(Aurel): Stub. Fill this with code.
	assert(("Not implemented yet", 0 != 0));
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
