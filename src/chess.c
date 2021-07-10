#include <assert.h>
#include <pthread.h>
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
#include "param_config.h"
#include "timer.h"

#define MAX_FEN_STR_LEN 128

struct piece empty_piece = { EMPTY, WHITE };
int* PIECE_VALUES        = config.piece_values;

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
init_chess(char color, float total_time_s, size_t max_moves)
{
	struct chess chess;
	chess.checkmate   = false;
	chess.moving      = color == 'w' ? WHITE : color == 'b' ? BLACK : UNDEFINED;
	chess.phase       = EARLY_GAME;
	chess.board       = calloc(64, sizeof(*(chess.board)));
	chess.move_count  = 0;
	chess.max_moves   = max_moves;
	chess.piece_count = 0;
	assert(init_ht(&chess.trans_table, TRANSPOSITION_TABLE_SIZE) &&
	       "Transposition table could not be initialized.");

	chess.t_remaining_s = total_time_s;
	chess.cur_best_move = NULL;

	return chess;
}

void*
choose_move_thread_wrapper(void* args)
{
	return choose_move((struct chess*)args);
}

void
run_chess(struct chess* game)
{
	char fen[MAX_FEN_STR_LEN] = { 0 };
	struct chess_timer* timer = start_timer(game->t_remaining_s);

	while (!game->checkmate) {
		ssize_t bytes_read = read(STDIN_FILENO, fen, sizeof(fen) - 1);
		if (bytes_read < 0) {
			perror("Error reading");
			free(game->board);
			exit(1);
		}
		fen[bytes_read] = '\0';

		update_timer(timer, game);

		fen_to_chess(fen, game);

#ifdef DEBUG_BOARD_WHEN_PLAYING
		fprint_board(DEBUG_PRINT_STREAM, game->board, NULL);
#endif

		pthread_t tid;
		pthread_create(&tid, NULL, choose_move_thread_wrapper, game);

		double move_time = get_remaining_move_time(timer);
		int move_secs = (int)move_time;
		struct timespec time = { move_secs, (move_time - move_secs) * 1e9 };
		nanosleep(&time, NULL);
		pthread_cancel(tid);

		if (!game->cur_best_move)
			break;

		gs_print_move(game->cur_best_move);

		do_move(game, game->cur_best_move);
#ifdef DEBUG_BOARD_WHEN_PLAYING
		struct move_list* list = move_list_push(NULL, game->cur_best_move);
		fprint_board(DEBUG_PRINT_STREAM, game->board, list);
#else
		free(game->cur_best_move);
#endif
		game->move_count++;
		game->phase = get_game_phase(game);
	}

	free(timer);
}
