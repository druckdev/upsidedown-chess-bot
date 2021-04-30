#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
#include "generator.h"

int
get_piece_value(enum PIECE_E piece)
{
	return PIECE_VALUES[piece];
}

struct move*
opponent_move(struct move* move)
{
	char move_str[9];

	ssize_t bytes_read = read(STDIN_FILENO, move_str, sizeof(move_str) - 1);
	if (bytes_read < 0) {
		perror("Error reading");
		exit(1);
	}

	//printf("Read: %s\n", move_str);

	// TODO: Format check and error handling
	move->start = atoi(move_str);
	// Convert second value after the comma which changes its position depending
	// on the number of digits of `start`.
	move->target = atoi(move_str + 2 + (move->start > 9));

	move->promotes_to = 0;
	if (move_str[bytes_read - 1] != ',') {
		// This move promotes
		enum PIECE_E promotes_to;
		switch (move_str[bytes_read - 1]) {
		case 'Q':
			promotes_to = QUEEN;
			break;
		case 'R':
			promotes_to = ROOK;
			break;
		case 'N':
			promotes_to = KNIGHT;
			break;
		case 'B':
			promotes_to = BISHOP;
			break;
		default:
			break; // Default of 0 already set.
		}
		move->promotes_to = promotes_to;
	}
	return move;
}

void
print_move(struct move* move)
{
	printf("%i,%i,", move->start, move->target);

	char promotes_to_char;
	switch (move->promotes_to) {
	case QUEEN:
		promotes_to_char = 'Q';
		break;
	case ROOK:
		promotes_to_char = 'R';
		break;
	case KNIGHT:
		promotes_to_char = 'N';
		break;
	case BISHOP:
		promotes_to_char = 'B';
		break;
	default:
		promotes_to_char = '\0';
		break;
	}
	if (promotes_to_char)
		printf("%c", promotes_to_char);

	printf("\n");
	fflush(stdout);
}

struct chess
init_chess(enum COLOR c)
{
	struct chess chess = { 0 };
	memcpy(chess.board, DEFAULT_BOARD, sizeof(chess.board));
	chess.moving = c;
	return chess;
}

void
run_chess(struct chess* game)
{
	// print_board(game->board);

	if (game->moving == BLACK) {
		// Let opponent make the first move
		struct move move;
		assert(execute_move(game->board, opponent_move(&move)));
		// print_board(game->board);
	}

	while (!game->checkmate) {
		struct list* moves = generate_moves(game);
		struct move* move  = choose_move(game, *moves);
		print_move(move);
		assert(execute_move(game->board, move));
		// TODO!
		// list_free(moves);
		// print_board(game->board);

		struct move oppo_move;
		assert(execute_move(game->board, opponent_move(&oppo_move)));
		// print_board(game->board);
	}
}
