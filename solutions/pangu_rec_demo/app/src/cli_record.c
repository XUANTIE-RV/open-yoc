/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"


static void cmd_rec_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc >= 2) {
        if (strcmp(argv[0], "rec") == 0) {
			if (strcmp(argv[1], "start") == 0) {
				extern void app_ws_rec_start(const char *url, const char *save_name);
				app_ws_rec_start(argv[2], argv[3]);
			} else if(strcmp(argv[1], "stop") == 0) {
				extern void app_ws_rec_stop(void);
				app_ws_rec_stop();
			}
        }
        return;
    }
}

void cli_reg_cmd_rec(void)
{
    static const struct cli_command cmd_info = {
        "rec",
        "rec cmd ...",
        cmd_rec_func,
    };

    aos_cli_register_command(&cmd_info);
}
