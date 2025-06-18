 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <aos/debug.h>
#include <aos/cli.h>

extern void cli_reg_cmd_ps(void);

void cmd_qemu_quit(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc > 0) {
        if (!strcmp(argv[0], "q") || !strcmp(argv[0], "quit")) {
            aos_cli_printf("quit from qemu.\n");
            *(unsigned long *)0x10002000 = 1;
            return;
        }
    }
    aos_cli_printf("Usage: quit\n");
}

void cli_reg_cmd_quit(void)
{
    static const struct cli_command cmd_list[] = {
        {"quit", "quit from qemu", cmd_qemu_quit},
        {"q", "quit from qemu", cmd_qemu_quit},
    };

    aos_cli_register_commands(cmd_list, sizeof(cmd_list)/sizeof(struct cli_command));
}

void board_cli_init()
{
    aos_cli_init();
    cli_reg_cmd_ps();
#if CONFIG_QEMU_RUN
    cli_reg_cmd_quit();
#endif
}
