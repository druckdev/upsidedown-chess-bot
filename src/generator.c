#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "move.h"

struct move_list* generate_moves_piece(struct chess* game, enum pos pos,
                                       bool check_checkless, bool hit_allies);

/**
 * @arg board - the current board state
 * @arg mate_move - the move on `board` that has the king as target.
 * @return If board is a checkmate position or if the color of the attacked king
 *         can still get out of check.
 */
bool
is_checkmate(struct piece board[], struct move* mate_move)
{
	if (board[mate_move->target].type != KING &&
	    board[mate_move->start].color != board[mate_move->target].color) {
		return false;
	}

	struct chess game = {
		.moving = -1 * board[mate_move->start].color,
		.board  = board,
	};

	// Generate moves for the king
	struct move_list* counter_moves =
			generate_moves_piece(&game, mate_move->target, true, false);
	if (move_list_count(counter_moves)) {
		// The king can still move out of check.
		move_list_free(counter_moves);
		return false;
	}
	move_list_free(counter_moves);

	// check all possible moves if they can prevent the checkmate
	// TODO: Add bitmask to check everything but the kings moves
	counter_moves     = generate_moves(&game, false, false);
	bool is_checkmate = true;
	while (move_list_count(counter_moves)) {
		struct move* cur_counter_move = move_list_pop(counter_moves);

		if (cur_counter_move->start == mate_move->target) {
			// Already checked the king moves as they need check_checkless
			free(cur_counter_move);
			continue;
		}

		if (cur_counter_move->target == mate_move->start) {
			// The counter_move hits the attacking piece
			free(cur_counter_move);
			is_checkmate = false;
			break;
		}

		// Backup piece for undo
		struct piece old = do_move(&game, cur_counter_move);

		// Check if mate_move is still doable or was declined
		struct move_list* moves =
				generate_moves_piece(&game, mate_move->start, false, false);
		bool mate_declined = true;
		while (move_list_count(moves)) {
			struct move* cur_move = (struct move*)move_list_pop(moves);
			if (cur_move->target == mate_move->target) {
				// mate_move was not declined, try the next counter_move
				free(cur_move);
				mate_declined = false;
				break;
			}

			free(cur_move);
		}
		move_list_free(moves);

		// Undo move
		undo_move(&game, cur_counter_move, old);
		free(cur_counter_move);

		if (mate_declined) {
			// counter_move successful
			is_checkmate = false;
			break;
		}
	}
	move_list_free(counter_moves);

	return is_checkmate;
}

/**
 * Execute move and check if the same piece from the new positions can hit the
 * opponent's king.
 * Also updates move->is_checkmate if true.
 */
bool
is_checkless_move(struct chess* game, struct move* move)
{
	struct piece old = do_move(game, move);

	struct move_list* new_moves;

	new_moves = generate_moves_piece(game, move->target, false, false);
	assert(new_moves);

	bool is_checkless_move = true;
	while (move_list_count(new_moves)) {
		struct move* cur_move = (struct move*)move_list_pop(new_moves);
		if (!cur_move->hit) {
			free(cur_move);
			continue;
		}

		if (game->board[cur_move->target].type == KING) {
			if (!is_checkmate(game->board, cur_move)) {
				// Found checkless move
				free(cur_move);
				is_checkless_move = false;
				break;
			} else {
				move->is_checkmate = true;
			}
		}
		free(cur_move);
	}
	move_list_free(new_moves);

	undo_move(game, move, old);
	return is_checkless_move;
}

bool
is_valid_pos(enum pos pos)
{
	return (pos >= 0 && pos < 64);
}

bool
is_occupied(struct piece board[], enum pos target)
{
	return board[target].type != EMPTY;
}

/**
 * @return true if the specified target is occupied by the enemy.
 * NOTE: If this function returns false, that might mean that target is not
 * occupied at all or that it's occupied by an ally.
 */
bool
is_occupied_by_enemy(struct piece board[], enum pos pos, enum pos target)
{
	return is_occupied(board, target) &&
		   board[pos].color != board[target].color;
}

/*-----------------------------
 * Helpers for move generation
 * ----------------------------*/

enum MOVES_TYPE { DIAGONAL, ORTHOGONAL, BOTH };

static int offsets_diag[4] = { +9, +7, -9, -7 };
static int offsets_orth[4] = { +1, +8, -1, -8 };
static int offsets_both[8] = { +1, +9, +8, +7, -1, -9, -8, -7 };

/**
 * @arg range: Use this parameter to calculate diagonal and orthogonal moves up
 * to a certain range, i.e. the king who may only walk one tile. Use -1 for
 * "unlimited" range, meaning until the end of the board is reached.
 */
struct move_list*
generate_moves_helper(struct chess* game, enum pos pos, bool endless,
                      enum MOVES_TYPE type, bool check_checkless,
                      bool hit_allies)
{
	struct move_list* moves = calloc(1, sizeof(*moves));
	int* offsets;
	size_t len;
	switch (type) {
	case DIAGONAL:
		offsets = offsets_diag;
		len     = sizeof(offsets_diag) / sizeof(*offsets_diag);
		break;
	case ORTHOGONAL:
		offsets = offsets_orth;
		len     = sizeof(offsets_orth) / sizeof(*offsets_orth);
		break;
	case BOTH:
		offsets = offsets_both;
		len     = sizeof(offsets_both) / sizeof(*offsets_both);
		break;
	default:
		fprintf(stderr, "Unknown moves type: %i", type);
		return NULL;
	}

	bool hit;
	for (size_t i = 0; i < len; ++i) {
		enum pos prev_target = pos;
		int prev_target_col  = pos % 8;
		bool diagonal        = !(offsets[i] % 7) || !(offsets[i] % 9);
		hit                  = false;

		// Continue as long as we are in range and did not hit something
		while (!hit) {
			enum pos target = prev_target + offsets[i];

			if (!is_valid_pos(target))
				break;

			int target_col = target % 8;
			if (prev_target_col - 1 != target_col &&
			    prev_target_col + 1 != target_col &&
			    (diagonal || prev_target_col != target_col))
				break; // we must have wrapped around the border

			if (is_occupied(game->board, target)) {
				if (is_occupied_by_enemy(game->board, pos, target) ||
				    hit_allies)
					hit = true; // in this move we will hit somebody
				else
					break;
			}

			/*
			 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
			 * be the last check!
			 */
			struct move test_move = { pos, target, hit, false, empty_piece, 0 };
			if (!check_checkless || is_checkless_move(game, &test_move)) {
				// Move passed all tests

				struct move* move = malloc(sizeof(*move));
				if (!move)
					return NULL;
				memcpy(move, &test_move, sizeof(*move));

				moves = move_list_push(moves, move);
			}

			prev_target     = target;
			prev_target_col = target_col;

			if (!endless)
				break;
		}
	}
	return moves;
}

/*-----------------------------
 * Piece based move generators
 * ----------------------------*/

struct move_list*
generate_moves_pawn_helper(struct chess* game, enum pos pos,
                           bool check_checkless, bool hit_allies)
{
	struct move_list* moves = calloc(1, sizeof(*moves));

	for (int i = -1; i <= 1; i++) {
		// value the start position needs to be modified by
		int move_by = (WIDTH + i) * -game->board[pos].color;

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
		bool occupied          = is_occupied(game->board, target);
		bool occupied_by_enemy = is_occupied_by_enemy(game->board, pos, target);

		if (!i == occupied)
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
		struct piece promotes_to = { EMPTY, UNDEFINED };
		if (target < 8 || target > 55) {
			promotes_to.type  = QUEEN;
			promotes_to.color = game->board[pos].color;
		}

		while (promotes_to.type <= QUEEN && promotes_to.type != PAWN) {
			struct move test = { pos,   target,      occupied_by_enemy && i,
				                 false, promotes_to, 0 };
			promotes_to.type--;

			if (check_checkless && !is_checkless_move(game, &test))
				continue;

			// add move if it passed all tests
			struct move* move = malloc(sizeof(*move));
			if (!move)
				return NULL;
			memcpy(move, &test, sizeof(*move));

			moves = move_list_push(moves, move);

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
generate_moves_knight_helper(struct chess* game, enum pos pos, enum pos target,
                             bool check_checkless, bool hit_allies)
{
	if (!is_valid_pos(target))
		return NULL;

	bool occupied_by_enemy = is_occupied_by_enemy(game->board, pos, target);
	bool occupied_by_ally =
			is_occupied(game->board, target) && !occupied_by_enemy;

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
	struct move test_move = { pos,   target,      occupied_by_enemy,
		                      false, empty_piece, 0 };
	if (check_checkless && !is_checkless_move(game, &test_move))
		return NULL;

	struct move* move = malloc(sizeof(*move));
	if (!move)
		return NULL;
	memcpy(move, &test_move, sizeof(*move));

	return move;
}

struct move_list*
generate_moves_king(struct chess* game, enum pos pos, bool check_checkless,
                    bool hit_allies)
{
	struct move_list* all_moves =
			generate_moves_helper(game, pos, false, BOTH, false, hit_allies);

	if (!all_moves)
		return NULL;

	if (!move_list_count(all_moves) || !check_checkless)
		return all_moves;

	struct chess new_game = {
		.moving = -1 * game->board[pos].color,
		.board  = game->board,
	};

	struct move_list* possible_hit_moves =
			generate_moves(&new_game, false, true);

	// TODO: use bitboard and & with king moves bitboard
	bool targets[64] = { 0 };
	are_attacked(possible_hit_moves, targets);

	// Remove all moves targeting an hittable field.
	struct move_list_elem* cur = move_list_get_first(all_moves);
	while (cur) {
		struct move* cur_move = (struct move*)cur->move;
		if (targets[cur_move->target]) {
			cur = move_list_remove(all_moves, cur);
			continue;
		}

		cur = move_list_get_next(cur);
	}

	return all_moves;
}

struct move_list*
generate_moves_knight(struct chess* game, enum pos pos, bool check_checkless,
                      bool hit_allies)
{
	struct move_list* moves = calloc(1, sizeof(*moves));
	int offsets[]           = { 6, 10, 15, 17 };
	for (int i = 0; i < 4; i++) {
		// downwards
		move_list_push(moves, generate_moves_knight_helper(
									  game, pos, pos + offsets[i],
									  check_checkless, hit_allies));
		// upwards
		move_list_push(moves, generate_moves_knight_helper(
									  game, pos, pos - offsets[i],
									  check_checkless, hit_allies));
	}

	return moves;
}

enum pos
get_king_pos(struct piece board[], enum color c)
{
	for (enum pos i = 0; i < 64; i++) {
		if (board[i].type == KING && board[i].color == c)
			return i;
	}

	return 64;
}

/*-------------------
 * Interface
 * ------------------*/

struct move_list*
generate_moves_piece(struct chess* game, enum pos pos, bool check_checkless,
                     bool hit_allies)
{
	struct move_list* moves = NULL;
	// clang-format off
	switch (game->board[pos].type) {
	case QUEEN:  moves = generate_moves_helper(game, pos, true, BOTH, check_checkless, hit_allies); break;
	case KING:   moves = generate_moves_king  (game, pos, check_checkless, hit_allies); break;
	case ROOK:   moves = generate_moves_helper(game, pos, true, ORTHOGONAL, check_checkless, hit_allies); break;
	case KNIGHT: moves = generate_moves_knight(game, pos, check_checkless, hit_allies); break;
	case PAWN:   moves = generate_moves_pawn_helper(game, pos, check_checkless, hit_allies); break;
	case BISHOP: moves = generate_moves_helper(game, pos, true, DIAGONAL, check_checkless, hit_allies); break;
	default:
		printf("Invalid piece at %i: %i\n", pos, game->board[pos].type);
		assert(false && "Invalid piece"); return NULL;
	}
	// clang-format on

	if (!moves)
		return NULL;

	if (!check_checkless)
		return moves;

	enum pos king_pos = get_king_pos(game->board, game->board[pos].color);
	if (king_pos == 64 || king_pos == pos)
		// No king, or not needed as we moved the king itself.
		return moves;

	struct chess new_game = {
		.moving = -1 * game->board[pos].color,
		.board  = game->board,
	};

	// Remove all moves that leave the king hittable.
	struct move_list_elem* cur = move_list_get_first(moves);
	while (cur) {
		struct move* cur_move = (struct move*)cur->move;
		bool opens_king       = false;

		// Backup piece for undo
		struct piece old = do_move(&new_game, cur_move);

		struct move_list* possible_hit_moves =
				generate_moves(&new_game, false, false);

		// Undo move
		undo_move(&new_game, cur_move, old);

		// Check if hitting moves target the king's field
		while (move_list_count(possible_hit_moves)) {
			struct move* hit_move = move_list_pop(possible_hit_moves);
			if (hit_move->target == king_pos) {
				cur        = move_list_remove(moves, cur);
				opens_king = true;
				free(hit_move);
				break;
			}

			free(hit_move);
		}
		move_list_free(possible_hit_moves);

		if (opens_king)
			continue;

		cur = move_list_get_next(cur);
	}

	return moves;
}

struct move_list*
generate_moves(struct chess* game, bool check_checkless, bool hit_allies)
{
	struct move_list* moves = NULL;

	struct piece* board = game->board;
	for (enum pos pos = 0; pos < 64; ++pos) {
		// if the `pos` is not occupied there are no moves to generate
		if (!is_occupied(board, pos))
			continue;

		// only the player moving can actually move pieces
		if (board[pos].color != game->moving)
			continue;

		moves = move_list_append_move_list(
				moves,
				generate_moves_piece(game, pos, check_checkless, hit_allies));
	}
	return moves;
}
