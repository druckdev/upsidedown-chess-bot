#include <stdlib.h>

#include "bot.h"
#include "chess.h"

int
main(int argc, char* argv[])
{
	if (argc > 1)
		MAX_NEGAMAX_DEPTH = atoi(argv[1]);

	run_chess();
}
