/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#ifndef TM_DEBUG_H
#define TM_DEBUG_H

#include <iostream>
#include <string>
#ifdef __linux__
#include <tmedia_config.h>
#endif

#define TEST_ASSERT(expr)                 \
    do {                                  \
        if (!(expr)) {                    \
            printf("\nASSERT failed at:\n"\
                   "  >File name: %s\n"   \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
            exit(-1);                     \
        } \
    } while(0)

#define DEBUG_LOG_CURRENT_POS                               \
    do {                                                    \
        printf("\nCurrent pos: Function:%s, Line No.:%d\n", \
               __FUNCTION__, __LINE__);                     \
    } while(0)

#define DEBUG_LOG_FUNC_ENTER()  LOG_D("Enter %s\n", __PRETTY_FUNCTION__)
#define DEBUG_LOG_FUNC_LEAVE()  LOG_D("Leave %s\n", __PRETTY_FUNCTION__)


#endif  /* TM_DEBUG_H */
