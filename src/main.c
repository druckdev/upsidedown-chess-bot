#include <stdlib.h>

#include "bot.h"
#include "chess.h"

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
	if (argc > 1)
		MAX_NEGAMAX_DEPTH = atoi(argv[1]);

	run_chess();
}
