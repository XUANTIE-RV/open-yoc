/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/log.h>
#include "pcm_buffer.h"

#define TAG "PCMB"

void pcm_buffer_init(struct pcm_buffer_list *list)
{
    aos_mutex_new(&list->mutex);

    int i = 0;
    for (i = 0; i < PCM_BUFFER_MAX_NUM; i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        buffer->buffer            = NULL;
        buffer->used              = 0;
        buffer->total             = 0;
        buffer->valid             = false;
    }
}


/*
    Just move the pointer, not deep move memory
**/
void pcm_buffer_append(struct pcm_buffer_list *list, struct pcm_buffer *in_buffer)
{
    int i = 0;
    
    //aos_mutex_lock(&list->mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < PCM_BUFFER_MAX_NUM; i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        if (buffer->valid == false) {
            buffer->buffer = in_buffer->buffer;
            buffer->used   = in_buffer->used;
            buffer->total  = in_buffer->total;
            buffer->valid  = true; 
            break;
        }
    }
    //aos_mutex_unlock(&list->mutex);


    if (i == PCM_BUFFER_MAX_NUM) {
          LOGE(TAG, "No buffer available2 !");
          //aos_assert(0);
          return;
     }

}


/**
    Just move the pointer, not deep move memory
    Move "ALL" buffer from source list to dest list
*/
void pcm_buffer_move(struct pcm_buffer_list *list_dst, struct pcm_buffer_list *list_from)
{
    int i = 0;

    aos_mutex_lock(&list_from->mutex, AOS_WAIT_FOREVER);
    for (i = 0; i < PCM_BUFFER_MAX_NUM; i++) {
        struct pcm_buffer *buffer = &list_from->buffer[i];
        if (buffer->valid == true) {
            pcm_buffer_append(list_dst, buffer);
            buffer->buffer = NULL;
            buffer->used   = 0;
            buffer->total  = 0;
            buffer->valid  = false;
        }
    }
    aos_mutex_unlock(&list_from->mutex);
    
}


void pcm_buffer_alloc(struct pcm_buffer *buffer)
{
    buffer->buffer = aos_zalloc(PCM_BUFFER_SIZE);
    buffer->used   = 0;
    buffer->total  = PCM_BUFFER_SIZE;
    buffer->valid  = true;

}


void pcm_buffer_free(struct pcm_buffer_list *list)
{
    aos_mutex_lock(&list->mutex,  AOS_WAIT_FOREVER);
    int i = 0;
    for (i = 0; i < PCM_BUFFER_MAX_NUM; i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        if (buffer->valid == true) {
            aos_free(buffer->buffer);
            buffer->buffer = NULL;
            buffer->used   = 0;
            buffer->total  = 0;
            buffer->valid  = false;
        }
    }
    aos_mutex_unlock(&list->mutex);
    //aos_mutex_free(&list->mutex);

}


/*
    everytime alloc 10KB, if not enough, chain another 10KB
    MAX size = PCM_BUFFER_MAX_NUM*PCM_BUFFER_SIZE
*/
int pcm_buffer_copy(struct pcm_buffer_list *list, char *data, int size)
{
    int i = 0;

    /** copy from stereo to mono,del later */
#if 0
    for (int i = 0; i < size; i +=4) {
        data[i/2 + 0] = data[i+0];
        data[i/2 + 1] = data[i+1];
    }

    size /= 2;
#endif

    if (size > PCM_BUFFER_SIZE) {
        LOGE(TAG, "PCM input buffer too large");
        return -1;
    }


    /** 1.Check free memory is not below 200KB */
    {
        int total,used,mfree,peak = 0;
        aos_get_mminfo(&total, &used, &mfree, &peak);
        if (mfree < 200*1024) {
            //printf("P");
            return -1;
        }
    }

    /** 2.Insert buffer into the pcm buffer list */


    aos_mutex_lock(&list->mutex,  AOS_WAIT_FOREVER);
    for (i = 0; i < PCM_BUFFER_MAX_NUM; i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        if ((buffer->valid == true) && (buffer->used + size <= buffer->total)) {
            memcpy(buffer->buffer + buffer->used, data, size);
            buffer->used += size;
            break;
        } else if (buffer->valid == false) {
            //LOGD(TAG, "Alloc new buffer at %d", i);
            pcm_buffer_alloc(buffer);
            memcpy(buffer->buffer + buffer->used, data, size);
            buffer->used += size;
            break;
        } else {
            continue;
        }
    }
    aos_mutex_unlock(&list->mutex);

    if (i == PCM_BUFFER_MAX_NUM) {
        //printf("F");
         //LOGE(TAG, "No buffer available!, check your (voice data size) <= (PCM_BUFFER_MAX_NUM*PCM_BUFFER_SIZE)");
         //aos_assert(0);
         return -1;
    }
    return 0;
}

int pcm_buffer_total_size(struct pcm_buffer_list *list)
{
    int i     = 0;
    int total = 0;

    aos_mutex_lock(&list->mutex,  AOS_WAIT_FOREVER);
    for (i = 0; i < sizeof(list->buffer) / sizeof(list->buffer[0]); i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        if (buffer->valid) {
            total += buffer->used;
        }
    }
    aos_mutex_unlock(&list->mutex);

    
    return total;
}

#if 0

static void pcm_buffer_dump(struct pcm_buffer_list *list)
{
    int i = 0;
//    int total = 0;
    for (i = 0; i < sizeof(list->buffer)/sizeof(list->buffer[0]); i++) {
        struct pcm_buffer *buffer = &list->buffer[i];
        if (buffer->valid) {
            LOGD(TAG, "Len Buf[%d]=%d", i, buffer->used);
        }
    }
}

#endif
