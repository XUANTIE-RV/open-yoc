/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STREAM_ALL_H__
#define __STREAM_ALL_H__

#include <aos/aos.h>

__BEGIN_DECLS__

/**
 * @brief  regist stream for memory
 * @return 0/-1
 */
int stream_register_mem();

/**
 * @brief  regist stream for file
 * @return 0/-1
 */
int stream_register_file();

/**
 * @brief  regist stream for http
 * @return 0/-1
 */
int stream_register_http();

/**
 * @brief  regist stream for fifo
 * @return 0/-1
 */
int stream_register_fifo();

/**
 * @brief  regist all streamer
 * @return 0/-1
 */
static inline int stream_register_all()
{
    stream_register_mem();
    stream_register_file();
    stream_register_http();
    stream_register_fifo();

    return 0;
}

__END_DECLS__

#endif /* __STREAM_ALL_H__ */

