/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "stream/stream.h"
#include "avformat/avformat_utils.h"

/**
 * @brief  xxx_sync read bytes from stream
 * @param  [in] opaque
 * @param  [in] bytes
 * @param  [in] size
 * @return -1 on error
 */
int sync_read_stream(void *opaque, uint8_t *bytes, size_t size)
{
    int rc;
    stream_cls_t *s = opaque;

    rc = stream_read(s, bytes, size);

    return rc;
}

/**
 * @brief  xxx_sync read bytes from bio_t, may be used for avformat probe
 * @param  [in] opaque
 * @param  [in] bytes
 * @param  [in] size
 * @return -1 on error
 */
int sync_read_bio(void *opaque, uint8_t *bytes, size_t size)
{
    bio_t *bio = opaque;
    return bio_read(bio, bytes, size);
}

/**
 * @brief  get size of the id2v2
 * @param  [in] buf
 * @param  [in] buf_size
 * @return 0 on no id3 info
 */
size_t get_id3v2_size(const uint8_t *buf, size_t buf_size)
{
    CHECK_PARAM(buf && (buf_size >= 10), 0);
    if (!strncmp((char *)buf, "ID3", 3)) {
        return (((buf[6] & 0x7f) << 21) | ((buf[7] & 0x7f) << 14) |
                ((buf[8] & 0x7f) << 7) | (buf[9] & 0x7f)) + 10;
    }

    return 0;
}

/**
 * @brief  new a track info
 * @param  [in] type : a/v
 * @return NULL on error
 */
track_info_t* track_info_new(avmedia_type_t type)
{
    track_info_t *track;

    CHECK_PARAM(type == AVMEDIA_TYPE_AUDIO || type == AVMEDIA_TYPE_VIDEO, NULL);
    track = aos_zalloc(sizeof(track_info_t));
    if (track) {
        track->type = type;
    }

    return track;
}

/**
 * @brief  free the track info
 * @param  [in] track
 * @return 0/-1
 */
int track_info_free(track_info_t *track)
{
    CHECK_PARAM(track, -1);
    if (track->type == AVMEDIA_TYPE_AUDIO) {
        aos_freep(&track->t.a.artist);
        aos_freep(&track->t.a.album);
        aos_freep(&track->t.a.title);
    }
    aos_freep(&track->codec_name);
    aos_free(track);

    return 0;
}

/**
 * @brief  free the track list info
 * @param  [in] ptracks
 * @return 0/-1
 */
int tracks_info_freep(track_info_t **ptracks)
{
    track_info_t *tracks, *ntrack;

    CHECK_PARAM(ptracks, -1);
    tracks = *ptracks;
    while (tracks) {
        ntrack = tracks->next;
        track_info_free(tracks);
        tracks = ntrack;
    }
    *ptracks = NULL;

    return 0;
}

/**
 * @brief  add track to the track list info
 * @param  [in] ptracks
 * @param  [in] track
 * @return 0/-1
 */
int tracks_info_add(track_info_t **ptracks, const track_info_t *track)
{
    track_info_t *tracks;

    CHECK_PARAM(ptracks && track, -1);
    tracks = *ptracks;
    if (tracks) {
        while ((*ptracks)->next) *ptracks = (*ptracks)->next;
        (*ptracks)->next = (track_info_t*)track;
        *ptracks         = tracks;
    } else {
        *ptracks = (track_info_t*)track;
    }

    return 0;
}

