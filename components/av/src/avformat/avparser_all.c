/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/avformat/avparser.h"
#include "av/avformat/avparser_all.h"

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

