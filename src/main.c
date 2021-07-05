#include <stdlib.h>

#include "bot.h"
#include "chess.h"
#include "hashtable.h"

void
debug_print_chess_features()
{
	fprintf(DEBUG_PRINT_STREAM, "Running bot with the following features:\n");
#ifdef ENABLE_ALPHA_BETA_CUTOFFS
	fprintf(DEBUG_PRINT_STREAM, "alpha-beta-cutoffs\n");
#endif
#ifdef ENABLE_TRANSPOSITION_TABLE
	fprintf(DEBUG_PRINT_STREAM, "transposition table\n");
#endif
	fprintf(DEBUG_PRINT_STREAM, "\n");
}

int
main(int argc, char* argv[])
{
	char color         = 'w';
	float total_time_s = 600.0;
	size_t max_moves   = MAX_MOVE_COUNT;

	if (argc == 2) {
		MAX_NEGAMAX_DEPTH = atoi(argv[1]);
	} else if (argc == 4) {
		color        = *argv[1];
		total_time_s = atof(argv[2]);
		max_moves    = atol(argv[3]) / 2; // the given moves is for two players
	}

	struct chess game = init_chess(color, total_time_s, max_moves);

	run_chess(&game);

	free_ht(&game.trans_table);
	free(game.board);
}
