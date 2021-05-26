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

struct list* generate_moves_piece(struct PIECE board[], enum POS pos,
                                  bool check_checkless, bool hit_allies);

/**
 * @arg board - the current board state
 * @arg mate_move - the move on `board` that has the king as target.
 * @return If board is a checkmate position or if the color of the attacked king
 *         can still get out of check.
 */
bool
is_checkmate(struct PIECE board[], struct move* mate_move)
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
	struct list* counter_moves =
			generate_moves_piece(game.board, mate_move->target, true, false);
	if (list_count(counter_moves)) {
		// The king can still move out of check.
		list_free(counter_moves);
		return false;
	}
	list_free(counter_moves);

	// check all possible moves if they can prevent the checkmate
	// TODO: Add bitmask to check everything but the kings moves
	counter_moves     = generate_moves(&game, false, false);
	bool is_checkmate = true;
	while (list_count(counter_moves)) {
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
		struct PIECE old = do_move(game.board, cur_counter_move);

		// Check if mate_move is still doable or was declined
		struct list* moves = generate_moves_piece(game.board, mate_move->start,
		                                          false, false);
		bool mate_declined = true;
		while (list_count(moves)) {
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

		// Undo move
		undo_move(game.board, cur_counter_move, old);
		free(cur_counter_move);

		if (mate_declined) {
			// counter_move successful
			counter_moves = false;
			break;
		}
	}
	list_free(counter_moves);

	return is_checkmate;
}

/**
 * Execute move and check if the same piece from the new positions can hit the
 * opponent's king.
 * Also updates move->is_checkmate if true.
 */
bool
is_checkless_move(struct PIECE board[], struct move* move)
{
	struct PIECE old = do_move(board, move);

	struct list* new_moves;

	new_moves = generate_moves_piece(board, move->target, false, false);
	assert(new_moves);

	bool is_checkless_move = true;
	while (list_count(new_moves)) {
		struct move* cur_move = (struct move*)list_pop(new_moves);
		if (!cur_move->hit) {
			free(cur_move);
			continue;
		}

		if (board[cur_move->target].type == KING) {
			if (!is_checkmate(board, cur_move)) {
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
	list_free(new_moves);

	undo_move(board, move, old);
	return is_checkless_move;
}

bool
is_valid_pos(enum POS pos)
{
	return (pos >= 0 && pos < 64);
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
struct list*
generate_moves_helper(struct PIECE board[], enum POS pos, bool endless,
                      enum MOVES_TYPE type, bool check_checkless,
                      bool hit_allies)
{
	struct list* moves = calloc(1, sizeof(*moves));
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
		enum POS prev_target = pos;
		int prev_target_col  = pos % 8;
		bool diagonal        = !(offsets[i] % 7) || !(offsets[i] % 9);
		hit                  = false;

		// Continue as long as we are in range and did not hit something
		while (!hit) {
			enum POS target = prev_target + offsets[i];

			if (!is_valid_pos(target))
				break;

			int target_col = target % 8;
			if (prev_target_col - 1 != target_col &&
			    prev_target_col + 1 != target_col &&
			    (diagonal || prev_target_col != target_col))
				break; // we must have wrapped around the border

			if (is_occupied(board, target)) {
				if (is_occupied_by_enemy(board, pos, target) || hit_allies)
					hit = true; // in this move we will hit somebody
				else
					break;
			}

			/*
			 * NOTE(Aurel): `is_checkless_move` is the slowest and should always
			 * be the last check!
			 */
			struct move test_move = { pos, target, hit, false, empty_piece };
			if (!check_checkless || is_checkless_move(board, &test_move)) {
				// Move passed all tests

				struct move* move = malloc(sizeof(*move));
				if (!move)
					return NULL;
				memcpy(move, &test_move, sizeof(*move));

				moves = list_push(moves, move);
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

struct list*
generate_moves_pawn_helper(struct PIECE board[], enum POS pos,
                           bool check_checkless, bool hit_allies)
{
	struct list* moves = calloc(1, sizeof(*moves));

	for (int i = -1; i <= 1; i++) {
		// value the start position needs to be modified by
		int move_by = (WIDTH + i) * -board[pos].color;

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
		struct PIECE promotes_to = { EMPTY, board[pos].color };
		if (target < 8 || target > 55)
			promotes_to.type = QUEEN;
		while (promotes_to.type <= QUEEN && promotes_to.type != PAWN) {
			struct move test = { pos, target, occupied_by_enemy && i, false,
				                 promotes_to };
			promotes_to.type--;

			if (check_checkless && !is_checkless_move(board, &test))
				continue;

			// add move if it passed all tests
			struct move* move = malloc(sizeof(*move));
			if (!move)
				return NULL;
			memcpy(move, &test, sizeof(*move));

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
	struct move test_move = { pos, target, occupied_by_enemy, false,
		                      empty_piece };
	if (check_checkless && !is_checkless_move(board, &test_move))
		return NULL;

	struct move* move = malloc(sizeof(*move));
	if (!move)
		return NULL;
	memcpy(move, &test_move, sizeof(*move));

	return move;
}

struct list*
generate_moves_king(struct PIECE board[], enum POS pos, bool check_checkless,
                    bool hit_allies)
{
	struct list* all_moves =
			generate_moves_helper(board, pos, false, BOTH, false, hit_allies);

	if (!all_moves)
		return NULL;

	if (!list_count(all_moves) || !check_checkless)
		return all_moves;

	struct chess game = {
		.moving = -1 * board[pos].color,
		.board  = board,
	};

	struct list* possible_hit_moves = generate_moves(&game, false, true);

	// TODO: use bitboard and & with king moves bitboard
	bool targets[64] = { 0 };
	are_attacked(possible_hit_moves, targets);

	// Remove all hittable fields.
	struct list_elem* cur = list_get_first(all_moves);
	while (cur) {
		struct move* cur_move = (struct move*)cur->object;
		if (targets[cur_move->target]) {
			cur = list_remove(all_moves, cur);
			continue;
		}

		cur = list_get_next(cur);
	}

	return all_moves;
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
 * Interface
 * ------------------*/

struct list*
generate_moves_piece(struct PIECE board[], enum POS pos, bool check_checkless,
                     bool hit_allies)
{
	struct list* moves;
	// clang-format off
	switch (board[pos].type) {
	case QUEEN:  moves = generate_moves_helper(board, pos, true, BOTH, check_checkless, hit_allies); break;
	case KING:   moves = generate_moves_king  (board, pos, check_checkless, hit_allies); break;
	case ROOK:   moves = generate_moves_helper(board, pos, true, ORTHOGONAL, check_checkless, hit_allies); break;
	case KNIGHT: moves = generate_moves_knight(board, pos, check_checkless, hit_allies); break;
	case PAWN:   moves = generate_moves_pawn_helper(board, pos, check_checkless, hit_allies); break;
	case BISHOP: moves = generate_moves_helper(board, pos, true, DIAGONAL, check_checkless, hit_allies); break;
	default:
		printf("Invalid piece at %i: %i\n", pos, board[pos].type);
		assert(false && "Invalid piece"); return NULL;
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

	struct chess game = {
		.moving = -1 * board[pos].color,
		.board  = board,
	};

	// Remove all moves that leave the king hittable.
	struct list_elem* cur = list_get_first(moves);
	while (cur) {
		struct move* cur_move = (struct move*)cur->object;
		bool opens_king       = false;

		// Backup piece for undo
		struct PIECE old = do_move(game.board, cur_move);

		struct list* possible_hit_moves = generate_moves(&game, false, false);

		// Undo move
		undo_move(game.board, cur_move, old);

		// Check if hitting moves target the king's field
		while (list_count(possible_hit_moves)) {
			struct move* hit_move = list_pop(possible_hit_moves);
			if (hit_move->target == king_pos) {
				cur        = list_remove(moves, cur);
				opens_king = true;
				free(hit_move);
				break;
			}

			free(hit_move);
		}
		list_free(possible_hit_moves);

		if (opens_king)
			continue;

		cur = list_get_next(cur);
	}

	return moves;
}

struct list*
generate_moves(struct chess* game, bool check_checkless, bool hit_allies)
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
