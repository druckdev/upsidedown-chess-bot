#include "types.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

#define ANSI_RED "\033[91m"
#define ANSI_RESET "\033[0m"

void
set_board_from_char(char fen_piece, enum POS pos, struct board* board)
{
    U64 one = 1; // helper, outsource later

    switch (fen_piece) {
	case 'p':
	case 'P':
		board->pawns |= one << pos;
		break;
	case 'b':
	case 'B':
		board->bishops |= one << pos;
		break;
	case 'n':
	case 'N':
		board->knights |= one << pos;
		break;
	case 'r':
	case 'R':
		board->rooks |= one << pos;
		break;
	case 'q':
	case 'Q':
		board->queens |= one << pos;
		break;
	case 'k':
	case 'K':
		board->kings |= one << pos;
		break;
	default:
		break;
	}

    if (fen_piece >= 'a' && fen_piece <= 'z') {
        board->black_pieces |= one << pos;
    }
    else {
        board->white_pieces |= one << pos;
    } 
}

void
fen_to_game(char* fen, struct chess* game)
{
    int i = -1, pos = 0; // fen string (is counted up immediately, hence -1) and board iterator

    // set bitboards
    while (fen[++i] && pos < 64) {
		if (fen[i] >= '0' && fen[i] <= '9') {
			// Skip number of fields indicated by number in fen[i].
			pos += atoi(&fen[i]);
		} 
        else if (fen[i] != '/') {
			set_board_from_char(fen[i], pos, &(game->board));
            pos++;
		}
	}

    // set starting player
    while (fen[++i]) {
		if (fen[i] == 'w')
			game->moving = WHITE;
		else if (fen[i] == 'b')
			game->moving = BLACK;
	}
}

void 
print_bitboard(U64 board)
{
    U64 one = 1; // helper, outsource later
    for (int i = 0; i < 64 ;i++) {
        if (i % WIDTH == 0)
            printf("\n");
        
        printf("%u ", board & one ? 1 : 0); // print lsb
        board = board >> 1;
    }
}