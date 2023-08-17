/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include "ex_cjson.h"

cJSON *cJSON_GetObjectItemByPath(cJSON *object, const char *path)
{
    if (object == NULL) {
        return NULL;
    }

    char *dup_path = strdup(path);
    char *saveptr  = NULL;
    char *next     = strtok_r(dup_path, ".", &saveptr);

    cJSON *item = object;
    while (next) {
        char *arr_ptr = strchr(next, '[');
        if (arr_ptr) {
            *arr_ptr++ = '\0';
            item = cJSON_GetObjectItem(item, next);
            if (item) {
                item = cJSON_GetArrayItem(item, atoi(arr_ptr));
            }
        } else {
            item = cJSON_GetObjectItem(item, next);
        }
        if (item == NULL) {
            break;
        }

        next = strtok_r(NULL, ".", &saveptr);
    }

    free(dup_path);

    return item;
}

int json_string_eq(cJSON *js, const char *str)
{
    if (cJSON_IsString(js)) {
        if (strcmp(js->valuestring, str) == 0) {
            return 1;
        }
    }
    return 0;
}
