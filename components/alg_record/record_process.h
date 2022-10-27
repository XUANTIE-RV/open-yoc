/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __RECORD_PROCESS_H__
#define __RECORD_PROCESS_H__

#ifdef __cplusplus
extern "C"
{
#endif

void record_msg_publish(int start);
void record_set_grpcnt(int grpcnt);
void record_set_chncnt(int chncnt);
int record_get_grpcnt(void);
int record_get_chncnt(void);
void rec_copy_data(int index, uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif