/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>
#include "avutil/path.h"

/**
 * @brief  get the file extension
 * @param  [in] filename
 * @return NULL when err
 */
const char *get_extension(const char *filename)
{
    const char *ext;

    if (filename) {
        ext = filename + strlen(filename) - 1;
        while (ext >= filename && *ext != '/') {
            if (*ext == '.') {
                ext++;
                return ext;
            }
            ext--;
        }
    }

    return NULL;
}

/**
 * @brief  get basename from the path
 * @param  [in] path
 * @return NULL when err
 */
const char *path_basename(const char *path)
{
    if (path) {
        const char *f;

        f = strrchr(path, '/');

        return f ? f + 1 : path;
    }

    return NULL;
}


