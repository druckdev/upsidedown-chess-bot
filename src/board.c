#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "move.h"

#define ANSI_RED "\033[91m"
#define ANSI_RESET "\033[0m"

struct piece*
board_cpy(struct piece* board)
{
	struct piece* board_cpy = malloc(64 * sizeof(*board_cpy));
	if (!board_cpy)
		return NULL;
	memcpy(board_cpy, board, 64 * sizeof(*board_cpy));
	return board_cpy;
}

bool
is_same_piece(struct piece* p1, struct piece* p2)
{
	if (!p1 || !p2)
		return false;

	if (p1->type != p2->type)
		return false;
	if (p1->color != p2->color)
		return false;

	return true;
}

bool
is_same_board(struct piece* b1, struct piece* b2)
{
	if (!b1 || !b2)
		return false;

	for (size_t i = 0; i < 64; ++i)
		if (!is_same_piece(&b1[i], &b2[i]))
			return false;

	return true;
}

struct piece
do_move(struct chess* game, struct move* move)
{
	if (!game || !game->board || !move)
		return (struct piece){};

	assert(move->target != move->start);

	struct piece old              = game->board[move->target];
	game->board[move->target]     = move->promotes_to.type ?
                                            move->promotes_to :
                                            game->board[move->start];
	game->board[move->start].type = EMPTY;

	if (move->hit)
		game->piece_count--;

	return old;
}

void
undo_move(struct chess* game, struct move* move, struct piece old)
{
	if (!game || !game->board || !move)
		return;

	game->board[move->start] = game->board[move->target];
	if (move->promotes_to.type)
		game->board[move->start].type = PAWN;

	game->board[move->target] = old;
	if (move->hit)
		game->piece_count++;
}

enum game_phase
get_game_phase(struct chess* game)
{
	/*
	 * TODO(Aurel): How do we determine when the early/mid game ends? What is a
	 * good heuristic besides just move and piece count.
	 */

	enum game_phase phase = game->phase;

	if (phase == EARLY_GAME && (game->move_count > EG_MOVE_COUNT_MAX ||
	                            game->piece_count < EG_PIECE_COUNT_MIN))
		phase = MID_GAME;

	if (phase == MID_GAME && (game->move_count > MG_MOVE_COUNT_MAX ||
	                          game->piece_count < MG_PIECE_COUNT_MIN))
		phase = LATE_GAME;

	return phase;
}

/*
 * Fills `str` with the corresponding human-readable string (null-terminated)
 * describing the field encoded in `pos`.
 * `str` should be at least (3 * sizeof(char)) big.
 *
 * Returns: `str`
 */
char*
pos_to_str(enum pos pos, char* str)
{
	str[0] = 'A' + pos % 8;
	str[1] = '8' - pos / 8;
	str[2] = '\0';

	return str;
}

// Return a character representing the piece.
// Returns ' ' on unknown or empty piece.
char
piece_e_to_chr(enum piece_type piece)
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
piece_to_chr(struct piece piece)
{
	char c = piece_e_to_chr(piece.type);
	if (c != ' ' && piece.color == BLACK)
		c += WHITE_TO_BLACK_OFF;
	return c;
}

struct piece
chr_to_piece(char fen_piece)
{
	struct piece piece = { EMPTY, WHITE };

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
fen_to_chess(char* fen, struct chess* game)
{
	memset(game->board, 0, sizeof(struct piece) * 64);

	size_t c = -1, i = 0;
	while (fen[++c] && i < 64) {
		if (fen[c] >= '0' && fen[c] <= '9') {
			// Skip number of fields indicated by number in fen[c].
			i += atoi(fen + c);
		} else if (fen[c] != '/') {
			game->board[i++] = chr_to_piece(fen[c]);
			game->piece_count++;
		}
	}
	if (!fen[c]) // abort on \0
		return;

	if (!fen[++c]) // skip space
		return;

	// get moving color
	game->moving = fen[c] == 'w' ? WHITE : fen[c] == 'b' ? BLACK : UNDEFINED;
	if (!fen[++c])
		return;

	if (!fen[++c]) // skip space
		return;

	// get remaining move count
	game->move_count = atoi(fen + c) / 2; // the given moves is for two players
	if (!fen[++c])
		return;

	if (!fen[++c]) // skip space
		return;

	// get remaining time
	game->t_remaining_s = atof(fen + c);
}

// returns true if the position is attacked by one of the given moves
bool
is_attacked(struct move_list* moves, enum pos pos)
{
	if (!moves)
		return false;

	struct move_list_elem* cur = move_list_get_first(moves);
	while (cur) {
		struct move* move = cur->move;
		if (move->target == pos)
			return true;
		cur = move_list_get_next(cur);
	}
	return false;
}

// Returns an array that indicates if a position is attackable by a move in
// `moves`.
// `moves` is consumed and freed.
// Allocates memory if the passed pointer equals to NULL.
bool*
are_attacked(struct move_list* moves, bool* targets)
{
	if (!targets) {
		targets = calloc(sizeof(bool), 64);
		if (!targets)
			return NULL;
	}

	if (!moves)
		return targets;

	while (move_list_count(moves)) {
		struct move* move     = move_list_pop(moves);
		targets[move->target] = true;
		free(move);
	}
	move_list_free(moves);

	return targets;
}

/**
 * @arg moves: optional for marking attacked fields, set to NULL if not wanted
 *             Because of efficiency reasons, moves is 'consumed' and freed.
 */
void
fprint_board(FILE* stream, struct piece board[], struct move_list* moves)
{
	bool* targets = are_attacked(moves, NULL);
	char* padding = "     ";

	fprintf(stream, "%s", padding);
	for (char label = 'A'; label <= 'H'; ++label)
		fprintf(stream, " %c ", label);
	fprintf(stream, "\n");

	size_t row = 8;
	for (enum pos pos = 0; pos < MAX; ++pos) {
		if (pos % 8 == 0)
			fprintf(stream, "%02i %li ", pos, row);

		if (targets[pos])
			fprintf(stream, ANSI_RED);
		fprintf(stream, "[%c]", piece_to_chr(board[pos]));
		fprintf(stream, ANSI_RESET);

		if (pos % 8 == 7)
			fprintf(stream, " %li\n", row--);
	}

	fprintf(stream, "%s", padding);
	for (char label = 'A'; label <= 'H'; ++label)
		fprintf(stream, " %c ", label);
	fprintf(stream, "\n");

	free(targets);
}
