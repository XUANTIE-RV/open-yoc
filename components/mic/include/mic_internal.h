/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __MIC_INTERNAL_H__
#define __MIC_INTERNAL_H__


#define MIC_REC_COUNT 3

void mic_rec_start(const char *url, const char *save_name);

void mic_rec_copy_data(int index, uint8_t *data, uint32_t size);

void mic_rec_stop(void);

#endif