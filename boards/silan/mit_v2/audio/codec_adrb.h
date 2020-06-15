/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdint.h>

#include "silan_voice_adc.h"

/**
 * @brief  create a ring buffer
 * @param[in] length length space of the ring buffer
 * @return  pointer to ring buffer
 */
int codec_adrb_create(codec_adrb_t *codec_adrb, char *buffer, int length);

/**
 * @brief   destroy the ring buffer
 * @param[in] codec_adrb pointer to the ring buffer
 * @return  null
 */
void codec_adrb_destroy(codec_adrb_t *codec_adrb);

/**
 * @brief   read data from ring buffer.
 * @param[in] buffer pointer to the ring buffer
 * @param[in] target pointer to buffer for data to read from ring buffer
 * @param[in] amount amount of data items to read
 * @return  number of actual read data
 */
int codec_adrb_read(codec_adrb_t *codec_adrb, uint8_t *target, uint32_t amount);

/**
 * @brief   write data to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @param[in] data pointer to buffer for data to write to ring buffer
 * @param[in] length length of data items to write
 * @return  0 for success
 */
int codec_adrb_write(codec_adrb_t *codec_adrb, uint8_t *data, uint32_t length);

/**
 * @brief   is the ring buffer empty?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int codec_adrb_empty(codec_adrb_t *codec_adrb);
/**
 * @brief   is the ring buffer full?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int codec_adrb_full(codec_adrb_t *codec_adrb);
/**
 * @brief   available write space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return   number of write spcae
 */
int codec_adrb_available_write_space(codec_adrb_t *codec_adrb);
/**
 * @brief    available read space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  number of read spcae
 */
int codec_adrb_available_read_space(codec_adrb_t *codec_adrb);

#define codec_adrb_available_write_space(B) (\
        (B)->length - codec_adrb_available_read_space(B))

#define codec_adrb_full(B) (codec_adrb_available_write_space(B) == 0)

#define codec_adrb_empty(B) (codec_adrb_available_read_space((B)) == 0)

#define codec_adrb_clear(B) ((B)->tail = (B)->head =(B)->data_len = 0)
