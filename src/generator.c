#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "types.h"

/*---------------------------------
 * Helpers for position validation
 * --------------------------------*/
// clang-format off
struct list* generate_moves_queen(struct PIECE board[], enum POS pos, bool check_checkless, bool hit_allies);
struct list* generate_moves_king(struct PIECE board[], enum POS pos, bool check_checkless, bool hit_allies);
struct list* generate_moves_rook(struct PIECE board[], enum POS pos, bool check_checkless, bool hit_allies);
struct list* generate_moves_knight(struct PIECE board[], enum POS pos, bool check_checkless, bool hit_allies);
struct list* generate_moves_pawn(struct PIECE board[], enum POS pos, bool check_checkless, bool hit_allies);
struct list* generate_moves_bishop(struct PIECE board[], enum POS pos, bool check_checkless, bool hit_allies);
// clang-format on

/**
 * @arg mate_move - the move that might hit the king.
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

	struct list* counter_moves =
			generate_moves_piece(game.board, mate_move->target, true, false);
	if (counter_moves->first) {
		// The king can still move out of check.
		list_free(counter_moves);
		return false;
	}
	list_free(counter_moves);

	// check all possible moves if they can prevent the checkmate
	// TODO: Add bitmask to check everything but the kings moves
	counter_moves = generate_moves(&game, false, false);
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
		struct list* moves = generate_moves_piece(game.board, mate_move->start,
		                                          false, false);
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

	new_moves = generate_moves_piece(new_board, move->target, false, false);
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
                          bool check_checkless, bool hit_allies)
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
				move->promotes_to = EMPTY;

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
                        bool check_checkless, bool hit_allies)
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
				move->promotes_to = EMPTY;

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
                           bool check_checkless, bool hit_allies)
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

		// diagonal
		if (i != 0)
			if (!occupied || !occupied_by_enemy) // not occupied by enemy
				continue;

		// straight
		if (occupied && !occupied_by_enemy && !hit_allies) // occupied by ally
			continue;

		/*
		 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
		 * be the last check!
		 */
		enum PIECE_E promotes_to = (target < 8 || target > 55) ? QUEEN : EMPTY;
		for (; promotes_to <= QUEEN && promotes_to != PAWN; promotes_to--) {
			struct move test_move = { pos, target, occupied_by_enemy,
				                      promotes_to };

			if (check_checkless && !is_checkless_move(board, &test_move))
				continue;

			// add move if it passed all tests
			struct move* move = malloc(sizeof(*move));
			if (!move)
				return NULL;
			move->start       = pos;
			move->target      = target;
			move->hit         = occupied_by_enemy;
			move->promotes_to = promotes_to;

			moves = list_push(moves, move);
		}
	}

	return moves;
}

struct move*
generate_moves_knight_helper(struct PIECE board[], enum POS pos,
                             enum POS target, bool check_checkless,
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
	move->promotes_to = EMPTY;

	return move;
}

/*-----------------------------
 * Piece based move generators
 * ----------------------------*/

struct list*
generate_moves_queen(struct PIECE board[], enum POS pos, bool check_checkless,
                     bool hit_allies)
{
	struct list* vertical_moves = generate_orthogonal_moves(
			board, pos, -1, check_checkless, hit_allies);
	struct list* diagonal_moves = generate_diagonal_moves(
			board, pos, -1, check_checkless, hit_allies);
	return list_append_list(vertical_moves, diagonal_moves);
}

struct list*
generate_moves_king(struct PIECE board[], enum POS pos, bool check_checkless,
                    bool hit_allies)
{
	struct list* vertical_moves = generate_orthogonal_moves(
			board, pos, 1, check_checkless, hit_allies);
	struct list* diagonal_moves =
			generate_diagonal_moves(board, pos, 1, check_checkless, hit_allies);

	struct list* all_moves = list_append_list(vertical_moves, diagonal_moves);
	if (!all_moves)
		return NULL;

	if (!all_moves->first)
		return all_moves;

	if (!check_checkless)
		return all_moves;

	// This array indicates if a position can be hit to keep the complexity at
	// O(2N) instead of O(N^2).
	// TODO: use bitboard and & with king moves bitboard
	bool targets[64] = { 0 };

	struct chess game = { .moving = !board[pos].color };
	memcpy(game.board, board, 64 * sizeof(*board));

	// Populate targets array
	struct list* possible_hit_moves = generate_moves(&game, false, true);
	while (possible_hit_moves->last) {
		struct move* cur_move     = list_pop(possible_hit_moves);
		targets[cur_move->target] = true;
		free(cur_move);
	}
	list_free(possible_hit_moves);

	// Remove all hittable fields.
	struct list_elem* cur = all_moves->first;
	while (cur) {
		struct move* cur_move = (struct move*)cur->object;
		if (targets[cur_move->target]) {
			// Remove this move from list
			if (cur->prev)
				cur->prev->next = cur->next;
			if (cur->next)
				cur->next->prev = cur->prev;
			if (all_moves->first == cur)
				all_moves->first = cur->next;
			if (all_moves->last == cur)
				all_moves->last = cur->prev;

			struct list_elem* tmp = cur->next;
			free(cur);
			cur = tmp;
			continue;
		}

		cur = cur->next;
	}

	return all_moves;
}

struct list*
generate_moves_rook(struct PIECE board[], enum POS pos, bool check_checkless,
                    bool hit_allies)
{
	return generate_orthogonal_moves(board, pos, -1, check_checkless,
	                                 hit_allies);
}

struct list*
generate_moves_knight(struct PIECE board[], enum POS pos, bool check_checkless,
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
generate_moves_pawn(struct PIECE board[], enum POS pos, bool check_checkless,
                    bool hit_allies)
{
	return generate_moves_pawn_helper(board, pos, check_checkless, hit_allies);
}

struct list*
generate_moves_bishop(struct PIECE board[], enum POS pos, bool check_checkless,
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
generate_moves_piece(struct PIECE board[], enum POS pos, bool check_checkless,
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
		struct list* possible_hit_moves = generate_moves(&game, false, false);
		// Undo move
		game.board[cur_move->start]  = game.board[cur_move->target];
		game.board[cur_move->target] = old;

		// Check if hitting moves target the king's field
		while (possible_hit_moves->last) {
			struct move* hit_move = list_pop(possible_hit_moves);
			if (hit_move->target == king_pos) {
				// Remove this move from list
				if (cur->prev)
					cur->prev->next = cur->next;
				if (cur->next)
					cur->next->prev = cur->prev;
				if (moves->first == cur)
					moves->first = cur->next;
				if (moves->last == cur)
					moves->last = cur->prev;

				free(hit_move);
				opens_king = true;

				struct list_elem* tmp = cur->next;
				free(cur);
				cur = tmp;
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
generate_moves(struct chess* game, bool check_checkless, bool hit_allies)
{
	struct list* moves  = calloc(1, sizeof(*moves));
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
