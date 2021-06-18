#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "devel_hashtable.h"
#include "unity.h"

void
test_hashfunction()
{
	printf("TEST: hash-function\t");
	/* Compare these two boards.
	  A  B  C  D  E  F  G  H                  A  B  C  D  E  F  G  H
00 8 [R][N][B][Q][K][ ][ ][R] 8       | 00 8 [R][N][B][Q][K][Q][ ][R] 8
08 7 [P][P][P][P][P][ ][B][P] 7         08 7 [P][P][P][P][P][ ][B][P] 7
16 6 [ ][ ][ ][ ][ ][ ][ ][N] 6         16 6 [ ][ ][ ][ ][ ][ ][ ][N] 6
24 5 [ ][ ][ ][N][ ][ ][ ][ ] 5         24 5 [ ][ ][ ][N][ ][ ][ ][ ] 5
32 4 [ ][ ][ ][ ][ ][ ][ ][ ] 4         32 4 [ ][ ][ ][ ][ ][ ][ ][ ] 4
40 3 [n][ ][n][n][ ][Q][ ][b] 3       | 40 3 [n][ ][n][ ][ ][n][ ][ ] 3
48 2 [p][b][q][ ][p][p][ ][p] 2       | 48 2 [p][b][q][ ][p][p][p][p] 2
56 1 [r][ ][ ][b][k][ ][q][r] 1       | 56 1 [r][ ][n][r][k][b][ ][r] 1
      A  B  C  D  E  F  G  H                  A  B  C  D  E  F  G  H
*/
	char* fen_1 = "RNBQK2R/PPPPP1BP/7N/3N4/8/n1nn1Q1b/pbq1pp1p/r2bk1qr w";
	char* fen_2 = "RNBQKQ1R/PPPPP1BP/7N/3N4/8/n1n2n2/pbq1pppp/r1nrkb1r w";

	struct chess game_1 = init_chess();
	struct chess game_2 = init_chess();

	fen_to_chess(fen_1, &game_1);
	fen_to_chess(fen_2, &game_2);

	ssize_t hash_1 =
			hash_board(TRANSPOSITION_TABLE_SIZE, game_1.board, game_1.moving);
	ssize_t hash_2 =
			hash_board(TRANSPOSITION_TABLE_SIZE, game_2.board, game_2.moving);

	TEST_ASSERT_MESSAGE(hash_1 != hash_2, "hashes of different board collide.");
}

void
test_hashtable()
{
	RUN_TEST(test_hashfunction);
}
