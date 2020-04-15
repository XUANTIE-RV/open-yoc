/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef STRING_H
#define STRING_H

#include <features.h>
#define __need_size_t
#include <stddef.h>

#ifndef NULL
#define NULL 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*  Copying functions */
extern void * memcpy( void *, const void *, size_t );
extern void * memmove( void *, const void *, size_t );
extern char * strcpy( char *, const char * );
extern char * strncpy( char *, const char *, size_t );

/*  Concatenation functions */
extern char * strcat( char *, const char * );
extern char * strncat( char *, const char *, size_t );

/*  Comparison functions */
extern int memcmp( const void *, const void *, size_t );
extern int strcmp( const char *, const char * );
extern int strcoll( const char *, const char * );
extern int strncmp( const char *, const char *, size_t );
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern size_t strxfrm( char *, const char *, size_t );

/*  Search functions */
extern void * memchr( const void *, int,  size_t );
extern char * strchr( const char *, int );
extern size_t strcspn( const char *, const char * );
extern char * strpbrk( const char *, const char * );
extern char * strrchr( const char *, int );
extern size_t strspn( const char *, const char * );
extern char * strstr( const char *, const char * );
extern char * strtok( char *, const char * );
extern char * strtok_r( char *, const char * , char **);

size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);

/*  Miscellaneous functions */
extern void * memset(void * buffer, int c, int count);
extern char * strerror(int errnum);
extern size_t strlen( const char * );

extern char *strdup(const char *s1);
extern char *strndup(const char *str, int n);
extern char *strsep(char **stringp, const char *delim);
extern void bzero(void *, size_t);
extern char *strlwr(char *str);
extern char *strupr(char *str);

//
#include <stdarg.h>
int vasprintf(char **buf, const char *fmt, va_list ap);
int asprintf(char **buf, const char *fmt, ...);

char *bytes2hexstr(char *str, size_t str_size, void *hex, size_t len);
int hexstr2bytes(char *str);

size_t strcount(const char *haystack, const char *needle);
int strsplit(char **array, size_t count, char *data, const char *delim);
char **strasplit(char *data, const char *delim, int *count);
int str2mac(const char *str, unsigned char mac[]);
const char *strstri(const char *str, const char *substr);

#ifdef __cplusplus
}   /* extern "C" */
#endif



#endif /* MINILIBC_STRING_H */

