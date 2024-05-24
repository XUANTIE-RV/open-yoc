/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/avutil/byte_rw.h"

uint16_t byte_r16be(const uint8_t *buf)
{
    if (buf) {
        return (buf[0] << 8) | buf[1];
    }
    return -1;
}

uint32_t byte_r24be(const uint8_t *buf)
{
    uint32_t x = -1;

    if (buf) {
        x = buf[0];
        x = (x << 8) | buf[1];
        x = (x << 8) | buf[2];
    }

    return x;
}

uint32_t byte_r32be(const uint8_t *buf)
{
    uint32_t x = -1;

    if (buf) {
        x = buf[0];
        x = (x << 8) | buf[1];
        x = (x << 8) | buf[2];
        x = (x << 8) | buf[3];
    }

    return x;
}

uint64_t byte_r64be(const uint8_t *buf)
{
    uint64_t x = -1;

    if (buf) {
        x = buf[0];
        x = (x << 8) | buf[1];
        x = (x << 8) | buf[2];
        x = (x << 8) | buf[3];
        x = (x << 8) | buf[4];
        x = (x << 8) | buf[5];
        x = (x << 8) | buf[6];
        x = (x << 8) | buf[7];
    }

    return x;
}

uint16_t byte_r16le(const uint8_t *buf)
{
    if (buf) {
        return buf[0] | (buf[1] << 8);
    }
    return -1;
}

uint32_t byte_r24le(const uint8_t *buf)
{
    uint32_t x = -1;

    if (buf) {
        x = buf[0];
        x = x | (buf[1] << 8);
        x = x | (buf[2] << 16);
    }

    return x;
}

uint32_t byte_r32le(const uint8_t *buf)
{
    uint32_t x = -1;

    if (buf) {
        x  = (uint32_t)byte_r16le(buf);
        x |= (uint32_t)byte_r16le(buf + 2) << 16;
    }

    return x;
}

uint64_t byte_r64le(const uint8_t *buf)
{
    uint64_t x = -1;

    if (buf) {
        x  = (uint64_t)byte_r32le(buf);
        x |= (uint64_t)byte_r32le(buf + 4) << 32;
    }

    return x;
}


