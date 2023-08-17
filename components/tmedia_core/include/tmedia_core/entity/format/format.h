/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_FORMAT_H
#define TM_FORMAT_H

#include <string>
#ifdef __linux__
#include <tmedia_config.h>
#endif

using namespace std;

class TMFormat
{
public:
    TMFormat() {}
    virtual ~TMFormat() {}
};

#endif  /* TM_FORMAT_H */
