/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <aos/aos.h>
#include <yoc/adb_server.h>
#include <yoc/adb_port.h>

#define TAG "adbserver"

#ifdef LF
#undef LF
#define LF 10
#else
#define LF 10
#endif

#ifdef CR
#undef CR
#define CR 13
#else
#define CR 13
#endif

#define UART_BUF_SIZE         16
#define ADB_UART_RINGBUF_SIZE 2048

typedef struct adbcmd_node {
    const adbserver_cmd_t *adb;
    int                    len; // cmd total num
    slist_t                next;
} adbcmd_node_t;

typedef enum
{
    NORMAL_MODE,
    PASS_THROUGH_MODE
} ADBSERVER_MODE;

typedef struct {
    ADBSERVER_MODE  mode;
    char           *data;
    int             recv_size;
    int             push_len;       /* pushpost len */
    int             last_push_len;  /* last pushpost len */
    pass_through_cb cb;
} adbserver_mode_t;

typedef struct adbserver {
    char        *buffer;
    uint16_t     buffer_size;
    uint16_t     recv_size;
    char         uart_buffer[UART_BUF_SIZE];
    uint8_t      uart_count;
    uint8_t      uart_inc;
    char         para_delimiter;
    //volatile int have_uart_event;

    int              timeout;
    const char      *output_terminator;
    slist_t          cmd_head;
    uservice_t      *srv;
    int              port;
    void            *uart_dev;
    aos_mutex_t      mutex;
    adbserver_mode_t adb_mode;
    adb_channel_t   *channel;
    int             adb_en;
    aos_log_level_t log_level; /* backup current loglevel */
} adbserver_uservice_t;

extern adb_channel_t adb_uart_channel;

typedef enum
{
    ADBSERVER_SEND_CMD,
    ADBSERVER_OUTPUT_CMD,
    ADBSERVER_PARA_CMD,
    ADBSERVER_TMOUT_CMD,
    ADBSERVER_INTERRUPT_CMD,
    ADBSERVER_PASS_THROUGH_CMD,
    ADBSERVER_PASS_THROUGH_EXIT_CMD,
    ADBSERVER_END_CMD
} ADBSERVER_CMD;

#define adbmode(adb) adb.adb_mode

static int adbserver_process_rpc(void *context, rpc_t *rpc);

static adbserver_uservice_t g_adbserver;

static void adbserver_lock(void)
{
    aos_mutex_lock(&(g_adbserver.mutex), AOS_WAIT_FOREVER);
}

static void adbserver_unlock(void)
{
    aos_mutex_unlock(&(g_adbserver.mutex));
}

static int adbserver_uart_send(const char *data, size_t size, size_t timeout)
{
    if (size == 0) {
        return 0;
    }

    int ret = g_adbserver.channel->send(g_adbserver.uart_dev, (void *)data, size);

    return ret;
}

static int adbserver_uart_recv(char *data)
{
    if (g_adbserver.uart_inc == g_adbserver.uart_count) {
        int ret = g_adbserver.channel->recv(g_adbserver.uart_dev, g_adbserver.uart_buffer, UART_BUF_SIZE, 0);
        if (ret > 0) {
            g_adbserver.uart_count = ret;
            g_adbserver.uart_inc   = 0;
        }
    }

    if (g_adbserver.uart_inc < g_adbserver.uart_count) {
        *data = g_adbserver.uart_buffer[g_adbserver.uart_inc++];

        return 1;
    }

    return 0;
}

static void channel_event(int event_id, void *priv)
{
    if (event_id == ADB_CHANNEL_EVENT_READ) {
        //if (g_adbserver.have_uart_event < 2) {
            uservice_call_async(g_adbserver.srv, ADBSERVER_INTERRUPT_CMD, NULL, 0);
            //g_adbserver.have_uart_event++;
        //}
    }
}

int adbserver_init(utask_t *task, uint8_t port, rvm_hal_uart_config_t *config)
{
    aos_assert(task);
    memset(&g_adbserver, 0, sizeof(adbserver_uservice_t));

    g_adbserver.channel = &adb_uart_channel;

    // uart init
    g_adbserver.port = port | RVM_HAL_SHADOW_PORT_MASK;
    g_adbserver.uart_dev = g_adbserver.channel->init(config, g_adbserver.port);

    g_adbserver.buffer_size = BUFFER_MIN_SIZE;
    g_adbserver.buffer      = aos_malloc(BUFFER_MIN_SIZE);
    aos_assert(g_adbserver.buffer);

    g_adbserver.timeout           = 500;
    g_adbserver.output_terminator = "\r\n";
    g_adbserver.para_delimiter    = ',';
    slist_init(&g_adbserver.cmd_head);

    // uservice
    g_adbserver.srv = uservice_new("adbserver", adbserver_process_rpc, NULL);
    aos_assert(g_adbserver.srv);

    utask_add(task, g_adbserver.srv);

    g_adbserver.channel->set_event(g_adbserver.uart_dev, channel_event, NULL);

    aos_mutex_new(&g_adbserver.mutex);

    return 0;
}

int adbserver_channel_init(utask_t *task, uint8_t port, void *config, adb_channel_t *channel)
{
    aos_assert(task);
    memset(&g_adbserver, 0, sizeof(adbserver_uservice_t));

    g_adbserver.channel = channel;

    // uart init
    g_adbserver.port = port;
    g_adbserver.uart_dev = g_adbserver.channel->init(config, g_adbserver.port);

    g_adbserver.buffer_size = BUFFER_MIN_SIZE;
    g_adbserver.buffer      = aos_malloc(BUFFER_MIN_SIZE);
    aos_assert(g_adbserver.buffer);

    g_adbserver.timeout           = 500;
    g_adbserver.output_terminator = "\r\n";
    g_adbserver.para_delimiter    = ',';
    slist_init(&g_adbserver.cmd_head);

    // uservice
    g_adbserver.srv = uservice_new("adbserver", adbserver_process_rpc, NULL);
    aos_assert(g_adbserver.srv);

    utask_add(task, g_adbserver.srv);

    g_adbserver.channel->set_event(g_adbserver.uart_dev, channel_event, NULL);

    aos_mutex_new(&g_adbserver.mutex);

    return 0;
}

void adbserver_uart_config(rvm_hal_uart_config_t *config)
{
    // if(g_adbserver.channel->config)
    //     g_adbserver.channel->config(config);
}

void adbserver_enabled(int flag)
{
    if (flag) {
        /* 关闭日志，并备份 */
        g_adbserver.log_level = aos_get_log_level();
        aos_set_log_level(AOS_LL_NONE);

        /* 串口驱动切换到ADB模式 */
        g_adbserver.uart_dev = g_adbserver.channel->init(NULL, g_adbserver.port);
        g_adbserver.channel->set_event(g_adbserver.uart_dev, channel_event, NULL);
        g_adbserver.adb_en = 1;

        /* 刷掉无效数据 */
        ulog_flush();
        aos_msleep(100);
    } else {
        g_adbserver.adb_en = 0;
        g_adbserver.channel->set_event(g_adbserver.uart_dev, NULL, NULL);
        g_adbserver.channel->deinit(g_adbserver.uart_dev);

        aos_set_log_level(g_adbserver.log_level);
    }
}

static int adbserver_cmd_check(const char *cmd)
{
    adbcmd_node_t *node;

    slist_for_each_entry(&g_adbserver.cmd_head, node, adbcmd_node_t, next)
    {
        adbserver_cmd_t *adbcmd = (adbserver_cmd_t *)node->adb;

        for (int i = 0; i < node->len; i++) {
            if (strcmp(cmd, adbcmd->cmd) == 0) {
                return (-EINVAL);
            }

            adbcmd++;
        }
    }
    return 0;
}

static int adbserver_cmd_check_group(const adbserver_cmd_t cmd[])
{
    adbserver_cmd_t *adb_cmd = (adbserver_cmd_t *)cmd;
    int              len     = 0;

    while (adb_cmd->cmd != NULL) {
        int ret = adbserver_cmd_check(adb_cmd->cmd);

        if (ret < 0) {
            return ret;
        }

        adb_cmd++;
        len++;
    }

    return len;
}

int adbserver_add_command(const adbserver_cmd_t cmd[])
{
    int len;

    aos_check_return_einval(cmd);

    len = adbserver_cmd_check_group(cmd);

    if (len < 0) {
        return (-EINVAL);
    }

    adbcmd_node_t *node = (adbcmd_node_t *)aos_malloc(sizeof(adbcmd_node_t));
    if (node) {
        node->adb = cmd;
        node->len = len;
        slist_add(&node->next, &g_adbserver.cmd_head);
        return 0;
    } else {
        return -ENOMEM;
    }
}

void adbserver_set_output_terminator(const char *terminator)
{
    uservice_call_sync(g_adbserver.srv, ADBSERVER_OUTPUT_CMD, terminator ? (void *)terminator : "", NULL, 0);
}

void adbserver_set_para_delimiter(char delimiter)
{
    uservice_call_sync(g_adbserver.srv, ADBSERVER_PARA_CMD, (void *)&delimiter, NULL, 0);
}

void adbserver_set_timeout(int timeout)
{
    uservice_call_sync(g_adbserver.srv, ADBSERVER_TMOUT_CMD, (void *)&timeout, NULL, 0);
}

static void adbserver_clear_buffer(void)
{
    if (g_adbserver.buffer_size >= BUFFER_MIN_SIZE) {
        aos_free(g_adbserver.buffer);

        g_adbserver.buffer      = aos_malloc(BUFFER_MIN_SIZE);
        g_adbserver.buffer_size = g_adbserver.buffer ? BUFFER_MIN_SIZE : 0;
    }

    g_adbserver.recv_size = 0;
}

static int adbserver_get_cmdline(void)
{
    char c    = 0;
    int  len  = 0;
    int  flag = 0;

    if (g_adbserver.recv_size != 0) {
        flag = 1;
        len  = g_adbserver.recv_size;
    }

    while (1) {
        int ret = adbserver_uart_recv(&c);

        if (ret == 0) {
            if (flag == 1) {
                g_adbserver.recv_size = len;
            }

            return 0;
        }

        if (len >= BUFFER_MAX_SIZE - 1) {
            LOGW(TAG, "buffer overflow");

            len = 0; // clear buffer
        }

        g_adbserver.buffer[len++] = c;

        if (len >= g_adbserver.buffer_size) {
            char *p;
            p = realloc(g_adbserver.buffer, g_adbserver.buffer_size + BUFFER_STEP_SIZE);

            if (p != NULL) {
                g_adbserver.buffer = p;
                g_adbserver.buffer_size += BUFFER_STEP_SIZE;
            } else {
                len = 0; // clear buffer
            }
        }

        if (flag == 0 && len > 1) {
            if ((g_adbserver.buffer[len - 2] == 'A') && (g_adbserver.buffer[len - 1] == 'T')) {
                flag                  = 1;
                g_adbserver.buffer[0] = 'A';
                g_adbserver.buffer[1] = 'T';
                len                   = 2;
            }
        } else if (flag == 1 && (c == LF || c == CR)) {
            g_adbserver.buffer[len - 1] = '\0';
            g_adbserver.recv_size       = len - 1;

            return len - 1;
        }
    }
}

static int get_cmd_len(ADB_CMD_TYPE *type)
{
    int   i;
    char *str = g_adbserver.buffer;

    for (i = 0; i <= strlen(g_adbserver.buffer); i++) {
        if (*str == '=') {
            if (*(str + 1) == '?') {
                *type = TEST_CMD;
            } else {
                *type = WRITE_CMD;
            }

            return i;
        } else if (*str == '?') {
            *type = READ_CMD;
            return i;
        } else if (*str == '\0') {
            *type = EXECUTE_CMD;
            return i;
        } else {
            str++;
        }
    }

    return -1;
}

static int get_para_num(void)
{
    int   argc = 0;
    char *p;
    char  para_delimiter = g_adbserver.para_delimiter;

    p = strstr(g_adbserver.buffer, "=");

    if (p != NULL) {
        if (*(p + 1) != '\0') {
            argc = 1;

            while (*p) {
                if (*p == para_delimiter) {
                    argc++;
                }

                p++;
            }
        } else {
            return -1;
        }
    }

    return argc;
}

static int para_cut(char *buf, char **argv)
{
    int   argc = 0;
    char *p;
    int   i;
    char  para_delimiter = g_adbserver.para_delimiter;

    if (buf == NULL) {
        return argc;
    }

    argv[argc++] = buf;

    while (*buf != '\0') {
        if (*buf == para_delimiter) {
            *buf = '\0';

            if ((*(buf + 1) == '\0') || (*(buf + 1) == para_delimiter)) {
                argv[argc++] = NULL;
            } else {
                argv[argc++] = buf + 1;
            }
        }

        buf++;
    } // cut para

    for (i = 0; i < argc; i++) {
        p = argv[i];

        while (*p != '\0') {
            if (*p != ' ') {
                break;
            } else {
                p++;
            }
        }

        argv[i] = p;
    } // remove head space

    for (i = 0; i < argc; i++) {
        p = argv[i] + strlen(argv[i]) - 1;

        while (*p != '\0') {
            if (*p == ' ') {
                //*p = '\0';
                p--;
            } else {
                p[1] = '\0';
                break;
            }
        }
    } // remove tail space

    return argc;
}

static int cmd_process(void)
{
    int            len;
    ADB_CMD_TYPE   type = INVAILD_CMD;
    adbcmd_node_t *node;

    len = get_cmd_len(&type);

    if (len < 0) {
        return -1;
    }

    slist_for_each_entry(&g_adbserver.cmd_head, node, adbcmd_node_t, next)
    {
        adbserver_cmd_t *adbcmd = (adbserver_cmd_t *)node->adb;

        for (int i = 0; i < node->len; i++) {
            if ((strncmp(g_adbserver.buffer, adbcmd->cmd, len) == 0) && (len == strlen(adbcmd->cmd))) {
                if (adbcmd->cb != NULL) {
                    char *param = NULL;

                    if (type == WRITE_CMD) {
                        param = strstr(g_adbserver.buffer, "=");
                        param++;
                    }
                    adbserver_lock();
                    adbcmd->cb((char *)adbcmd->cmd, type, param);
                    adbserver_unlock();
                    return 0;
                }
            }

            adbcmd++;
        }
    }

    return -1;
}

static int get_next_line(void)
{
    char *p_pre = g_adbserver.buffer;
    char *p     = p_pre;

    if (strlen(g_adbserver.buffer) >= 2) {
        if (p_pre) {
            p = strstr(p_pre + 2, "AT");
        }

        if (p != NULL && p != g_adbserver.buffer) {
            int len = strlen(p);

            memmove(g_adbserver.buffer, p, len);
            g_adbserver.buffer[len] = '\0';
            return 0;
        }
    }

    return -1;
}

static int pass_through_handle(void)
{
    if (adbmode(g_adbserver).mode == PASS_THROUGH_MODE) {
        int ret, recv_size, offset;

        recv_size = adbmode(g_adbserver).push_len - adbmode(g_adbserver).recv_size;
        offset    = adbmode(g_adbserver).recv_size;

        ret       = g_adbserver.channel->recv(g_adbserver.uart_dev, adbmode(g_adbserver).data + offset, recv_size, 0);
        adbmode(g_adbserver).recv_size += ret;

        if (adbmode(g_adbserver).recv_size >= adbmode(g_adbserver).push_len) {

            char *data = adbmode(g_adbserver).data;
            int   len  = adbmode(g_adbserver).push_len;

            adbmode(g_adbserver).cb(data, len); // call pass_th_cb, copy data to adb

            adbmode(g_adbserver).mode          = NORMAL_MODE;
            adbmode(g_adbserver).push_len      = 0;
            adbmode(g_adbserver).cb            = NULL;
            adbmode(g_adbserver).recv_size     = 0;

            //dont clear data & last_push_len, for next push package
            //adbmode(g_adbserver).last_push_len
            //adbmode(g_adbserver).data = NULL;

            return 0;
        }

        return -1;
    }

    return 0;
}

static int interrupt_cmd_handle(void *context, rpc_t *rpc)
{
    int ret = -1;

    if (pass_through_handle() < 0) {
        //g_adbserver.have_uart_event--;

        return 0;
    }

    while (1) {
        ret = adbserver_get_cmdline();

        if (ret > 0) {
            while (ret >= 0) {
                ret = cmd_process();

                if (ret < 0) {
                    ret = get_next_line();
                } else {
                    break;
                }
            }

            if (ret < 0) {
                const char *err = "\r\nERROR\r\n";
                adbserver_write(err, strlen(err));
            }

            adbserver_clear_buffer();
        } else {
            break;
        }
    }
    //g_adbserver.have_uart_event--;

    return 0;
}

int adbserver_scanf(const char *format, ...)
{
    va_list args;
    int     argc;
    char  **argv;
    int     ret = -1;

    aos_check_return_einval(format);
    va_start(args, format);

    argc = get_para_num();

    if (argc > 0) {
        char *copy = strdup(g_adbserver.buffer);
        if (copy == NULL) {
            va_end(args);
            return -ENOMEM;
        }

        argv = aos_malloc(sizeof(char *) * argc);
        if (argv == NULL) {
            aos_free(copy);
            va_end(args);
            return -ENOMEM;
        }

        char *param = NULL;

        param = strstr(copy, "=");
        param++;
        argc = para_cut(param, argv);

        int len    = 0;
        int offset = 0;

        if (argv[0] != NULL) {
            len = strlen(argv[0]);
            memmove(copy, argv[0], len);
            copy[len] = 0;
            offset += len;
        }

        for (int i = 1; i < argc; i++) {
            copy[offset] = g_adbserver.para_delimiter;
            offset++;
            len = 0;

            if (argv[i] != NULL) {
                len = strlen(argv[i]);
                memmove(copy + offset, argv[i], len);
                offset += len;
                copy[offset] = 0;
            }
        }

        // offset += len;
        copy[offset] = '\0';
        ret          = vsscanf(copy, format, args);

        aos_free(argv);
        aos_free(copy);
    }

    va_end(args);

    if (ret < 0) {
        return -EINVAL;
    } else {
        return ret;
    }
}

int adbserver_write(const void *data, int size)
{
    int ret = -1;

    aos_check_return_einval(data && size > 0);

    ret = adbserver_uart_send(data, size, g_adbserver.timeout);

    return ret;
}

extern int vasprintf(char **strp, const char *fmt, va_list ap);
int        adbserver_sendv(const char *command, va_list args)
{
    int   ret      = -EINVAL;
    char *send_buf = NULL;

    aos_check_return_einval(command);

    if (vasprintf(&send_buf, command, args) >= 0) {
        ret = adbserver_uart_send(send_buf, strlen(send_buf), g_adbserver.timeout);

        if (ret == 0 && g_adbserver.output_terminator != NULL) {
            ret = adbserver_uart_send(
                g_adbserver.output_terminator, strlen(g_adbserver.output_terminator), g_adbserver.timeout);
        }

        aos_free(send_buf);
    }

    return ret;
}

int adbserver_send(const char *command, ...)
{
    int     ret;
    va_list args;

    aos_check_return_einval(command);

    va_start(args, command);

    if (g_adbserver.adb_en) {
        ret = adbserver_sendv(command, args);
    } else {
        ret = vprintf(command, args);
    }

    va_end(args);

    return ret;
}

int adbserver_pass_through(int len, pass_through_cb cb)
{
    aos_check_return_einval(len && cb);

    int last_push_len = adbmode(g_adbserver).last_push_len;
 
    adbmode(g_adbserver).mode          = PASS_THROUGH_MODE;
    adbmode(g_adbserver).push_len      = len;
    adbmode(g_adbserver).last_push_len = len;
    adbmode(g_adbserver).cb            = cb;

    if ((adbmode(g_adbserver).data != NULL) && (last_push_len != len)) {
         aos_free(adbmode(g_adbserver).data);
         adbmode(g_adbserver).data = NULL;
    }

    if (adbmode(g_adbserver).data == NULL) {
        adbmode(g_adbserver).data = aos_malloc_check(len);
    }

    return 0;
}

int adbserver_pass_through_exit(void)
{
    adbmode(g_adbserver).mode      = NORMAL_MODE;
    adbmode(g_adbserver).push_len  = 0;
    adbmode(g_adbserver).cb        = NULL;
    adbmode(g_adbserver).recv_size = 0;

    aos_free(adbmode(g_adbserver).data);
    adbmode(g_adbserver).data = NULL;

    return 0;
}

static int send_cmd_handle(void *context, rpc_t *rpc)
{
    char *param = (char *)rpc_get_point(rpc);

    adbserver_uart_send(param, strlen(param), g_adbserver.timeout);
    return 0;
}

static int output_cmd_handle(void *context, rpc_t *rpc)
{
    g_adbserver.output_terminator = (char *)rpc_get_point(rpc);
    return 0;
}

static int para_cmd_handle(void *context, rpc_t *rpc)
{
    g_adbserver.para_delimiter = *(char *)rpc_get_point(rpc);
    return 0;
}

static int tmout_cmd_handle(void *context, rpc_t *rpc)
{
    g_adbserver.timeout = rpc_get_int(rpc);
    return 0;
}

static const rpc_process_t c_adbserver_cmd_cb_table[] = {
    { ADBSERVER_SEND_CMD, send_cmd_handle },
    { ADBSERVER_OUTPUT_CMD, output_cmd_handle },
    { ADBSERVER_PARA_CMD, para_cmd_handle },
    { ADBSERVER_TMOUT_CMD, tmout_cmd_handle },
    { ADBSERVER_INTERRUPT_CMD, interrupt_cmd_handle },
    { ADBSERVER_END_CMD, NULL },
};

static int adbserver_process_rpc(void *context, rpc_t *rpc)
{
    return uservice_process(context, rpc, c_adbserver_cmd_cb_table);
}
