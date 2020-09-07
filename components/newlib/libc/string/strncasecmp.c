/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	int r = 0;

	while (n
			&& ((s1 == s2) ||
				!(r = ((int)(tolower(*s1)))
					- tolower(*s2)))
			&& (--n, ++s2, *s1++));

	return r;
}
