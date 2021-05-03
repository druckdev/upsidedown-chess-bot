#include <stdio.h>
#include <time.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "helper.h"

#define ITERATIONS 10000

int
main(int argc, char* argv[])
{
	printf("Benchmarking function generate_moves()...\n");

	// TODO(Aurel): Run multiple times and take average.

	for (size_t i = 0; i < sizeof(test_boards) / sizeof(*test_boards); ++i) {
		printf("BENCHMARK: %s \n", test_boards[i].fen);

		// Init game
		struct chess chess;
		chess.moving = test_boards[i].moving;
		board_from_fen(test_boards[i].fen, chess.board);

		print_board(chess.board, NULL);

		struct list* moves;
		struct timespec t_start_cpu, t_end_cpu, t_start_wall, t_end_wall;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu); // CPU time
		clock_gettime(CLOCK_MONOTONIC, &t_start_wall);         // "actual" time

		for (size_t j = 0; j < ITERATIONS; ++j) {
			/* functions to benchmark */

			moves = generate_moves(&chess, true);

			/* \functions to benchmark */
		}

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end_cpu);
		clock_gettime(CLOCK_MONOTONIC, &t_end_wall);

		double cpu_sec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) +
						 (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec) * 1e-9;
		size_t cpu_nsec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) * 1e9 +
						  (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec);

		double wall_sec = (t_end_wall.tv_sec - t_start_wall.tv_sec) +
						  (t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
		size_t wall_nsec = (t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
						   (t_end_wall.tv_nsec - t_start_wall.tv_nsec);

		printf("Generated moves:\t%li\n", moves->count);
		printf("Average elapsed CPU-time over %i iterations:  %lf sec, %li nsec\n",
		       ITERATIONS, cpu_sec, cpu_nsec);
		printf("Average elapsed wall-time over %i iterations: %lf sec, %li nsec\n",
		       ITERATIONS, wall_sec, wall_nsec);

		printf("\n-------------------------------------------\n\n");
	}
}
