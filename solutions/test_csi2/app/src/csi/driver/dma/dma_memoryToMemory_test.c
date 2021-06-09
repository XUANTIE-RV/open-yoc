/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <dma_test.h>

volatile static uint8_t lock = 0;

static void dma_ch_callback(csi_dma_ch_t *dma_ch, csi_dma_event_t event, void *arg)
{
    if (event == DMA_EVENT_TRANSFER_DONE) {
        lock = 0;
    }
}

static int data_init(void *addr_src, void *addr_dest, uint8_t size, uint32_t data_length)
{
    uint32_t i;

    if (size == 0) {
        for (i = 0; i < data_length; i++) {
            *(uint8_t *)((uint32_t)addr_src + i) = i;
            *(uint8_t *)((uint32_t)addr_dest + i) = 0;
        }
    } else if (size == 1) {
        for (i = 0; i < data_length / 2; i++) {
            *(uint16_t *)((uint32_t)addr_src + 2 * i) = i;
            *(uint16_t *)((uint32_t)addr_dest + 2 * i) = 0;
        }
    } else if (size == 2) {
        for (i = 0; i < data_length / 4; i++) {
            *(uint32_t *)((uint32_t)addr_src + 4 * i) = i;
            *(uint32_t *)((uint32_t)addr_dest + 4 * i) = 0;
        }
    } else {
        return -1;
    }

    return 0;
}

int transfer_check(void *src, void *dest, test_dma_args_t *args)
{
    uint32_t i = 0;

    if ((args->src_addr_inc == args->dest_addr_inc) && (args->dest_addr_inc != DMA_ADDR_CONSTANT)) {
        return memcmp(src, dest, args->transfer_length);
    } else if ((args->src_addr_inc == args->dest_addr_inc) && (args->dest_addr_inc == DMA_ADDR_CONSTANT)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            if ((*(uint8_t *)src != *(uint8_t *)dest) || (*(uint8_t *)((uint32_t)dest + args->transfer_length - 1) == *(uint8_t *)((uint32_t)src + args->transfer_length - 1))) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            if ((*(uint16_t *)src != *(uint16_t *)dest) || (*(uint16_t *)((uint32_t)dest + args->transfer_length - 2) == *(uint16_t *)((uint32_t)src + args->transfer_length - 2))) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            if ((*(uint32_t *)src != *(uint32_t *)dest) || (*(uint32_t *)((uint32_t)dest + args->transfer_length - 4) == *(uint32_t *)((uint32_t)src + args->transfer_length - 4))) {
                return -1;
            }
        }
    }

    if ((args->src_addr_inc == DMA_ADDR_INC) && (args->dest_addr_inc == DMA_ADDR_DEC)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            for (i = 0; i < args->transfer_length; i++) {
                if ((*(uint8_t *)((uint32_t)src + i)) != (*(uint8_t *)((uint32_t)dest + args->transfer_length - (i + 1)))) {
                    return -1;
                }
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            for (i = 0; i < args->transfer_length / 2; i++) {
                if ((*(uint16_t *)((uint32_t)src + 2 * i)) != (*(uint16_t *)((uint32_t)dest + args->transfer_length - 2 * (i + 1)))) {
                    return -1;
                }
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            for (i = 0; i < args->transfer_length / 4; i++) {
                if ((*(uint32_t *)((uint32_t)src + 4 * i)) != (*(uint32_t *)((uint32_t)dest + args->transfer_length - 4 * (i + 1)))) {
                    return -1;
                }
            }
        }
    }

    if ((args->src_addr_inc == DMA_ADDR_DEC) && (args->dest_addr_inc == DMA_ADDR_INC)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            for (i = 0; i < args->transfer_length; i++) {
                if ((*(uint8_t *)((uint32_t)dest + i)) != (*(uint8_t *)((uint32_t)src + args->transfer_length - (i + 1)))) {
                    return -1;
                }
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            for (i = 0; i < args->transfer_length / 2; i++) {
                if ((*(uint16_t *)((uint32_t)dest + 2 * i)) != (*(uint16_t *)((uint32_t)src + args->transfer_length - 2 * (i + 1)))) {
                    return -1;
                }
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            for (i = 0; i < args->transfer_length / 4; i++) {
                if ((*(uint32_t *)((uint32_t)dest + 4 * i)) != (*(uint32_t *)((uint32_t)src + args->transfer_length - 4 * (i + 1)))) {
                    return -1;
                }
            }
        }
    }

    if ((args->src_addr_inc == DMA_ADDR_INC) && (args->dest_addr_inc == DMA_ADDR_CONSTANT)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            if (*(uint8_t *)((uint32_t)src + args->transfer_length - 1) != *(uint8_t *)dest) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            if (*(uint16_t *)((uint32_t)src + args->transfer_length - 2) != *(uint16_t *)dest) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            if (*(uint32_t *)((uint32_t)src + args->transfer_length - 4) != *(uint32_t *)dest) {
                return -1;
            }
        }
    }

    if ((args->src_addr_inc == DMA_ADDR_DEC) && (args->dest_addr_inc == DMA_ADDR_CONSTANT)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            if (*(uint8_t *)src != *(uint8_t *)dest) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            if (*(uint16_t *)src != *(uint16_t *)dest) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            if (*(uint32_t *)src != *(uint32_t *)dest) {
                return -1;
            }
        }
    }

    if ((args->src_addr_inc == DMA_ADDR_CONSTANT) && (args->dest_addr_inc == DMA_ADDR_INC)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            if (*(uint8_t *)src != *(uint8_t *)((uint32_t)dest + args->transfer_length - 1)) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            if (*(uint16_t *)src != *(uint16_t *)((uint32_t)dest + args->transfer_length - 2)) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            if (*(uint32_t *)src != *(uint32_t *)((uint32_t)dest + args->transfer_length - 4)) {
                return -1;
            }
        }
    }

    if ((args->src_addr_inc == DMA_ADDR_CONSTANT) && (args->dest_addr_inc == DMA_ADDR_DEC)) {
        if (args->data_width == DMA_DATA_WIDTH_8_BITS) {
            if (*(uint8_t *)src != *(uint8_t *)dest) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_16_BITS) {
            if (*(uint16_t *)src != *(uint16_t *)dest) {
                return -1;
            }
        }

        if (args->data_width == DMA_DATA_WIDTH_32_BITS) {
            if (*(uint32_t *)src != *(uint32_t *)dest) {
                return -1;
            }
        }
    }

    return 0;
}

int test_dma_MemoryToMemory(char *args)
{
    csi_dma_ch_t ch_hd;
    csi_error_t ret_sta;
    void *src = NULL, *dest = NULL;
    int ret;
    test_dma_args_t td;

    uint32_t get_data[7];

    ret = args_parsing(args, get_data, 7);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dma_idx = (uint8_t)get_data[0];
    td.channel_idx = (uint8_t)get_data[1];
    td.data_width = (uint8_t)get_data[2];
    td.src_addr_inc = (uint8_t)get_data[3];
    td.dest_addr_inc = (uint8_t)get_data[4];
    td.group_length = (uint8_t)get_data[5];
    td.transfer_length = (uint32_t)get_data[6];

    TEST_CASE_TIPS("test DMA idx is %d", td.dma_idx);
    TEST_CASE_TIPS("test channel is %d", td.channel_idx);
    TEST_CASE_TIPS("config data width is %d", td.data_width);
    TEST_CASE_TIPS("config source address increment is %d", td.src_addr_inc);
    TEST_CASE_TIPS("config destination address increment is %d", td.dest_addr_inc);
    TEST_CASE_TIPS("config group length is %d", td.group_length);
    TEST_CASE_TIPS("config transfer data length is %d bytes", td.transfer_length);

    src = (void *)malloc(td.transfer_length);
    dest = (void *)malloc(td.transfer_length);

    if ((src == NULL) || (dest == NULL)) {
        free(src);
        free(dest);
        TEST_CASE_WARN_QUIT("malloc error");
    }

    ret = data_init(src, dest, td.data_width, td.transfer_length);

    if (ret != 0) {
        free(src);
        free(dest);
        TEST_CASE_WARN_QUIT("data init error");
    }

    ret_sta = csi_dma_ch_alloc(&ch_hd, td.channel_idx, td.dma_idx);

    if (ret_sta != 0) {
        free(src);
        free(dest);
        TEST_CASE_ASSERT_QUIT(1 == 0, "DMA %d alloc channel %d error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);
    }

    csi_dma_ch_config_t config;
    config.src_inc = td.src_addr_inc;
    config.dst_inc = td.dest_addr_inc;
    config.src_tw = td.data_width;
    config.dst_tw = td.data_width;
    config.trans_dir = DMA_MEM2MEM;
    config.group_len = td.group_length;

    ret_sta = csi_dma_ch_config(&ch_hd, &config);

    if (ret_sta != 0) {
        csi_dma_ch_free(&ch_hd);
        free(src);
        free(dest);
        TEST_CASE_ASSERT_QUIT(1 == 0, "DMA %d channel %d config error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);
    }

    ret_sta = csi_dma_ch_attach_callback(&ch_hd, dma_ch_callback, NULL);

    if (ret_sta != 0) {
        csi_dma_ch_free(&ch_hd);
        free(src);
        free(dest);
        TEST_CASE_ASSERT_QUIT(1 == 0, "DMA %d channel %d attach callback error,should return CSI_OK,but returned %d.", td.dma_idx, td.channel_idx, ret_sta);
    }

    soc_dcache_clean_invalid_range((unsigned long)src, td.transfer_length);
    soc_dcache_clean_invalid_range((unsigned long)dest, td.transfer_length);

    lock = 1;

    if ((td.src_addr_inc == DMA_ADDR_INC) && (td.dest_addr_inc == DMA_ADDR_INC)) {
        csi_dma_ch_start(&ch_hd, src, dest, td.transfer_length);
    }

    if ((td.src_addr_inc == DMA_ADDR_DEC) && (td.dest_addr_inc == DMA_ADDR_INC)) {
        if (td.data_width == 0) {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 1), dest, td.transfer_length);
        } else if (td.data_width == 1) {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 2), dest, td.transfer_length);
        } else {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 4), dest, td.transfer_length);
        }
    }

    if ((td.src_addr_inc == DMA_ADDR_INC) && (td.dest_addr_inc == DMA_ADDR_DEC)) {
        if (td.data_width == 0) {
            csi_dma_ch_start(&ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 1), td.transfer_length);
        } else if (td.data_width == 1) {
            csi_dma_ch_start(&ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 2), td.transfer_length);
        } else {
            csi_dma_ch_start(&ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 4), td.transfer_length);
        }
    }

    if ((td.src_addr_inc == DMA_ADDR_DEC) && (td.dest_addr_inc == DMA_ADDR_DEC)) {
        if (td.data_width == 0) {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 1), (void *)((uint32_t)dest + td.transfer_length - 1), td.transfer_length);
        } else if (td.data_width == 1) {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 2), (void *)((uint32_t)dest + td.transfer_length - 2), td.transfer_length);
        } else {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 4), (void *)((uint32_t)dest + td.transfer_length - 4), td.transfer_length);
        }
    }

    if ((td.src_addr_inc == DMA_ADDR_CONSTANT) && (td.dest_addr_inc == DMA_ADDR_CONSTANT)) {
        csi_dma_ch_start(&ch_hd, src, dest, td.transfer_length);
    }

    if ((td.src_addr_inc == DMA_ADDR_INC) && (td.dest_addr_inc == DMA_ADDR_CONSTANT)) {
        csi_dma_ch_start(&ch_hd, src, dest, td.transfer_length);
    }

    if ((td.src_addr_inc == DMA_ADDR_CONSTANT) && (td.dest_addr_inc == DMA_ADDR_INC)) {
        csi_dma_ch_start(&ch_hd, src, dest, td.transfer_length);
    }

    if ((td.src_addr_inc == DMA_ADDR_DEC) && (td.dest_addr_inc == DMA_ADDR_CONSTANT)) {
        if (td.data_width == 0) {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 1), dest, td.transfer_length);
        } else if (td.data_width == 1) {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 2), dest, td.transfer_length);
        } else {
            csi_dma_ch_start(&ch_hd, (void *)((uint32_t)src + td.transfer_length - 4), dest, td.transfer_length);
        }
    }

    if ((td.src_addr_inc == DMA_ADDR_CONSTANT) && (td.dest_addr_inc == DMA_ADDR_DEC)) {
        if (td.data_width == 0) {
            csi_dma_ch_start(&ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 1), td.transfer_length);
        } else if (td.data_width == 1) {
            csi_dma_ch_start(&ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 2), td.transfer_length);
        } else {
            csi_dma_ch_start(&ch_hd, src, (void *)((uint32_t)dest + td.transfer_length - 4), td.transfer_length);
        }
    }

    while (lock)
        ;

    csi_dma_ch_stop(&ch_hd);

    csi_dma_ch_detach_callback(&ch_hd);

    csi_dma_ch_free(&ch_hd);

    ret = transfer_check(src, dest, &td);
    TEST_CASE_ASSERT(ret == 0, "data transfer error");

    free(src);
    free(dest);

    return 0;
}


