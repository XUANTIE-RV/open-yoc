/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __PCM_BUFFER_H__
#define __PCM_BUFFER_H__

#include <aos/kernel.h>
#include <stdbool.h>

#define PCM_BUFFER_MAX_NUM 50
#define PCM_BUFFER_SIZE (10 * 1024)

/**
    Internal PCM Buffer
*/
struct pcm_buffer {
    char *buffer;
    int   used;
    int   total;
    bool  valid; /* means buffer is malloced */
};

struct pcm_buffer_list {
    struct pcm_buffer buffer[PCM_BUFFER_MAX_NUM];
    aos_mutex_t mutex;
};

void pcm_buffer_move(struct pcm_buffer_list *list_dst, struct pcm_buffer_list *list_from);
void pcm_buffer_init(struct pcm_buffer_list *list);
void pcm_buffer_alloc(struct pcm_buffer *buffer);
void pcm_buffer_free(struct pcm_buffer_list *list);
int pcm_buffer_copy(struct pcm_buffer_list *list, char *data, int size);
int pcm_buffer_total_size(struct pcm_buffer_list *list);

#endif
