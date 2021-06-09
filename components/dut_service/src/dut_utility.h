/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __DUT_UTILITY_H_
#define __DUT_UTILITY_H_
int str2_char(const char *str, uint8_t *addr);
int int_num_check(char *data);
char *char_cut(char *d, char *s, int b , int e);
int  argc_len(char *s);
char *str_chr(char *d, char *s, int c);
#endif