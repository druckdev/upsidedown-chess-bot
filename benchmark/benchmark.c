#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "list.h"
#include "board.h"
#include "chess.h"
#include "devel_bot.h"
#include "generator.h"
#include "helper.h"
#include "move.h"

#define BENCHMARK_FILE_NAME_SIZE 40
#define BENCHMARK_DIR_NAME "benchmarks"

#define N_FOR_AVG 3
#define BENCHMARK_ITERATION_COUNT 100
#define NEGAMAX_DEPTH 5

size_t sample_size = sizeof(test_boards) / sizeof(*test_boards);

void
summarize_benchmark(FILE* file, char* benchmark, double cpu_secs[],
                    size_t cpu_nsecs[], double wall_secs[], size_t wall_nsecs[])
{
	fprintf(stdout, "FEN;cpu secs;cpu nsecs;wall secs; wall nsecs\n");
	fprintf(file, "%s\n", benchmark);
	for (size_t i = 0; i < sample_size; ++i) {
		fprintf(stdout, "%s;%lf;%li;%lf;%li\n", test_boards[i].fen, cpu_secs[i],
		        cpu_nsecs[i], wall_secs[i], wall_nsecs[i]);

		if (file)
			fprintf(file, "%s;%lf;%li;%lf;%li\n", test_boards[i].fen,
			        cpu_secs[i], cpu_nsecs[i], wall_secs[i], wall_nsecs[i]);
	}
	if (file)
		fprintf(file, "\n");
}

void
benchmark_generate_moves(FILE* file)
{
	printf("Benchmarking function generate_moves()...\n");

	double cpu_secs[sample_size];
	size_t cpu_nsecs[sample_size];
	double wall_secs[sample_size];
	size_t wall_nsecs[sample_size];

	// Init game
	struct chess chess = init_chess(' ', 9999, 99);
	for (size_t i = 0; i < sample_size; ++i) {
		fen_to_chess(test_boards[i].fen, &chess);

		//printf("BENCHMARK: %s \n", test_boards[i].fen);
		//fprint_board(stdout, chess.board, NULL);

		struct list* moves;
		struct timespec t_start_cpu, t_end_cpu, t_start_wall, t_end_wall;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu); // CPU time
		clock_gettime(CLOCK_MONOTONIC, &t_start_wall);         // "actual" time

		for (size_t j = 0; j < BENCHMARK_ITERATION_COUNT; ++j) {
			moves = generate_moves(&chess, true, false);
			if (j != BENCHMARK_ITERATION_COUNT - 1)
				list_free(moves);
		}

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end_cpu);
		clock_gettime(CLOCK_MONOTONIC, &t_end_wall);

		double cpu_sec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) +
						 (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec) * 1e-9;
		size_t cpu_nsec = (t_end_cpu.tv_sec - t_start_cpu.tv_sec) * 1e9 +
						  (t_end_cpu.tv_nsec - t_start_cpu.tv_nsec);
		cpu_secs[i] += cpu_sec;
		cpu_secs[i] /= BENCHMARK_ITERATION_COUNT;
		cpu_nsecs[i] += cpu_nsec;
		cpu_nsecs[i] /= BENCHMARK_ITERATION_COUNT;

		double wall_sec = (t_end_wall.tv_sec - t_start_wall.tv_sec) +
						  (t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
		size_t wall_nsec = (t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
						   (t_end_wall.tv_nsec - t_start_wall.tv_nsec);
		wall_secs[i] += wall_sec;
		wall_secs[i] /= BENCHMARK_ITERATION_COUNT;
		wall_nsecs[i] += wall_nsec;
		wall_nsecs[i] /= BENCHMARK_ITERATION_COUNT;
	}
	free(chess.board);

	summarize_benchmark(file, "generate_moves\0", cpu_secs, cpu_nsecs,
	                    wall_secs, wall_nsecs);
}

void
benchmark_negamax(FILE* file)
{
	printf("Benchmarking function negamax()...\n");

	double cpu_secs[sample_size];
	size_t cpu_nsecs[sample_size];
	double wall_secs[sample_size];
	size_t wall_nsecs[sample_size];

	struct chess game = init_chess(' ', 9999, 99);
	struct timespec t_start_cpu, t_end_cpu, t_start_wall, t_end_wall;

	for (size_t i = 0; i < sample_size; ++i) {
		fen_to_chess(test_boards[i].fen, &game);

		printf("test %lu: %s\n", i, test_boards[i].fen);
		fprint_board(stdout, game.board, NULL);

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start_cpu); // CPU time
		clock_gettime(CLOCK_MONOTONIC, &t_start_wall);         // "actual" time

		for (size_t j = 0; j < BENCHMARK_ITERATION_COUNT; ++j)
			negamax(&game, NEGAMAX_DEPTH, INT_MIN + 1, INT_MAX);

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

		double wall_sec = (t_end_wall.tv_sec - t_start_wall.tv_sec) +
						  (t_end_wall.tv_nsec - t_start_wall.tv_nsec) * 1e-9;
		size_t wall_nsec = (t_end_wall.tv_sec - t_start_wall.tv_sec) * 1e9 +
						   (t_end_wall.tv_nsec - t_start_wall.tv_nsec);
		wall_secs[i] = wall_sec;
		wall_secs[i] /= BENCHMARK_ITERATION_COUNT;
		wall_nsecs[i] = wall_nsec;
		wall_nsecs[i] /= BENCHMARK_ITERATION_COUNT;

		printf("\n");
	}
	free(game.board);

	summarize_benchmark(file, "negamax\0", cpu_secs, cpu_nsecs, wall_secs,
	                    wall_nsecs);
}

int
main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	struct stat st = { 0 };
	// create dir if it does not exist
	if (stat(BENCHMARK_DIR_NAME, &st) == -1)
		mkdir(BENCHMARK_DIR_NAME, 0777);

	char* fname = malloc(BENCHMARK_FILE_NAME_SIZE);
	snprintf(fname, BENCHMARK_FILE_NAME_SIZE, "%s/benchmark%lu.csv",
	         BENCHMARK_DIR_NAME, time(NULL));

	// open file
	FILE* file = fopen(fname, "w");

	free(fname);

	benchmark_generate_moves(file);
	benchmark_negamax(file);

	fflush(file);
	fclose(file);
}
