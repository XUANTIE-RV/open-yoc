/**
 * @file file.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_THREAD_H
#define CX_COMMON_THREAD_H

namespace cx {

    struct ThreadAttributes {
        int prio;
        int stackSize;
        const char *name;
    };
}

#endif /* CX_COMMON_THREAD_H */

