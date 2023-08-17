/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_PARSER_H
#define TM_PARSER_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>

using namespace std;

class TMParser
{
public:
    TMParser();
    virtual ~TMParser();

    virtual int Open(TMPropertyList *propList = NULL) = 0;
    virtual int Close() = 0;

    virtual int Parse(TMPacket &packet, const uint8_t *buf, size_t buf_size) = 0;
    virtual int Parse(TMPacket &packet, uint32_t &width, uint32_t &height, const uint8_t *buf, size_t buf_size) = 0;

    enum class PropID : int
    {
        RETAIN_FRAME_SIZE, // Type: UINT32, default=1920*1080, Usually the size of the I frame.
    };
};

#endif  /* TM_PARSER_H */
