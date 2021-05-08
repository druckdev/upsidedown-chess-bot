#ifndef HELPER_H
#define HELPER_H

#include "chess.h" // for enum COLOR

struct game_samples {
	char* fen;
	enum COLOR moving;
	int move_cnt;
};

static struct game_samples test_boards[] = {
	// Test single figures
	{ "8/8/8/8/8/3k4/8/8", BLACK, 8 },  // test king
	{ "8/8/8/8/3r4/8/8/8", BLACK, 14 }, // test rook
	{ "8/8/8/8/3b4/8/8/8", BLACK, 13 }, // test bishop
	{ "8/8/8/8/8/8/6p1/8", BLACK, 4 },  // test pawn (black)
	{ "8/6P1/8/8/8/8/8/8", WHITE, 4 },  // test pawn (white)
	{ "8/8/8/8/8/6p1/8/8", BLACK, 1 },  // test pawn (black)
	{ "8/8/6P1/8/8/8/8/8", WHITE, 1 },  // test pawn (white)
	{ "8/8/8/8/3q4/8/8/8", BLACK, 27 }, // test queen
	{ "8/8/8/8/3n4/8/8/8", BLACK, 8 },  // test knight

	// Test start board
	{ "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", WHITE, 4 },
	{ "RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr", BLACK, 4 },

	// Test different boards
	{ "RNBKQB1R/PPPPPPPP/8/8/4N3/2n5/pppppppp/r1bkqbnr", BLACK, 12 },
	{ "RqBQKB1R/P1PPPPQP/2N2N2/8/8/4n3/p1pppppp/r1bqkbnr", WHITE, 29 },
	{ "RNBQKB1R/PPPPPPPP/5N2/8/8/5n2/pppppppp/rnbqkb1r", WHITE, 12 },
	{ "RNBQKBQR/PPPPPP1P/5N2/8/8/5n2/pppppp1p/rnbqkbqr", WHITE, 15 },
	{ "RQBQKB1R/P1PPPPPP/2N2N2/8/5n2/n7/pppppppp/r1bqkb1r", WHITE, 22 },
	{ "RNBQK2R/PPPPPPBP/7N/3N4/8/n1nn1n2/pbqppppp/r3kb1r", BLACK, 38 },
	{ "RNBKQB1R/PPPPPPPP/8/3N4/8/5n2/pppppp1p/rnbkqbrr", WHITE, 12 },
	{ "qRQQK1QR/3PPP1P/5N1B/4Q3/1b6/1Bn5/3ppp1p/1rqqkbqr", WHITE, 61 },
	{ "4K2R/4P2P/8/q1N5/8/6B1/p1k3q1/r1bR4", WHITE, 32 },
	{ "RNQQKB1r/PP1PPPP1/8/8/8/8/pp1ppp2/r1qkb1q1", WHITE, 18 },
	{ "3QK3/4Q1P1/8/8/8/1rq1n3/2n5/2k5", WHITE, 33 },
	{ "3R4/3R4/8/7K/7B/8/3r2n1/3k4", BLACK, 10 },
	{ "6KR/8/1N6/8/8/3q4/4b3/6kr", WHITE, 15 },

	{ "R1QQKBNR/PP1PPPPP/2N5/3b1B2/4q3/2n1n3/pppppppp/r1b1k2r", WHITE, 22 },

	{ "K7/2r5/1q6/8/8/8/8/8", BLACK, 31 },  // check checkmate
	{ "8/8/8/8/8/8/8/3K1k2", WHITE, 3 },    // check weird king interaction 1
	{ "8/8/8/8/8/4R3/8/3K1k2", WHITE, 15 }, // check weird king interaction 2

	{ "8/8/8/8/8/8/2p3p1/3k4", BLACK, 12 }, // Check pawn promotion (ally king)
	{ "8/8/8/8/8/K7/2p5/8", BLACK, 2 },     // Check pawn promotion (q, b)
	{ "8/8/8/8/8/8/K1p5/8", BLACK, 3 },     // Check pawn promotion (n)
	{ "8/8/8/8/8/8/2p5/K7", BLACK, 2 },     // Check pawn promotion (q, r)
	{ "8/8/8/8/8/8/3p4/2RRR3", BLACK, 8 },  // Check that pawn hits right fields

	// Check proper `targets` buildup
	{ "3k4/4P3/8/2Q5/3R4/8/8/8", BLACK, 1 },
	{ "8/1K6/8/8/8/8/p7/r7", WHITE, 8 },

	/* Invalid boards */
	{ "r1bqkb1r/pppppppp/2n2n2/8/8/2N2N2/PPPPPPPP/R1BQKB1R", WHITE, 18 },
	{ "r1bqkbqr/pppppp1p/2n2n2/8/8/2N2N2/P1PPPPPP/RQBQKB1R", WHITE, 23 },
	{ "r1bqk1nr/pppppp1p/2k4b/8/8/2N2N2/P1PPPP1P/RQBQKB1R", WHITE, 19 },
	{ "r1bqkqnr/ppppp2p/2n4b/3N4/8/5N2/P1PPPP1P/RQBQKB1R", WHITE, 27 },
};
#endif /* HELPER_H */
