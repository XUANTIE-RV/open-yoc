/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "aui_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/**

    Those symbols, return true:
        0-9
        a-z
        A-Z
*/
static bool is_normal_symbol(char c)
{
    int c_int = (int)c;
    if (c == '\0') {
        return 1;
    }

    return (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) ||
           (c_int >= 97 && c_int <= 122);
}

/**
 * @brief  url encode
 * @param  [in] input
 * @param  [out] output
 * @return 0/-1
 */
int http_url_encode(char *output, const char *input)
{
    char encoded[4] = {0};
    char c;

    while (*input) {
        c = *input;

        if (c < 0) {
            input++;
        } else if (is_normal_symbol(c)) {
            *output++ = *input++;
        } else {
            snprintf(encoded, 4, "%%%02X", c);
            *output++ = encoded[0];
            *output++ = encoded[1];
            *output++ = encoded[2];
            input++;
        }
    }

    *output++ = 0;

    return 0;
}