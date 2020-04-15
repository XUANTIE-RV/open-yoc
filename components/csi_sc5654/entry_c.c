/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

extern void main();

__attribute__((weak)) void entry_c()
{
    main();
}