#include <stdio.h>

#include "unity.h"

#include "test_bot.h"
#include "test_generator.h"

// Necessary for Unity
// clang-format off
void setUp(void)    {}
void tearDown(void) {}
// clang-format on

// TODO(Luis): use generate_test_runner.rb
int
main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	UNITY_BEGIN();
	test_generator();
	test_bot();
	return UNITY_END();
}
