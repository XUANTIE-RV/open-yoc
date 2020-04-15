/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "string.h"
#include "sd.h"
#include "aos/hal/sd.h"

typedef struct {
    sd_card_t *card;
    void *user_data;
} hal_sd_handle_t;

int32_t hal_sd_init(sd_dev_t *sd)
{
    if (sd == NULL) {
         return -1;
    }

    sd->priv = malloc(sizeof(hal_sd_handle_t));
    if (sd->priv == NULL) {
        return -1;
    }

    hal_sd_handle_t *handle = (hal_sd_handle_t *)sd->priv;
    handle->card = malloc(sizeof(sd_card_t));
    if (handle->card == NULL) {
        return -1;
    }

    memset(handle->card, 0, sizeof(sd_card_t));

    handle->user_data = NULL;

    int ret = SD_Init(handle->card, handle->user_data, sd->port);
    if (ret != kStatus_Success) {
            return -1;
    }

    return 0;
}

int32_t hal_sd_blks_read(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                              uint32_t blks, uint32_t timeout)
{
    if (sd == NULL || sd->priv == NULL) {
        return -1;
    }

    hal_sd_handle_t *handle = (hal_sd_handle_t *)sd->priv;

    int32_t ret = SD_ReadBlocks(handle->card, data, blk_addr, blks);
    if (ret != kStatus_Success) {
        return -1;
    }

    return 0;
}

int32_t hal_sd_blks_write(sd_dev_t *sd, uint8_t *data, uint32_t blk_addr,
                       uint32_t blks, uint32_t timeout)
{
    if (sd == NULL || sd->priv == NULL) {
        return -1;
    }

    hal_sd_handle_t *handle = (hal_sd_handle_t *)sd->priv;

    int32_t ret = SD_WriteBlocks(handle->card, data, blk_addr, blks);
    if (ret != kStatus_Success) {
        return -1;
    }

    return 0;
}

int32_t hal_sd_erase(sd_dev_t *sd, uint32_t blk_start_addr, uint32_t blk_end_addr)
{
    if (sd == NULL || sd->priv == NULL) {
        return -1;
    }

    hal_sd_handle_t *handle = (hal_sd_handle_t *)sd->priv;
    int32_t ret = SD_EraseBlocks(handle->card, blk_start_addr, blk_end_addr);

    if (ret != kStatus_Success) {
        return -1;
    }

    return 0;
}

int32_t hal_sd_stat_get(sd_dev_t *sd, hal_sd_stat *stat)
{
    *stat = SD_STAT_READY;
    return 0;
}

int32_t hal_sd_info_get(sd_dev_t *sd, hal_sd_info_t *info)
{
    if (sd == NULL || sd->priv == NULL) {
        return -1;
    }

    hal_sd_handle_t *handle = (hal_sd_handle_t *)sd->priv;
    info->blk_nums = handle->card->block_count;
    info->blk_size = handle->card->block_size;

    return 0;
}

int32_t hal_sd_finalize(sd_dev_t *sd)
{
    if (sd == NULL || sd->priv == NULL) {
        return -1;
    }

    hal_sd_handle_t *handle = (hal_sd_handle_t *)sd->priv;
    SD_Deinit(handle->card);

    if (handle->card != NULL) {
        free(handle->card);
    }

    if (handle->user_data != NULL) {
        free(handle->user_data);
    }

    free(handle);

    return 0;
}
