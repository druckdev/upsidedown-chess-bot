#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "board.h"
#include "chess.h"
#include "generator.h"
#include "helper.h"
#include "move.h"
#include "devel_bot.h"

#define N_FOR_AVG 3
#define BENCHMARK_ITERATION_COUNT 10000
#define CSV_STREAM stderr

size_t sample_size = sizeof(test_boards) / sizeof(*test_boards);

void
benchmark_generate_moves()
{
	printf("Benchmarking function generate_moves()...\n");

	double* cpu_secs[sample_size];
	size_t* cpu_nsecs[sample_size];
	double* wall_secs[sample_size];
	size_t* wall_nsecs[sample_size];

	for (size_t i = 0; i < sample_size; ++i) {
		//printf("BENCHMARK: %s \n", test_boards[i].fen);
		// Reserve for average too
		cpu_secs[i]   = calloc((N_FOR_AVG + 1), sizeof(**cpu_secs));
		cpu_nsecs[i]  = calloc((N_FOR_AVG + 1), sizeof(**cpu_nsecs));
		wall_secs[i]  = calloc((N_FOR_AVG + 1), sizeof(**wall_secs));
		wall_nsecs[i] = calloc((N_FOR_AVG + 1), sizeof(**wall_nsecs));

		// Init game
		struct chess chess;
		chess.board = calloc(64, sizeof(*chess.board));
		fen_to_chess(test_boards[i].fen, &chess);

		//print_board(chess.board, NULL);

		struct move_list* moves;
		struct timespec t_start_cpu, t_end_cpu, t_start_wall, t_end_wall;

		for (size_t j = 0; j < N_FOR_AVG; ++j) {
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu); // CPU time
			clock_gettime(CLOCK_MONOTONIC, &t_start_wall); // "actual" time

			for (size_t k = 0; k < BENCHMARK_ITERATION_COUNT; ++k) {
				/* functions to benchmark */

				moves = generate_moves(&chess, true, false);
				if (k != BENCHMARK_ITERATION_COUNT - 1)
					move_list_free(moves);

				/* \functions to benchmark */
			}

			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end_cpu);
			clock_gettime(CLOCK_MONOTONIC, &t_end_wall);

			double cpu_sec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) +
							 (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec) * 1e-9;
			size_t cpu_nsec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) * 1e9 +
							  (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec);
			cpu_secs[i][j]  = cpu_sec;
			cpu_nsecs[i][j] = cpu_nsec;
			cpu_secs[i][N_FOR_AVG] += cpu_sec;
			cpu_nsecs[i][N_FOR_AVG] += cpu_nsec;

			double wall_sec =
					(t_end_wall.tv_sec - t_start_wall.tv_sec) +
					(t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
			size_t wall_nsec = (t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
							   (t_end_wall.tv_nsec - t_start_wall.tv_nsec);
			wall_secs[i][j]  = wall_sec;
			wall_nsecs[i][j] = wall_nsec;
			wall_secs[i][N_FOR_AVG] += wall_sec;
			wall_nsecs[i][N_FOR_AVG] += wall_nsec;

#if 0
			printf("Generated moves: %li\n", moves->count);
			printf("Elapsed CPU-time over %i iterations:  %lf sec, %li nsec\n",
			       BENCHMARK_ITERATION_COUNT, cpu_sec, cpu_nsec);
			printf("Elapsed wall-time over %i iterations: %lf sec, %li nsec\n",
			       BENCHMARK_ITERATION_COUNT, wall_sec, wall_nsec);
#endif
		}

		cpu_secs[i][N_FOR_AVG] /= N_FOR_AVG;
		cpu_nsecs[i][N_FOR_AVG] /= N_FOR_AVG;
		wall_secs[i][N_FOR_AVG] /= N_FOR_AVG;
		wall_nsecs[i][N_FOR_AVG] /= N_FOR_AVG;

#if 0
		printf("Average CPU-time (%ix):  %lf sec, %li nsec\n", N_FOR_AVG,
		       cpu_secs[i][N_FOR_AVG], cpu_nsecs[i][N_FOR_AVG]);
		printf("Average wall-time (%ix): %lf sec, %li nsec\n", N_FOR_AVG,
		       wall_secs[i][N_FOR_AVG], wall_nsecs[i][N_FOR_AVG]);

		printf("\n-------------------------------------------\n\n");
#endif

		free(chess.board);
	}

	//printf("Summary:\n");
	fprintf(CSV_STREAM, "FEN;cpu secs;cpu nsecs;wall secs; wall nsecs\n");
	for (size_t i = 0; i < sample_size; ++i) {
		fprintf(CSV_STREAM, "%s;%lf;%li;%lf;%li\n", test_boards[i].fen,
		        cpu_secs[i][N_FOR_AVG], cpu_nsecs[i][N_FOR_AVG],
		        wall_secs[i][N_FOR_AVG], wall_nsecs[i][N_FOR_AVG]);

		free(cpu_secs[i]);
		free(cpu_nsecs[i]);
		free(wall_secs[i]);
		free(wall_nsecs[i]);
	}
}

void
benchmark_negamax()
{
	printf("Benchmarking function negamax()...\n");

	double cpu_secs[sample_size];
	size_t cpu_nsecs[sample_size];
	double wall_secs[sample_size];
	size_t wall_nsecs[sample_size];

	struct chess game = init_chess();
	for (size_t i = 0; i < sample_size; ++i) {
		fen_to_chess(test_boards[i].fen, &game);

		struct timespec t_start_cpu, t_end_cpu, t_start_wall, t_end_wall;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu); // CPU time
		clock_gettime(CLOCK_MONOTONIC, &t_start_wall); // "actual" time

		for (size_t j = 0; j < BENCHMARK_ITERATION_COUNT; ++j)
			negamax(&game, 15, INT_MIN + 1, INT_MAX);

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end_cpu);
		clock_gettime(CLOCK_MONOTONIC, &t_end_wall);

		double cpu_sec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) +
			(t_end_cpu.tv_nsec - t_start_cpu.tv_nsec) * 1e-9;
		size_t cpu_nsec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) * 1e9 +
			(t_end_cpu.tv_nsec - t_start_cpu.tv_nsec);
		cpu_secs[i] = cpu_sec;
		cpu_secs[i] /= BENCHMARK_ITERATION_COUNT;
		cpu_nsecs[i] = cpu_nsec;
		cpu_nsecs[i] /= BENCHMARK_ITERATION_COUNT;

		double wall_sec =
			(t_end_wall.tv_sec - t_start_wall.tv_sec) +
			(t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
		size_t wall_nsec = (t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
			(t_end_wall.tv_nsec - t_start_wall.tv_nsec);
		wall_secs[i] = wall_sec;
		wall_secs[i] /= BENCHMARK_ITERATION_COUNT;
		wall_nsecs[i] = wall_nsec;
		wall_nsecs[i] /= BENCHMARK_ITERATION_COUNT;
	}

	fprintf(CSV_STREAM, "FEN;cpu secs;cpu nsecs;wall secs; wall nsecs\n");
	for (size_t i = 0; i < sample_size; ++i) {
		fprintf(CSV_STREAM, "%s;%lf;%li;%lf;%li\n", test_boards[i].fen,
		        cpu_secs[i], cpu_nsecs[i], wall_secs[i], wall_nsecs[i]);
	}
}

int
main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	//benchmark_generate_moves();
	benchmark_negamax();
}
