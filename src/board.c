#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"

bool
execute_move(struct PIECE* board, struct move* move)
{
	// printf("exec: %i, %i, %i\n", move->start, move->target, move->promotes_to);
	if (!board || !move)
		return false;

	board[move->target]     = board[move->start];
	board[move->start].type = EMPTY;

	if (move->promotes_to != EMPTY)
		board[move->target].type = move->promotes_to;

	return true;
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
	str[1] = '8' - pos / 8;
	str[2] = '\0';

	return str;
}

void
board_from_fen(char* fen, struct PIECE board[])
{
	int cnt = 0;
	for (int i = 0; fen[i] != '\0' || cnt < 64; i++) {
		switch (fen[i]) {
		case 'r':
			board[cnt].type  = ROOK;
			board[cnt].color = BLACK;
			break;
		case 'q':
			board[cnt].type  = QUEEN;
			board[cnt].color = BLACK;
			break;
		case 'p':
			board[cnt].type  = PAWN;
			board[cnt].color = BLACK;
			break;
		case 'k':
			board[cnt].type  = KING;
			board[cnt].color = BLACK;
			break;
		case 'b':
			board[cnt].type  = BISHOP;
			board[cnt].color = BLACK;
			break;
		case 'n':
			board[cnt].type  = KNIGHT;
			board[cnt].color = BLACK;
			break;
		case 'R':
			board[cnt].type  = ROOK;
			board[cnt].color = WHITE;
			break;
		case 'Q':
			board[cnt].type  = QUEEN;
			board[cnt].color = WHITE;
			break;
		case 'P':
			board[cnt].type  = PAWN;
			board[cnt].color = WHITE;
			break;
		case 'K':
			board[cnt].type  = KING;
			board[cnt].color = WHITE;
			break;
		case 'B':
			board[cnt].type  = BISHOP;
			board[cnt].color = WHITE;
			break;
		case 'N':
			board[cnt].type  = KNIGHT;
			board[cnt].color = WHITE;
			break;
		case '/':
			cnt--;
			break;

		default:
			for (int j = 0; j < atoi(&fen[i]); j++) {
				board[cnt + j].type = EMPTY;
			}
			cnt += atoi(&fen[i]) - 1;
		}
		cnt++;
	}
}

// returns true if the position is attacked by one of the given moves
bool
is_attacked(struct list* moves, enum POS pos)
{
	if (!moves)
		return false;

	struct list_elem* cur = moves->first;
	while (cur) {
		struct move* move = (struct move*)cur->object;
		if (move->target == pos)
			return true;
		cur = cur->next;
	}
	return false;
}

/**
 * @arg moves: optional for marking attacked fields, set to NULL if not wanted
 */
void
print_board(struct PIECE board[], struct list* moves)
{
	printf("\n      ");
	for (char label = 'A'; label <= 'H'; ++label) {
		printf(" %c ", label);
	}

	size_t row = 9;
	for (int pos = 0; pos < 64; ++pos) {
		if (pos % 8 == 0) {
			if (pos != 0)
				printf(" %li", row);
			size_t first_index_of_row = 8 * (9 - row);
			printf("\n");
			if (first_index_of_row < 10)
				printf(" ");
			printf(" %li ", first_index_of_row);
			printf("%li ", --row);
		}

		printf("[");

		if (moves && is_attacked(moves, pos)) {
			printf("X");
		} else {
			struct PIECE piece = board[pos];
			// clang-format off
			switch (piece.type) {
			case PAWN:   piece.color == WHITE ? printf("P") : printf("p"); break;
			case BISHOP: piece.color == WHITE ? printf("B") : printf("b"); break;
			case KNIGHT: piece.color == WHITE ? printf("N") : printf("n"); break;
			case ROOK:   piece.color == WHITE ? printf("R") : printf("r"); break;
			case QUEEN:  piece.color == WHITE ? printf("Q") : printf("q"); break;
			case KING:   piece.color == WHITE ? printf("K") : printf("k"); break;
        	default: printf(" "); break;
			}
		}
		// clang-format on
		printf("]");
	}
	printf(" %li\n      ", row);

	for (char label = 'A'; label <= 'H'; ++label) {
		printf(" %c ", label);
	}
	printf("\n");
}
