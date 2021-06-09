/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "mbox_test.h"


int test_mbox_interface(char *args)
{
    csi_error_t ret;

    ret = csi_mbox_init(NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_mbox_init interface test error, should return CSI_ERROR,but returned %d", ret);

    ret  = csi_mbox_send(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_mbox_send interface test error, should return CSI_ERROR,but returned %d", ret);

    ret = csi_mbox_receive(NULL, 0, NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_mbox_receive interface test error, should return CSI_ERROR,but returned %d", ret);

    ret = csi_mbox_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_mbox_attach_callback interface test error, should return CSI_ERROR,but returned %d", ret);

    csi_mbox_detach_callback(NULL);

    csi_mbox_uninit(NULL);

    return 0;
}
