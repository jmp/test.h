/*
 * This is a set of macros/functions for testing small C programs.
 * It's nothing fancy, but good enough for my own purposes.
 * To start writing some tests, simply include this header file.
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

/* Define a test case. */
#define TEST(test_name) \
	static void test_name(void)

/* Inside a test case, check a condition. */
#define test_assert(expr) \
	if (!(expr)) { \
		++test_fail_count; \
		test_fail_expr = #expr; \
		test_fail_file = __FILE__; \
		test_fail_line = __LINE__; \
		test_malloc_enable(); \
		test_realloc_enable(); \
		return; \
	}

/* Run the given test. */
#define test_run(test_name) { \
		const int prev_fail_count = test_fail_count; \
		(test_name)(); \
		if (prev_fail_count == test_fail_count) { \
			++test_pass_count; \
			fprintf(stdout, "OK	" #test_name "\n"); \
		} else { \
			fprintf(stderr, "FAIL  " #test_name "\n\n"); \
			fprintf(stderr, "      In file %s, line %d:\n\n", \
				test_fail_file, test_fail_line); \
			fprintf(stderr, "          %s\n\n", test_fail_expr); \
			fprintf(stderr, "      Assertion of the above expression" \
				" failed.\n\n"); \
		} \
	}

/* Print the total/passed/failed test counts. */
#define test_print_stats() \
	printf("\n%d total, %d passed, %d failed.\n", \
		test_pass_count + test_fail_count, \
		test_pass_count, test_fail_count);

/* Makes malloc return NULL after the given number of calls. */
#define test_malloc_disable_after(n) { \
		test_malloc_call_count = 0; \
		test_malloc_fail_after = (n); \
	}

/* Makes realloc return NULL after the given number of calls. */
#define test_realloc_disable_after(n) { \
		test_realloc_call_count = 0; \
		test_realloc_fail_after = (n); \
	}

/* Convenience macros for making malloc/realloc fail/succeed. */
#define test_malloc_enable() test_malloc_disable_after(-1)
#define test_realloc_enable() test_realloc_disable_after(-1)
#define test_malloc_disable() test_malloc_disable_after(0)
#define test_realloc_disable() test_realloc_disable_after(0)

/* Keep track of how many tests have passed and failed. */
int test_pass_count = 0;
int test_fail_count = 0;

/* The latest expression whose assertion failed. */
const char *test_fail_expr = NULL;
const char *test_fail_file = NULL;
int test_fail_line = 0;

/* Counters for the number of malloc/realloc calls made. */
int test_malloc_call_count = 0;
int test_realloc_call_count = 0;

/* These tell after how many calls malloc/realloc should fail. */
int test_malloc_fail_after = -1;
int test_realloc_fail_after = -1;

/* Replacement for malloc that fails after a certain amount of calls. */
void *test_malloc(size_t s)
{
	if (test_malloc_call_count++ < test_malloc_fail_after ||
		test_malloc_fail_after < 0) {
		return malloc(s);
	}
	return NULL;
}

/* Replacement for realloc that fails after a certain amount of calls. */
void *test_realloc(void *ptr, size_t s)
{
	if (test_realloc_call_count++ < test_realloc_fail_after ||
		test_realloc_fail_after < 0) {
		return realloc(ptr, s);
	}
	return NULL;
}

#endif /* TEST_H */
