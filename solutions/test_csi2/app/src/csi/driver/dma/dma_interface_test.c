/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <dma_test.h>

int test_dma_interface(char *args)
{
    csi_error_t ret_status;

    ret_status = csi_dma_init(NULL, 0);
    TEST_CASE_ASSERT(ret_status != CSI_OK, "csi_dma_init interface test error,shouldn't return CSI_OK,but returned  %d.", ret_status);

    csi_dma_uninit(NULL);

    ret_status = csi_dma_ch_alloc(NULL, 0, 0);
    TEST_CASE_ASSERT(ret_status != CSI_OK, "csi_dma_ch_alloc interface test error,shouldn't return CSI_OK,but returned  %d.", ret_status);

    csi_dma_ch_free(NULL);

    csi_dma_ch_config_t config;
    ret_status = csi_dma_ch_config(NULL, &config);
    TEST_CASE_ASSERT(ret_status != CSI_OK, "csi_dma_ch_config interface test error,shouldn't return CSI_OK,but returned  %d.", ret_status);
    csi_dma_ch_t dma_ch;
    ret_status = csi_dma_ch_config(&dma_ch, NULL);
    TEST_CASE_ASSERT(ret_status == CSI_OK, "csi_dma_ch_config interface test error,should return CSI_OK,but returned  %d.", ret_status);

    csi_dma_ch_start(NULL, 0x0, 0x0, 100);

    csi_dma_ch_stop(NULL);

    ret_status = csi_dma_ch_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_status != CSI_OK, "csi_dma_ch_attach_callback interface test error,shouldn't return CSI_OK,but returned  %d.", ret_status);

    csi_dma_ch_detach_callback(NULL);

    return 0;
}
