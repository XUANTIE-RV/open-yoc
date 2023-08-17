#include <board.h>

#include <string.h>
#include <aos/cli.h>
#include <yoc/adb_server.h>
#include <yoc/adb.h>
#include <yoc/pcm_input_port.h>

static int g_adb_inited = 0;

static const adbserver_cmd_t adb_cmd[] = {
    AT_PUSHINFO,   AT_PUSHSTART, AT_PUSHFINISH, AT_PUSHPOST,    AT_PULLINFO,   AT_PULLSTART,
    AT_PULLFINISH, AT_PULLREQ,   AT_DELETE,     AT_ADBOK,       AT_DEVICES,    AT_ADBCLI,
    AT_ADBSTART,   AT_ADBSTOP,   AT_ADBEXIT,    AT_RECORDSTART, AT_RECORDSTOP, AT_NULL,
};

static void adb_event_cb(adb_event_t event)
{
    switch(event) {
        case ADB_PUSH_START:
        case ADB_PULL_START:
            pcm_acquire_set_enable(0);
            break;
        case ADB_PUSH_FINISH:
        case ADB_PULL_FINISH:
            pcm_acquire_set_enable(1);
            break;
        default:;
    }
}

static void adb_init(void)
{
    utask_t *task = utask_new("adb_srv", 8192, QUEUE_MSG_COUNT, 50);
    adbserver_init(task, CONSOLE_UART_IDX, NULL);
    adbserver_enabled(1);
    adbserver_set_output_terminator("");
    adbserver_add_command(adb_cmd);
    adb_start();
    adb_event_register(adb_event_cb);
}

static void cmd_adb_start(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (g_adb_inited == 0) {
        adb_init(); /* 第一次初始化，创建服务 */
        g_adb_inited = 1;
    } else {
        adbserver_enabled(1);
    }
}

static void cmd_adb_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    extern void adb_info_print(int clear);
    extern void adb_flac_info_print(int clear);

    if (strcmp(argv[1], "info") == 0) {
        adb_info_print(0);
        // adb_flac_info_print(0);
    } else if (strcmp(argv[1], "clear") == 0) {
        adb_info_print(1);
        // adb_flac_info_print(1);
    }
}

void cli_reg_cmd_adb_config(void)
{
    static const struct cli_command cli_info_adb_start = { "AT+ADBSTART", "adb start", cmd_adb_start };
    aos_cli_register_command(&cli_info_adb_start);

    static const struct cli_command cli_info_adb_info = { "adb", "adb info", cmd_adb_func };
    aos_cli_register_command(&cli_info_adb_info);
}
