/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <app_init.h>
#include <string.h>

extern int application_start(int argc, char **argv);

int main()
{
    board_yoc_init();

    application_start(0, NULL);

    return 0;
}

