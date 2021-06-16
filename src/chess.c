#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "board.h"
#include "bot.h"
#include "chess.h"
#include "generator.h"
#include "hashtable.h"
#include "move.h"
#include "timer.h"

#define MAX_FEN_STR_LEN 128

struct piece empty_piece = { EMPTY, WHITE };
int PIECE_VALUES[]       = { 0, 100, 400, 400, 500, 900, 1000000 };

int
get_piece_value(enum piece_type piece)
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
	if (bytes_read < 5) {
		fprintf(stderr, "Could not read move.\n");
		exit(1);
	}

	// TODO: Format check and error handling
	move->start = atoi(move_str);
	// Convert second value after the comma which changes its position depending
	// on the number of digits of `start`.
	move->target = atoi(move_str + 2 + (move->start > 9));

	size_t last_komma_index = 3 + (move->start > 9) + (move->target > 9);
	move->promotes_to       = chr_to_piece(move_str[last_komma_index + 1]);

	return move;
}

void
gs_print_move(struct move* move)
{
	printf("%i,%i,", move->start, move->target);

	char promotes_to_char = piece_to_chr(move->promotes_to);
	if (promotes_to_char != ' ')
		printf("%c", promotes_to_char);

	printf("\n");
	fflush(stdout);
}

struct chess
init_chess()
{
	struct chess chess;
	chess.checkmate   = false;
	chess.moving      = UNDEFINED;
	chess.phase       = EARLY_GAME;
	chess.board       = calloc(64, sizeof(*(chess.board)));
	chess.move_count  = 0;
	chess.max_moves   = MAX_MOVE_COUNT;
	chess.piece_count = 0;
	assert(init_ht(&chess.trans_table, TRANSPOSITION_TABLE_SIZE) &&
	       "Transposition table could not be initialized.");

	// TODO(Aurel): Once the server implements it, this will need to change.
	//chess.t_remaining_s = -1;
	chess.t_remaining_s = 1000; // Always update timer to have 1000s left

	return chess;
}

void
run_chess()
{
	struct chess game         = init_chess();
	char fen[MAX_FEN_STR_LEN] = { 0 };
	struct chess_timer* timer = start_timer(10 * 60);

	while (!game.checkmate) {
		ssize_t bytes_read = read(STDIN_FILENO, fen, sizeof(fen) - 1);
		if (bytes_read < 0) {
			perror("Error reading");
			free(game.board);
			exit(1);
		}
		fen[bytes_read] = '\0';

		update_timer(timer, &game);

		fen_to_chess(fen, &game);

#ifdef DEBUG_BOARD_WHEN_PLAYING
		fprint_board(DEBUG_PRINT_STREAM, game.board, NULL);
#endif

		struct move* move = choose_move(&game, timer);
		if (!move)
			break;

		gs_print_move(move);

		do_move(&game, move);
#ifdef DEBUG_BOARD_WHEN_PLAYING
		struct move_list* list = move_list_push(NULL, move);
		fprint_board(DEBUG_PRINT_STREAM, game.board, list);
#else
		free(move);
#endif
		game.move_count++;
		game.phase = get_game_phase(&game);
	}

	free_ht(&game.trans_table);
	free(game.board);
	free(timer);
}
