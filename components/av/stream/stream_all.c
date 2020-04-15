/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "stream/stream.h"
#include "stream/stream_all.h"

/**
 * @brief  regist stream for memory
 * @return 0/-1
 */
int stream_register_mem()
{
    extern struct stream_ops stream_ops_mem;
    return stream_ops_register(&stream_ops_mem);
}

/**
 * @brief  regist stream for file
 * @return 0/-1
 */
int stream_register_file()
{
    extern struct stream_ops stream_ops_file;
    return stream_ops_register(&stream_ops_file);
}

/**
 * @brief  regist stream for http
 * @return 0/-1
 */
int stream_register_http()
{
    extern struct stream_ops stream_ops_http;
    return stream_ops_register(&stream_ops_http);
}

/**
 * @brief  regist stream for fifo
 * @return 0/-1
 */
int stream_register_fifo()
{
    extern struct stream_ops stream_ops_fifo;
    return stream_ops_register(&stream_ops_fifo);
}




