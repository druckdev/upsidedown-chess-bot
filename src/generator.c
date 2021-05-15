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
	struct list* moves;

	U64 mask = move_masks->knights[pos];
	U64 friendly_fire = ally_board & mask;
	U64 valid = mask - friendly_fire;

	return moves;
}

struct list*
generate_moves_queen()
{
	struct list* moves;
	return moves;
}

struct list*
generate_moves_rook()
{
	struct list* moves;
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
	struct move_masks* move_masks = calloc(1, sizeof(*move_masks));
	
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
	printf("\n\ncomplete");
	print_board(&(chess.board));
}
