#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "bot.h"
#include "chess.h"
#include "hashtable.h"

void
debug_print_chess_features()
{
	fprintf(DEBUG_PRINT_STREAM,
	        "Running bot with the following configuration:\n");
	fprintf(DEBUG_PRINT_STREAM, "Base algorithm:\n");
#if defined(VANILLA_MINIMAX)
	fprintf(DEBUG_PRINT_STREAM, "\tvanilla minimax\n");
#elif defined(ALPHA_BETA_CUTOFFS)
	fprintf(DEBUG_PRINT_STREAM, "\talpha-beta cutoffs\n");
#elif defined(PRINCIPAL_VARIATION_SEARCH)
	fprintf(DEBUG_PRINT_STREAM, "\tprincipal variation search\n");
#else
	fprintf(DEBUG_PRINT_STREAM, "\t[unknown]\n");
#endif
	fprintf(DEBUG_PRINT_STREAM, "features:\n");
#ifdef PIECE_SQUARE_TABLES
	fprintf(DEBUG_PRINT_STREAM, "\tpiece square tables\n");
#endif
#ifdef TRANSPOSITION_TABLES
	fprintf(DEBUG_PRINT_STREAM, "\ttransposition tables\n");
#endif
}

int
main(int argc, char* argv[])
{
	if (argc >= 2 && !strncmp(argv[1], "--about", 7)) {
		debug_print_chess_features();
		exit(0);
	}

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
