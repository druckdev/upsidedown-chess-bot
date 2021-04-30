#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "board.h"

bool
execute_move(struct chess* game, struct move move)
{
	// TODO(Aurel): Stub. Fill this with code.
	// assert(("Not implemented yet", 0 != 0));
	return 1;
}

/*
 * Fills `str` with the corresponding human-readable string (null-terminated)
 * describing the field encoded in `pos`.
 * `str` should be at least (3 * sizeof(char)) big.
 *
 * Returns: `str`
 */
char*
pos_to_str(enum POS pos, char* str)
{
	str[0] = 'A' + pos % 8;
	str[1] = '1' + pos / 8;
	str[2] = '\0';

	return str;
}

void
print_board(enum PIECE board[])
{
	for (int pos = 0; pos < 64; ++pos) {
		if (pos % 8 == 0)
			printf("\n");

		printf("[");

		enum PIECE piece = board[pos];
		switch (piece) {
		// clang-format off
        case PAWN: printf("P"); break;
        case BISHOP: printf("B"); break;
        case KNIGHT: printf("K"); break;
        case ROOK: printf("R"); break;
        case QUEEN: printf("Q"); break;
        case KING: printf("K"); break;
        default: printf(" "); break;
			// clang-format on
		}
		printf("]");
	}
	printf("\n");
}
