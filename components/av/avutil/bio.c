/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/bio.h"

int bio_init(bio_t *bio, uint8_t *buf, size_t size)
{
    CHECK_PARAM(bio && buf && size, -1);
    bio->pos  = 0;
    bio->buf  = buf;
    bio->size  = size;

    return 0;
}

int bio_reset(bio_t *bio)
{
    CHECK_PARAM(bio, -1);
    bio->pos = 0;

    return 0;
}

int bio_read(bio_t *bio, uint8_t *buf, size_t count)
{
    size_t size = 0, remain;

    CHECK_PARAM(bio && buf && count, -1);
    remain = bio->size - bio->pos;
    size = remain > count ? count : remain;
    if (size) {
        memcpy(buf, bio->buf + bio->pos, size);
        bio->pos += size;
    }

    return size;
}

int bio_skip(bio_t *bio, int32_t offset)
{
    int rc = -1;

    CHECK_PARAM(bio && offset > 0, -1);
    if (offset + bio->pos <= bio->size) {
        bio->pos += offset;
        rc = 0;
    }

    return rc;
}

int bio_tell(bio_t *bio)
{
    CHECK_PARAM(bio, -1);
    return bio->pos;
}

uint8_t bio_r8(bio_t *bio)
{
    uint8_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (1 + bio->pos <= bio->size) {
        rc = bio->buf[bio->pos];
        bio->pos += 1;
    }

    return rc;
}

uint16_t bio_r16be(bio_t *bio)
{
    uint16_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (2 + bio->pos <= bio->size) {
        rc = byte_r16be(bio->buf + bio->pos);
        bio->pos += 2;
    }

    return rc;
}

uint32_t bio_r24be(bio_t *bio)
{
    uint32_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (3 + bio->pos <= bio->size) {
        rc = byte_r24be(bio->buf + bio->pos);
        bio->pos += 3;
    }

    return rc;
}

uint32_t bio_r32be(bio_t *bio)
{
    uint32_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (4 + bio->pos <= bio->size) {
        rc = byte_r32be(bio->buf + bio->pos);
        bio->pos += 4;
    }

    return rc;
}

uint64_t bio_r64be(bio_t *bio)
{
    uint64_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (8 + bio->pos <= bio->size) {
        rc = byte_r64be(bio->buf + bio->pos);
        bio->pos += 8;
    }

    return rc;
}

uint16_t bio_r16le(bio_t *bio)
{
    uint16_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (2 + bio->pos <= bio->size) {
        rc = byte_r16le(bio->buf + bio->pos);
        bio->pos += 2;
    }

    return rc;
}

uint32_t bio_r24le(bio_t *bio)
{
    uint32_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (3 + bio->pos <= bio->size) {
        rc = byte_r24le(bio->buf + bio->pos);
        bio->pos += 3;
    }

    return rc;
}

uint32_t bio_r32le(bio_t *bio)
{
    uint32_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (4 + bio->pos <= bio->size) {
        rc = byte_r32le(bio->buf + bio->pos);
        bio->pos += 4;
    }

    return rc;
}

uint64_t bio_r64le(bio_t *bio)
{
    uint64_t rc = -1;

    CHECK_PARAM(bio, -1);
    if (8 + bio->pos <= bio->size) {
        rc = byte_r64le(bio->buf + bio->pos);
        bio->pos += 8;
    }

    return rc;
}






