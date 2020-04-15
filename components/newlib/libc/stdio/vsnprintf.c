/*
 * vsnprintf.c - the file contains the functions vsnprintf.
 *
 * Copyright (C): 2012 Hangzhou C-SKY Microsystem Co.,LTD.
 * Author: yu chaojun  (chaojun_yu@c-sky.com)
 * Date: 2012-5-9
 */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "serf/minilibc_stdio.h"

struct str_data {
  char* str;
  size_t len;
  size_t size;
};

static int swrite(void*ptr, size_t nmemb, struct str_data* sd) {
  size_t tmp=sd->size-sd->len;
  if (tmp>0) {
    size_t len=nmemb;
    if (len>tmp) len=tmp;
    if (sd->str) {
      memcpy(sd->str+sd->len,ptr,len);
      sd->str[sd->len+len]=0;
    }
    sd->len+=len;
  }
  return nmemb;
}

//libc_hidden_proto(vsnprintf)
int vsnprintf(char* str, size_t size, const char *format, va_list arg_ptr) {
  int n;
  struct str_data sd = { str, 0, size?size-1:0 };
  struct arg_printf ap = { &sd, (int(*)(void*,size_t,void*)) swrite };
  n=yoc__v_printf(&ap,format,arg_ptr);
  if (str && size && n>=0) {
    if (size!=(size_t)-1 && ((size_t)n>=size)) str[size-1]=0;
    else str[n]=0;
  }
  return n;
}
//libc_hidden_weak(vsnprintf)
