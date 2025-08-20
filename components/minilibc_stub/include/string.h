
#ifndef _PRIV_STRING_H_
#define _PRIV_STRING_H_

#include <stddef.h>

char *strdup(const char *s);
size_t strlcpy(char *dest, const char *src, size_t len);

#include_next <string.h>

#endif