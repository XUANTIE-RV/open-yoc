/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <board.h>

#if defined(BOARD_AUDIO_SUPPORT) && BOARD_AUDIO_SUPPORT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/cli.h>

#define MAX_EQ_DATA_BYTE 512

#define TAG "EQSET"

extern int strsplit(char **array, size_t count, char *data, const char *delim);

static int *g_eq_array  = NULL;
static int  g_array_pos = -1;
static void cmd_eqset_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc == 3) {
        if (strcmp(argv[1], "push") == 0) {
            if (strcmp(argv[2], "start") == 0) {
                if (g_eq_array == NULL) {
                    g_eq_array = (int *)malloc(MAX_EQ_DATA_BYTE);
                }
                g_array_pos = 0;
                printf("push start ok\r\n");
                return;
            } else if (strcmp(argv[2], "end") == 0) {
                if (g_eq_array) {
                    // for (int i = 0; i < g_array_pos; i++) {
                    //     printf("0x%08x\r\n", g_eq_array[i]);
                    // }
                    board_eq_set_param((void*)g_eq_array, g_array_pos * sizeof(int));
                    free(g_eq_array);

                    printf("push end %d\r\n", g_array_pos);
                    g_eq_array  = NULL;
                    g_array_pos = -1;
                    return;
                }
            } else {
                //printf("%s\r\n", argv[2]);
                if (g_array_pos >= 0) {
                    char *endptr;
                    char *str_array[20];
                    int   count = strsplit(str_array, 20, argv[2], ",");
                    for (int i = 0; i < count; i++) {
                        //printf("%d, %s\r\n", i, str_array[i]);
                        g_eq_array[g_array_pos] = strtol(&str_array[i][2], &endptr, 16);
                        g_array_pos++;
                    }
                    printf("push data %d\r\n", count);
                    return;
                }
            }
        } /* push */
    } /*argc == 3*/
 
    if (argc == 2) {
        if (strcmp(argv[1], "show") == 0) {
            board_eq_show_param();
            return;
        }
    }
    printf("usage:\r\n");
    printf("\teq push start\r\n");
    printf("\teq push \"0x1a96b,0xcad2a,0x1a96b,0x352d6,0xe5695\"\r\n");
    printf("\teq push end\r\n");
}

//extern int board_drc_set_param(void *data, int byte);
static void cmd_drcset_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    int  eq_array[2];
    int  array_pos = 0;

    if (argc == 3) {
        if (strcmp(argv[1], "set") == 0) {
            // drc buf update
            printf("%s\r\n", argv[2]);
            char *endptr;
            char *str_array[8];
            int   count = strsplit(str_array, 8, argv[2], ",");
            for (int i = 0; i < count; i++) {
                //printf("%d, %s\r\n", i, str_array[i]);
                eq_array[array_pos] = strtoul(&str_array[i][2], &endptr, 16);
                array_pos++;
            }
            // drc set
            printf("---> drc set \"0x%08x,0x%08x\"\r\n", eq_array[0], eq_array[1]);
            board_drc_set_param((void*)eq_array, sizeof(eq_array));
        }
    } else {
        printf("usage:\r\n");
        printf("\tdrc set \"0xFF428800,0x44245011\"\r\n");
    }
}

void cli_reg_cmd_eqset(void)
{
    static const struct cli_command cmd_list[] = {
        {"eq", "set eq", cmd_eqset_func},
        {"drc", "set drc", cmd_drcset_func},
    };

    aos_cli_register_commands(cmd_list, sizeof(cmd_list)/sizeof(struct cli_command));
}
#endif
