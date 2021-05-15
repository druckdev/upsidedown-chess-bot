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
	U64 valid = friendly_fire | mask;

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

		if (x > 1 && x < 6 && y > 1 && y < 6) {
			move_masks->knights[i] = 0;

			move_masks->knights[i] |= one << i + 10;
			move_masks->knights[i] |= one << i + 17;
			move_masks->knights[i] |= one << i + 15;
			move_masks->knights[i] |= one << i + 6;
			move_masks->knights[i] |= one << i - 10;
			move_masks->knights[i] |= one << i - 17;
			move_masks->knights[i] |= one << i - 15;
			move_masks->knights[i] |= one << i - 6;
		}

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
