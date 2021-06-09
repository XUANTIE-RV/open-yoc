/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

extern int main(void);

__attribute__((weak)) int pre_main(void)
{
    return main();
}
