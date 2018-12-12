#ifndef _MY_ASSERT_
#define _MY_ASSERT_

#include <windows.h>
#include <cstdio>

static FILE * assert_fp;

#define my_assert(exp) \
( \
	static_cast<void> \
	( \
		(exp) || \
		( \
			assert_fp = fopen("assert-info.txt", "w"), \
			fprintf(assert_fp, "Assertion Failed!\n"), \
			fprintf(assert_fp, "%s(%d): %s\n", __FILE__, __LINE__, #exp), \
			fclose(assert_fp), \
			system("notepad assert-info.txt"), \
			exit(-1), \
			0 \
		) \
	) \
)

#endif