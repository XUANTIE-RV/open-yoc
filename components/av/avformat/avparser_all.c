/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avformat/avparser.h"
#include "avformat/avparser_all.h"

/**
 * @brief  regist avparser for mp3
 * @return 0/-1
 */
int avparser_register_mp3()
{
    extern struct avparser_ops avparser_ops_mp3;
    return avparser_ops_register(&avparser_ops_mp3);
}

/**
 * @brief  regist avparser for adts
 * @return 0/-1
 */
int avparser_register_adts()
{
    extern struct avparser_ops avparser_ops_adts;
    return avparser_ops_register(&avparser_ops_adts);
}

