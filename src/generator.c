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

/*-----------------------------
 * Helper
 * ---------------------------*/

// generates a list of moves (allocates memory) with
// start being the given 'pos' and targets being the bits
// set on the given 'bitboard'
struct list*
moves_from_bitboard(U64 bitboard, enum POS pos, U64 enemy_board) {
	struct list* moves = NULL;

	for (int i = 0; i < MAX; i++) {
		if (is_set_at(bitboard, i)) {
			struct move* move = malloc(sizeof(*move));

			if (!move)
				return NULL;

			move->start = pos; 
			move->target = i;
			move->hit = is_set_at(enemy_board, i);

			moves = list_push(moves, move);
			if (!moves)
				return NULL;
		}
	}

	return moves;
}


// Generates a list of moves (allocates memory) with
// start being the given 'rook_pos' going into a direction and
// with a stepsize given by 'factor'.
// Target is the possible target being checked next.
struct list*
append_unblocked_rook_moves(struct list* moves, U64 valid, enum POS rook_pos, 
							enum POS target, U64 enemy_board, int factor)
{
	// recursion hook, checks whether board has ended or an ally is the target
	if(!is_set_at(valid, target) || target >= MAX || target <= MIN)
		return moves;
	
	struct move* move = malloc(sizeof(*move));
	move->start = rook_pos;
	move->target = target;
	move->hit = is_set_at(enemy_board, target);

	moves = list_push(moves, move);
	enum POS new_target = target + factor;

	// stop when the most recent move was a hit
	if (!move->hit) {
		moves = append_unblocked_rook_moves(moves, valid, rook_pos, new_target, enemy_board, factor);
	}
	return moves;
}

/*-----------------------------
 * Piece based move generators
 * ---------------------------*/

struct list*
generate_moves_king()
{
	struct list* moves;
	return moves;
}

struct list*
generate_moves_knight(U64 ally_board, U64 enemy_board, enum POS pos, 
						struct move_masks* move_masks)
{
	U64 mask = move_masks->knights[pos];
	U64 friendly_fire = ally_board & mask;
	U64 valid = mask - friendly_fire;

	struct list* moves = moves_from_bitboard(valid, pos, enemy_board);
	return moves;
}

struct list*
generate_moves_queen()
{
	struct list* moves;
	return moves;
}

struct list*
generate_moves_rook(U64 ally_board, U64 enemy_board, enum POS pos, 
						struct move_masks* move_masks)
{
	U64 mask = move_masks->rooks[pos];
	U64 friendly_fire = ally_board & mask;
	U64 valid = mask - friendly_fire;

	struct list* moves = NULL;
	moves = append_unblocked_rook_moves(moves, valid, pos, pos+1, 
										enemy_board, 1);
	moves = append_unblocked_rook_moves(moves, valid, pos, pos-1, 
										enemy_board, -1);
	moves = append_unblocked_rook_moves(moves, valid, pos, pos+8, 
										enemy_board, 8);
	moves = append_unblocked_rook_moves(moves, valid, pos, pos-8, 
										enemy_board, -8);

	return moves;
}


struct list*
generate_moves_bishop()
{
	struct list* moves;
	return moves;
}

struct list*
generate_moves_pawn()
{
	struct list* moves;
	return moves;
}

/*----------------------------------
 * Piece based move mask generation
 * --------------------------------*/

void
init_move_masks_king(struct move_masks* move_masks)
{
}

void
init_move_masks_knight(struct move_masks* move_masks)
{
	U64 one = 1; // helper, outsource later

	int x = 0, y = 0;
	for (int i=H1; i < MAX; i++) {

		if (x >= WIDTH) {
			x = 0;
			y++;
		}

		move_masks->knights[i] = 0;
		if (x < 6 && y < 7) 
			move_masks->knights[i] |= one << i + 10;

		if (x < 7 && y < 6)	
			move_masks->knights[i] |= one << i + 17;
		
		if (x > 0 && y < 6)
			move_masks->knights[i] |= one << i + 15;
		
		if (x > 1 && y < 7)
			move_masks->knights[i] |= one << i + 6;

		if (x > 1 && y > 0)
			move_masks->knights[i] |= one << i - 10;
		
		if (x > 0 && y > 1)
			move_masks->knights[i] |= one << i - 17;
		
		if (x < 7 && y > 1)
			move_masks->knights[i] |= one << i - 15;
		
		if (x < 6 && y > 0)
			move_masks->knights[i] |= one << i - 6;

		x++;
	}
}

void
init_move_masks_queen(struct move_masks* move_masks)
{
}

void
init_move_masks_rook(struct move_masks* move_masks)
{
	U64 one = 1; // helper, outsource later
	
	int x = 0, y = 0;
	for (int i=H1; i < MAX; i++) {
		if (x >= WIDTH) {
			x = 0;
			y++;
		}

		move_masks->rooks[i] = 0;

		// generate horizontal moves
		int lower_x = (WIDTH * y)-1; // is immediatly incremented
		int higher_x = (lower_x + WIDTH);
		
		while (lower_x++ < higher_x) {
			if (lower_x != i)
				move_masks->rooks[i] |= one << lower_x;
		}
		
		// generate vertical moves
		int lower_y = -1;
		int higher_y = lower_y+HEIGHT;
		while (lower_y++ < higher_y) {
			int curr = lower_y * WIDTH + x;
			if (curr != i)
				move_masks->rooks[i] |= one << curr;
		}

		x++;
	}
}

void
init_move_masks_bishop(struct move_masks* move_masks)
{
}

void
init_move_masks_pawn(struct move_masks* move_masks)
{
}


/*-------------------
 * Interface
 * ------------------*/

struct list*
generate_moves(struct chess* game)
{ 
}

// call this once at the start of the program
struct move_masks*
init_move_masks()
{
	struct move_masks* move_masks = malloc(sizeof(*move_masks));
	
	init_move_masks_king(move_masks);
	init_move_masks_knight(move_masks);
	init_move_masks_queen(move_masks);
	init_move_masks_rook(move_masks);
	init_move_masks_bishop(move_masks);
	init_move_masks_pawn(move_masks);
	
	return move_masks;
}

// TEMPORARY MAIN
int
main(int argc, char* argv[])
{
	
	struct chess chess;
	
	fen_to_game(DEFAULT_BOARD, &chess);
	printf("complete \n\n");
	print_board(&(chess.board), NULL);

	fen_to_game("4KBR/4PPP1/8/7P/8/8/pppp4/rnbq4", &chess);
	printf("\n\ncomplete \n");
	print_board(&(chess.board), NULL);
	
	struct move_masks* move_masks = malloc(sizeof(*move_masks));

	// KNIGHT

	init_move_masks_knight(move_masks);
	printf("\n");
	print_bitboard(move_masks->knights[57]);


	struct list* knight_moves = generate_moves_knight(chess.board.white_pieces, 
														chess.board.black_pieces, 
														57, move_masks);
	printf("\n\nknight moves : \n");
	print_moves(knight_moves);
	print_board(&(chess.board), knight_moves);

	// ROOK 

	init_move_masks_rook(move_masks);
	printf("\n");
	print_bitboard(move_masks->rooks[56]);

	struct list* rook_moves = generate_moves_rook(chess.board.white_pieces, 
														chess.board.black_pieces, 
														56, move_masks);
	printf("\n\nrook moves : \n");
	print_moves(rook_moves);
	print_board(&(chess.board), rook_moves);
}
