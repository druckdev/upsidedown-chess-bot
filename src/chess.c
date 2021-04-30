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

struct move
opponent_move()
{
	struct move move;

	char move_str[8];

	ssize_t bytes_read = read(STDIN_FILENO, move_str, sizeof(move_str) - 1);
	if (bytes_read < 0) {
		perror("Error reading");
		exit(1);
	}
	move_str[bytes_read] = '\0';

	printf("Read: %s", move_str);

	// TODO: Format check and error handling
	move.start = atoi(move_str);
	// Convert second value after the comma which changes its position depending
	// on the number of digits of `start`.
	move.target = atoi(move_str + 2 + move.start / 10);

	move.promotes_to = 0;
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
		case 'K':
			promotes_to = KING;
			break;
		case 'B':
			promotes_to = BISHOP;
			break;
		default:
			break; // Default of 0 already set.
		}
		move.promotes_to = promotes_to;
	}
	return move;
}

struct chess
init_chess()
{
	struct chess chess = { 0 };
	memcpy(chess.board, DEFAULT_BOARD, sizeof(chess.board));
	chess.moving = WHITE;
	return chess;
}

void
run_chess(struct chess* game)
{
	while (!game->checkmate) {
		// struct list* moves = generate_moves(game);
		// struct move* move  = choose_move(game, *moves);
		// assert(execute_move(game, *move));
		assert(execute_move(game, opponent_move()));
		print_board(game->board);
	}
}
