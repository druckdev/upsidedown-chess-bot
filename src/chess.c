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

	// TODO: Format check and error handling
	move->start = atoi(move_str);
	// Convert second value after the comma which changes its position depending
	// on the number of digits of `start`.
	move->target = atoi(move_str + 2 + (move->start > 9));

	size_t last_komma_index = 3 + (move->start > 9) + (move->target > 9);
	move->promotes_to       = chr_to_piece(move_str[last_komma_index + 1]).type;

	return move;
}

void
print_move(struct move* move)
{
	printf("%i,%i,", move->start, move->target);

	char promotes_to_char = piece_e_to_chr(move->promotes_to);
	if (promotes_to_char != ' ')
		printf("%c", promotes_to_char);

	printf("\n");
	fflush(stdout);
}

struct chess
init_chess(enum COLOR c)
{
	struct chess chess = { 0 };
	board_from_fen(DEFAULT_BOARD, chess.board);
	chess.moving = c;
	return chess;
}

void
run_chess(struct chess* game)
{
	// print_board(game->board, NULL);

	if (game->moving == BLACK) {
		// Let opponent make the first move
		struct move move;
		assert(execute_move(game->board, opponent_move(&move)));
		// print_board(game->board, NULL);
	}

	while (!game->checkmate) {
		// sleep(1);
		struct list* moves = generate_moves(game, 1, false);
		struct move* move  = choose_move(game, moves);
		print_move(move);

		assert(execute_move(game->board, move));
		list_free(moves);

		struct move oppo_move;
		assert(execute_move(game->board, opponent_move(&oppo_move)));
		// print_board(game->board, NULL);
	}
}
