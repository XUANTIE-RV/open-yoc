/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <mbox_test.h>

test_func_map_t mailbox_test_funcs_map[] = {
    {"MBOX_INTERFACE", test_mbox_interface},
    {"MBOX_SEND", test_mbox_transferSend},
    {"MBOX_RECEIVE", test_mbox_transferReceive}
};


int test_mbox_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(mailbox_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, mailbox_test_funcs_map[i].test_func_name)) {
            (*(mailbox_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("MAILBOX module don't support this command.");
    return -1;
}