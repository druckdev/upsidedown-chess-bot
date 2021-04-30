#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include "board.h"

struct test_case {
	char* fen;
	enum COLOR moving;
	int move_cnt;
};

#endif /* UNIT_TEST_H */