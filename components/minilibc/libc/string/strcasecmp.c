/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int strcasecmp(const char *s1, const char *s2)
{
	int r = 0;

	while (((s1 == s2) ||
			!(r = ((int)(tolower(*s1)))
				- tolower(*s2)))
			&& (++s2, *s1++));

	return r;
}
