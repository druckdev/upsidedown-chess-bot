#include <stdio.h>
#include <time.h>

#include "board.h"
#include "chess.h"
#include "generator.h"

#define ITERATIONS 100000

int
main(int argc, char* argv[])
{
	// clang-format off

	struct chess chess_games[] = {
		init_chess(WHITE),
		{{{ ROOK, WHITE }, { EMPTY, WHITE }, { QUEEN,   WHITE }, { QUEEN, WHITE }, { KING, WHITE }, { BISHOP, WHITE }, { KNIGHT, WHITE }, { ROOK, WHITE },
		 { PAWN, WHITE }, { PAWN,   WHITE }, { EMPTY,   WHITE }, { PAWN,  WHITE }, { PAWN, WHITE }, { PAWN,   WHITE }, { PAWN,   WHITE }, { PAWN, WHITE },

		 [18] = { KNIGHT, WHITE }, { PAWN,  WHITE },
		 [27] = { BISHOP, BLACK }, { EMPTY, WHITE }, { BISHOP, WHITE },
		 [36] = { QUEEN,  BLACK },
		 [42] = { KNIGHT, BLACK }, [44] = { KNIGHT, BLACK},

		 [48] = { PAWN, BLACK }, { PAWN,   BLACK }, { PAWN,   BLACK }, { PAWN,  BLACK }, { PAWN, BLACK }, { PAWN,   BLACK }, { PAWN,   BLACK }, { PAWN, BLACK },
		 { ROOK, BLACK }, { EMPTY,  WHITE }, { BISHOP, BLACK }, { EMPTY, WHITE }, { KING, BLACK }, { EMPTY,  WHITE }, { EMPTY,  WHITE }, { ROOK, BLACK }},
		WHITE, 0, 0 },
	};
	// clang-format on
	printf("Benchmarking function generate_moves()...\n");

	// TODO(Aurel): Run multiple times and take average.

	for (size_t i = 0; i < sizeof(chess_games) / sizeof(*chess_games); ++i) {
		printf("\nBenchmarking board");
		print_board(chess_games[i].board, NULL);

		double t_elapsed_cpu_sec;
		size_t t_elapsed_cpu_nanosec;
		double t_elapsed_wall_sec;
		size_t t_elapsed_wall_nanosec;

		struct list* moves;
		struct timespec t_start_cpu, t_end_cpu;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu);
		// benchmark wall-time ("actual" time)
		struct timespec t_start_wall, t_end_wall;

		clock_gettime(CLOCK_MONOTONIC, &t_start_wall);
		for (size_t j = 0; j < ITERATIONS; ++j) {
			// benchmark CPU-time
			/* functions to benchmark */
			moves = generate_moves(&chess_games[i], true);
			/* \functions to benchmark */
		}
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end_cpu);

		t_elapsed_cpu_sec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) +
							(t_end_cpu.tv_nsec - t_start_cpu.tv_nsec) * 1e-9;
		t_elapsed_cpu_nanosec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) * 1e9 +
								(t_end_cpu.tv_nsec - t_start_cpu.tv_nsec);

		clock_gettime(CLOCK_MONOTONIC, &t_end_wall);
		t_elapsed_wall_sec = (t_end_wall.tv_sec - t_start_wall.tv_sec) +
							 (t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
		t_elapsed_wall_nanosec =
				(t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
				(t_end_wall.tv_nsec - t_start_wall.tv_nsec);

		printf("Generated moves:\t%li\n", moves->count);
		printf("Average elapsed CPU-time over %i iterations:\t%lf s\t%li ns\nAverage elapsed wall-time over %i iterations:\t%lf s\t%li ns\n",
		       ITERATIONS, t_elapsed_cpu_sec, t_elapsed_cpu_nanosec, ITERATIONS,
		       t_elapsed_wall_sec, t_elapsed_wall_nanosec);
	}
}
