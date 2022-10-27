/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#include <time.h>
#include <aos/cli.h>
#include <clock_alarm.h>
#include <rtc_alarm.h>

#define TAG "clock_alarm"

static void rtc_print_help(void)
{
    printf("Usage:\n\
            - clock rtc get: get rtc time\n\
            - clock rtc update: update system time to rtc\n\
            - clock rtc set <year> <month> <day> <hour> <min> <sec>: set rtc time\n\
                (100 <= year <= 227)\n");
}

static void clock_print_help(void)
{
    printf("Usage:\n\
           - clock alarm del <clock_id>: delete clock <id>\n\
           - clock alarm set <clock_id> <period> <hour> <minute> <second>: create a new alarm\n\
           - clock alarm get: get all set alarms\n");
}

static void cmd_clock_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (strcmp(argv[1], "rtc") == 0) {
        if (argc < 3) {
            rtc_print_help();
            return;
        }

        if (strcmp(argv[2], "update") == 0) {
            rtc_from_system();
        } else if (strcmp(argv[2], "get") == 0) {
            time_t     rtc_time  = rtc_get_time();
            struct tm *read_time = gmtime(&rtc_time);
            printf("rtc time [%04d-%02d-%02d, %02d:%02d:%02d]\n", read_time->tm_year + 1900,
                   read_time->tm_mon, read_time->tm_mday, read_time->tm_hour + TIME_ZONE,
                   read_time->tm_min, read_time->tm_sec);
        }
        if (strcmp(argv[2], "debug") == 0) {
            extern void rtc_debug(void);
            rtc_debug();
        } else if (strcmp(argv[2], "set") == 0) {
            if (argc >= 9) {
                struct tm tm_set;
                tm_set.tm_year = atoi(argv[3]);
                tm_set.tm_mon  = atoi(argv[4]);
                tm_set.tm_mday = atoi(argv[5]);
                tm_set.tm_hour = atoi(argv[6]);
                tm_set.tm_min  = atoi(argv[7]);
                tm_set.tm_sec  = atoi(argv[8]);
                rtc_set_time(&tm_set);
            } else {
                rtc_print_help();
            }
        }
    } else if (strcmp(argv[1], "alarm") == 0) {
        if (argc < 3) {
            clock_print_help();
        }

        int id = 0;
        if (strcmp(argv[2], "del") == 0) {
            id      = atoi(argv[3]);
            int ret = clock_alarm_set(id, NULL);
            printf("clock alarm: clock id %d is delete. ret %d\n", id, ret);
        } else if (strcmp(argv[2], "set") == 0) {
            id = atoi(argv[3]);
            clock_alarm_config_t cli_time;
            cli_time.period = atoi(argv[4]);
            cli_time.hour   = atoi(argv[5]);
            cli_time.min    = atoi(argv[6]);
            cli_time.sec    = atoi(argv[7]);

            printf("set alarm: id %d, period %d, time %02d:%02d:%02d\n", id, cli_time.period,
                   cli_time.hour, cli_time.min, cli_time.sec);
            id = clock_alarm_set(id, &cli_time);
            if (id == -2) {
                printf("clock alarm has repeat time.\n");
                return;
            } else if (id == -1) {
                printf("clock alarm has full.\n");
                return;
            }
            if (id == -4) {
                printf("clock alarm time config error.\n");
                return;
            } else {
                clock_alarm_enable(id, 1);
                printf("clock id %d set success. time %02d:%02d:%02d, period %d.\n", id,
                       cli_time.hour, cli_time.min, cli_time.sec, cli_time.period);
            }
        } else if (strcmp(argv[2], "get") == 0) {
            clock_alarm_ctx_t *clock_info;
            struct tm *        clock_tm;
            uint8_t            has_clock = 0;
            for (uint8_t idx = 1; idx < CLOCK_ALARM_NUM + 1; idx++) {
                clock_info = NULL;
                clock_info = clock_alarm_get(idx);
                if (NULL != clock_info) {
                    clock_tm = gmtime(&clock_info->time);
                    printf("clock id[%d], time[%02d:%02d:%02d], period[%d], enable[%d]\n",
                           clock_info->id, clock_tm->tm_hour + TIME_ZONE, clock_tm->tm_min,
                           clock_tm->tm_sec, clock_info->period, clock_info->enable);
                    has_clock = 1;
                }
            }
            if (0 == has_clock) {
                printf("no clock alarm set!\n");
            }
        }
    }
}

void cli_reg_cmd_clock(void)
{
    static const struct cli_command cmd_info_clock = {"clock", "clock test cmd", cmd_clock_func};
    aos_cli_register_command(&cmd_info_clock);
}