#include "types.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

#define ANSI_RED "\033[91m"
#define ANSI_RESET "\033[0m"
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

// Return a character representing the piece.
// Returns ' ' on unknown or empty piece.
char
piece_e_to_chr(enum PIECE_E piece)
{
	switch (piece) {
	case PAWN:
		return 'P';
	case BISHOP:
		return 'B';
	case KNIGHT:
		return 'N';
	case ROOK:
		return 'R';
	case QUEEN:
		return 'Q';
	case KING:
		return 'K';
	default:
		return ' ';
	}
}

// Return a character representing the piece and its color.
// Returns ' ' on unknown or empty piece.
char
piece_to_chr(struct PIECE piece)
{
	return piece_e_to_chr(piece.type) + (piece.color ? WHITE_TO_BLACK_OFF : 0);
}

struct PIECE
chr_to_piece(char fen_piece)
{
	struct PIECE piece = { EMPTY, WHITE };

	switch (fen_piece) {
	case 'p':
	case 'P':
		piece.type = PAWN;
		break;
	case 'b':
	case 'B':
		piece.type = BISHOP;
		break;
	case 'n':
	case 'N':
		piece.type = KNIGHT;
		break;
	case 'r':
	case 'R':
		piece.type = ROOK;
		break;
	case 'q':
	case 'Q':
		piece.type = QUEEN;
		break;
	case 'k':
	case 'K':
		piece.type = KING;
		break;
	default:
		break;
	}

	// Set to black if lowercase letter.
	if (fen_piece >= 'a' && fen_piece <= 'z')
		piece.color = BLACK;

	return piece;
}

void
board_from_fen(char* fen, struct PIECE board[])
{
	memset(board, 0, sizeof(struct PIECE) * 64);

	size_t c = -1, i = 0;
	while (fen[++c]) {
		if (fen[c] >= '0' && fen[c] <= '9') {
			// Skip number of fields indicated by number in fen[c].
			i += atoi(fen + c);
		} else if (fen[c] != '/') {
			board[i++] = chr_to_piece(fen[c]);
		}
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

// Returns an array that indicates if a position is attackable by a move in
// `moves`.
// `moves` is consumed and freed.
// Allocates memory if the passed pointer equals to NULL.
bool*
are_attacked(struct list* moves, bool* targets)
{
	if (!targets) {
		targets = calloc(sizeof(bool), 64);
		if (!targets)
			return NULL;
	}

	if (!moves)
		return targets;

	while (moves->last) {
		struct move* move     = list_pop(moves);
		targets[move->target] = true;
		free(move);
	}
	list_free(moves);

	return targets;
}

/**
 * @arg moves: optional for marking attacked fields, set to NULL if not wanted
 *             Because of efficiency reasons, moves is 'consumed' and freed.
 */
void
print_board(struct PIECE board[], struct list* moves)
{
	bool* targets = are_attacked(moves, NULL);
	char* padding = "     ";

	printf("%s", padding);
	for (char label = 'A'; label <= 'H'; ++label)
		printf(" %c ", label);
	printf("\n");

	size_t row = 8;
	for (enum POS pos = 0; pos < MAX; ++pos) {
		if (pos % 8 == 0)
			printf("%02i %li ", pos, row);

		if (targets[pos])
			printf(ANSI_RED);
		printf("[%c]", piece_to_chr(board[pos]));
		printf(ANSI_RESET);

		if (pos % 8 == 7)
			printf(" %li\n", row--);
	}

	printf("%s", padding);
	for (char label = 'A'; label <= 'H'; ++label)
		printf(" %c ", label);
	printf("\n");

	free(targets);
}
