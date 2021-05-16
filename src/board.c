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

bool is_set_at(U64 bitboard, enum POS pos);

/*----------------------------------
 * Helper
 * --------------------------------*/

char
get_piece_char(struct board* board, enum POS pos) 
{
    char res;

    if (is_set_at(board->knights, pos))
        res = 'n';

    if (is_set_at(board->pawns, pos))
        res = 'p';

    if (is_set_at(board->bishops, pos))
        res = 'b';

    if (is_set_at(board->kings, pos))
        res = 'k';

    if (is_set_at(board->queens, pos))
        res = 'q';

    if (is_set_at(board->rooks, pos))
        res = 'r';
    
    if (is_set_at(board->white_pieces, pos)) {    
        res -= 32; // ascii distance between lower and upper case
    }

    return res;
}

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
set_boards_zero(struct board* board)
{
    board->white_pieces = 0;
    board->black_pieces = 0;
    board->knights = 0;
    board->pawns = 0;
    board->bishops = 0;
    board->kings = 0;
    board->queens = 0;
    board->rooks = 0;	
}

/*
 * Fills `str` with the corresponding human-readable string (null-terminated)
 * describing the field encoded in `pos`.
 * `str` should be at least (3 * sizeof(char)) big.
 *
 * Returns: `str`
 */
char*
pos_to_str(enum POS pos, char* str)
{
	str[0] = 'H' - pos % 8;
	str[1] = '1' + pos / 8;
	str[2] = '\0';

	return str;
}

/*----------------------------------
 * Exposed functions
 * --------------------------------*/

void
fen_to_game(char* fen, struct chess* game)
{
    int i = -1, pos = A8; // fen string (is counted up immediately, hence -1) and board iterator

    set_boards_zero(&(game->board));

    // set bitboards
    while (fen[++i] && pos > MIN) {
		if (fen[i] >= '0' && fen[i] <= '9') {
			// Skip number of fields indicated by number in fen[i].
			pos -= atoi(&fen[i]);
		} 
        else if (fen[i] != '/') {
			set_board_from_char(fen[i], pos, &(game->board));
            pos--;
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
    U64 max = one << 63; 

    for (int i = A8; i > MIN ;i--) {
        if (i % WIDTH == 7)
            printf("\n");
        
        printf("%u ", board & max ? 1 : 0); // print msb
        board = board << 1;
    }
}

void
print_board(struct board* board)
{        
    U64 all = board->black_pieces|board->white_pieces;
    
    printf("\n ");
    // print column name A-H 
    for (char col = 'A'; col < 'A'+WIDTH; col++) {
        printf(" %c ", col);
    }

    // print board
    for (int i = A8; i > MIN ;i--) {
        if (i % WIDTH == 7)
            printf("\n%d", ((int)i / WIDTH)+1);
        
        printf("[");
        if (is_set_at(all, i)) {
            printf("%c", get_piece_char(board, i));
        }
        else {
            printf(" ");
        }
        printf("]");
    }
}

void 
print_moves(struct list* moves)
{
    struct list_elem* cur = moves->first;
    struct move* move = (struct move*)cur->object;
	
    printf("Moves:\n");
	while (cur) {
		struct move* move = (struct move*)cur->object;

        char start[3], target[3];
        pos_to_str(move->start, start);
        pos_to_str(move->target, target);
		printf("%s -> %s\n", start, target);
		
        cur = cur->next;
	}
}

bool 
is_set_at(U64 bitboard, enum POS pos)
{
    U64 one = 1; // helper, outsource later
    
    if (bitboard >> pos & one) {
        return true;
    }
    return false;
}