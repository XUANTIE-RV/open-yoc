/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

static int g_wwv_confirm = 0;

int app_wwv_get_confirm()
{
    return g_wwv_confirm;
}

void app_wwv_set_confirm(int confirm)
{
    g_wwv_confirm = confirm;
}
