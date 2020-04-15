/*
 * vfprintf.c - the file contains the functions vfprintf.
 *
 * Copyright (C): 2012 Hangzhou C-SKY Microsystem Co.,LTD.
 * Author: yu chaojun  (chaojun_yu@c-sky.com)
 * Date: 2012-5-9
 */
#include <stdarg.h>
#include <stdlib.h>
#include "serf/minilibc_stdio.h"

static int __fwrite(void*ptr, size_t nmemb, FILE* f) { 	
  return fwrite(ptr,1,nmemb,f);
}

//libc_hidden_proto(vfprintf)
int vfprintf(FILE *stream, const char *format, va_list arg_ptr)
{
	struct arg_printf ap = { stream, (int(*)(void*,size_t,void*)) __fwrite };
	return yoc__v_printf(&ap,format,arg_ptr);
}
//libc_hidden_weak(vfprintf)

