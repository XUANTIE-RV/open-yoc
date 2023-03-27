/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __EX_CJSON_H__
#define __EX_CJSON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C"
{
#endif

cJSON *cJSON_GetObjectItemByPath(cJSON *object, const char *path);
int json_string_eq(cJSON *js, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* __EX_CJSON_H__ */

