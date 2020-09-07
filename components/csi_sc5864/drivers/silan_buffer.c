/*
 * buffer.c
 */

#define MODULE_NAME           "BUFFER"
//#define __DEBUG__

#ifndef NULL
#define NULL   0
#endif

#include "silan_buffer.h"
#include "silan_printf.h"

inline int silan_buf_get_fullness(buf_t *buf)
{
    int ret = 0;
    char *rd, *wt;

    rd = (char*)buf->rd;
    wt = (char*)buf->wt;

    ret = ((wt - rd) < 0) ? ((wt - rd) + buf->bytes) : (wt - rd);

    return ret;
}

static inline int silan_buf_get_bytes(buf_t *buf, char *rd, char *wt)
{
    return ((wt - rd) < 0) ? ((wt - rd) + buf->bytes) : (wt - rd);
}

static inline int silan_buf_get_cont_read_bytes(buf_t *buf, char *rd, char *wt)
{
    if (rd == wt) {
        return 0;
    } else if (rd < wt) {
        return (int)(wt - rd);
    } else { /*if (rd > wt)*/
        return (int)(buf->lmt - rd);
    }
}

static inline int silan_buf_get_cont_write_bytes(buf_t *buf, char *rd, char *wt)
{
    if (rd < wt) {
        if (rd == buf->base) {
            return (int)(buf->lmt - wt - 1);
        } else {
            return (int)(buf->lmt - wt);
        }
    } else if (rd > wt) {
        return (int)(rd - wt - 1);
    } else if (rd == wt) {
        if(rd == buf->base) {
            return (int)(buf->lmt - wt - 1);
        } else {
            return (int)(buf->lmt - wt);
        }
    }
    return 0;
}

int silan_buf_pre_read(buf_t *buf, char **read_p, int offset, int bytes)
{
    int ret = 0;
    char *rd, *wt;
    int  cont_read_bytes;

    wt = (char*)buf->wt;
    rd = (char*)buf->rd;
    cont_read_bytes = silan_buf_get_cont_read_bytes(buf, rd, wt);

    if (!cont_read_bytes) {
        *read_p = NULL;
        ret = 0;
    } else if ((offset+bytes) > silan_buf_get_bytes(buf, rd, wt)) {
        *read_p = NULL;
        ret = 0;
    } else if ((offset+bytes) > cont_read_bytes) {
        if (offset >= cont_read_bytes) {
            offset -= cont_read_bytes;
            rd = (char*)(buf->base + offset);
            cont_read_bytes = silan_buf_get_cont_read_bytes(buf, rd, wt);
            *read_p = rd;
            ret = (bytes <= cont_read_bytes) ? bytes : cont_read_bytes;
        } else {
            *read_p = rd + offset;
            ret = cont_read_bytes - offset;
        }
    } else {
        *read_p = rd + offset;
        ret = bytes;
    }

    return ret;
}

int silan_buf_read(buf_t *buf, int bytes)
{
    int ret = 0;
    char *rd, *wt;
    int  cont_read_bytes;

    rd = (char*)buf->rd;
    wt = (char*)buf->wt;
    cont_read_bytes = silan_buf_get_cont_read_bytes(buf, rd, wt);

    if (bytes <= cont_read_bytes) {
        buf->rd = ((rd+bytes) >= buf->lmt) ? buf->base : (rd+bytes);
        ret = bytes;
    } else {
        ret = 0;
    }

    return ret;
}

int silan_buf_pre_write(buf_t *buf, char **write_p, int offset, int bytes)
{
    int ret = 0;
    char *rd, *wt;
    int  cont_write_bytes, remains;

    wt = (char*)buf->wt;
    rd = (char*)buf->rd;

    cont_write_bytes = silan_buf_get_cont_write_bytes(buf, rd, wt);
    remains = buf->bytes - silan_buf_get_bytes(buf, rd, wt);

    if (!cont_write_bytes) {
        *write_p = NULL;
        ret = 0;
    } else if ((offset+bytes) >= remains) {
        *write_p = NULL;
        ret = 0;
    } else if ((offset+bytes) > cont_write_bytes) {
        if (offset >= cont_write_bytes) {
            offset -= cont_write_bytes;
            wt = (char*)(buf->base + offset);
            cont_write_bytes = silan_buf_get_cont_read_bytes(buf, rd, wt);
            *write_p = wt;
            ret = (bytes <= cont_write_bytes) ? bytes : cont_write_bytes;
        } else {
            *write_p = wt + offset;
            ret = cont_write_bytes - offset;
        }
    } else {
        *write_p = wt + offset;
        ret = bytes;
    }

    return ret;
}

int silan_buf_write(buf_t *buf, int bytes)
{
    int ret = 0;
    char *rd, *wt;
    int cont_write_bytes;

    wt = (char*)buf->wt;
    rd = (char*)buf->rd;
    cont_write_bytes = silan_buf_get_cont_write_bytes(buf, rd, wt);

    if (bytes <= cont_write_bytes) {
        buf->wt = ((wt+bytes) >= buf->lmt) ? buf->base : (wt+bytes);
        ret = bytes;
    } else {
        ret = 0;
    }

    return ret;
}

int silan_buf_init(buf_t *buf, char *base, int bytes)
{
    if (((int)base & 0x3) || (bytes <= 1)) {
        return -1;
    }

    buf->base  = buf->rd = buf->wt = base;
    buf->lmt   = base + bytes;
    buf->bytes = bytes;
    return 0;
}

