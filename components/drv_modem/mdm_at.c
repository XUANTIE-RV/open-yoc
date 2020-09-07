/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <oss.h>
#include <aos/aos.h>
#include <arpa/inet.h>
#include <lwip/sockets.h>
#include "mdm_netif_si.h"
#include "mdm_at.h"
#include <aos/yloop.h>

#ifdef CONFIG_YOC_LPM
#include "softap_com.h"
#endif

#define assert(x)    \
    if((x) == 0) {   \
        while(1);    \
    }

#define AT_TASK_STACK_SIZE  (1024)
#define AT_IPV4V6_MAX_SIZE  (80)
#define MSG_NUM             (10)
#define ICCID_LEN           (21)

#define AT_CMD_PREFIX   10

#define in_range(c, lo, up)  ((u8_t)c >= lo && (u8_t)c <= up)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#define xchar(i)             ((i) < 10 ? '0' + (i) : 'A' + (i) - 10)
#define AT_OFFSET(TYPE, MEMBER) ((int) &((TYPE *)0)->MEMBER)

#define TAG "MDM"

#define FROM_NONE  0
#define FROME_API  1
#define FROME_USER 2

typedef struct mdm_at_recv_context {
    uint8_t from;
    mdm_at_send_cb cb;
    char *cmd;
} mdm_at_recv_context_t;

static mdm_at_recv_context_t g_mdm_recv_ctx;

//static uint8_t g_at_send_flag = 0;
//static mdm_at_send_cb g_at_send_cb = NULL;

enum mdm_at_msg_type {
    MDM_AT_FROM_USER = 0,
    MDM_AT_FROM_MDM,
};

typedef struct {
    char *at_prefix;
    int (*proc)(char *str);
} mdm_at_proc_table_t;

typedef struct {
    enum   mdm_at_msg_type type;
    char  *cmd;
    size_t size;
} mdm_at_msg_t;

extern int oss_icp_channel_deregister_cb(unsigned short cid);
extern int oss_icp_channel_register_cb(unsigned short cid, oss_icp_cb_t cb);
static int mdm_proc_zmsri_rsp(char *at_buf);
static int mdm_proc_iccid_info(char *at_buf);
static int mdm_proc_zmmi_info(char *at_buf);
static int mdm_proc_ereg_info(char *at_buf);
static int mdm_proc_cgev_rsp(char *at_buf);
static int mdm_proc_zgipdns_rsp(char *at_buf);
static int mdm_proc_zdatopt_info(char *at_buf);

static unsigned char at_cur_plane = 1;
static aos_queue_t g_mdm_msg_queue;
static char g_msg_buf[sizeof(mdm_at_msg_t) * MSG_NUM] = {0};
static aos_sem_t g_mdm_sem_handle = { NULL };
static aos_task_t g_mdmat_tsk_hdl = { NULL };
static char iccid[ICCID_LEN] = {0};

static const mdm_at_proc_table_t rsp_proc_table[] = {
    {"+ZMSRI",      mdm_proc_zmsri_rsp},      /* CP RESET */
    {"+CGEV: ",     mdm_proc_cgev_rsp},       /* Loss connect */
    {"+ZGIPDNS: ",  mdm_proc_zgipdns_rsp},    /* Get IP */
    {"+ZDATOPT: ",  mdm_proc_zdatopt_info},   /* Set active mod */
    {"+CEREG: ",    mdm_proc_ereg_info},      /* ereg mod */
    {"+ZICCID: ",   mdm_proc_iccid_info},     /* ereg mod */
    {"+ZMMI: ",     mdm_proc_zmmi_info},      /* set time  */
    {"",            NULL}
};

static int _cmd_is_end(const char *at_buf)
{
    if ('\0' == *at_buf) {
        return 1;
    }

    if ('\r' == *at_buf) {
        return 1;
    }

    return 0;
}

static const char *_skip_str(const char *at_buf)
{
    const char *str_cur = at_buf;

    for (; !_cmd_is_end(str_cur); str_cur++) {
        if (',' == *str_cur || '"' == *str_cur) {
            break;
        }
    }

    return str_cur;
}

static const char *_skip_cmd_head_blanks(const char *at_buf)
{
    const char *str_cur = at_buf;

    for (; !_cmd_is_end(str_cur); str_cur++) {
        if (' ' != *str_cur) {
            break;
        }
    }

    return str_cur;
}

static void _set_next_param(char **next_param, const char *next_value)
{
    if (next_param != NULL) {
        *next_param = (char *)next_value;
    }
}

static const char *_extract_int(const char *str_param, int *int_param)
{
    char c_cur;
    char str_int[9] = {0};
    int offset;

    for (offset = 0; !_cmd_is_end(str_param + offset); offset++) {
        c_cur = *(str_param + offset);

        if (!isdigit(c_cur)) {
            break;
        }

        str_int[offset] = c_cur;
    }

    if (NULL != int_param) {
        *int_param = atoi(str_int);
    }

    return str_param + offset;
}

static int _get_cmd_param_int(const char *at_buf, int *param, char **at_next)
{
    const char *str_next = at_buf;

    if (NULL == str_next || NULL == param) {
        return -1;
    }

    str_next = _skip_cmd_head_blanks(at_buf);
    _set_next_param(at_next, str_next);

    if (',' == *str_next || _cmd_is_end(str_next)) {
        return -1;
    }

    if (!isdigit(*str_next)) {
        return -1;
    }

    str_next = _extract_int(str_next, param);
    _set_next_param(at_next, str_next);

    if (' ' == *str_next || ',' == *str_next || _cmd_is_end(str_next)) {
        return 0;
    } else if (isdigit(*str_next)) {
        return -1;
    } else {
        return -1;
    }
}

static int _get_cmd_param_str(const char *at_buf, char *str_param,
                              int param_len, char **at_next)
{
    int real_len;
    const char *str_next;
    const char *str_cur = at_buf;

    if (NULL == str_cur || NULL == str_param || 0 == param_len) {
        _set_next_param(at_next, str_cur);
        return -1;
    }

    str_next = _skip_cmd_head_blanks(str_cur);
    _set_next_param(at_next, str_next);

    if (_cmd_is_end(str_next)) {
        return -1;
    }

    if (',' == *str_next) {
        return -1;
    }

    if ('"' != *str_next) {
        return -1;
    }

    str_next = _skip_str(++str_cur);
    _set_next_param(at_next, str_next);

    if ('"' != *str_next) {
        return -1;
    }

    real_len = str_next - str_cur;
    _set_next_param(at_next, ++str_next);

    if (real_len >= param_len) {
        return -1;
    }

    if (real_len > 0) {
        memset(str_param, 0, param_len);
        memcpy(str_param, str_cur, real_len);
    }

    return 0;
}

#if 0
int _addr_aton(int family, const char *src, void *dst)
{
    size_t i, len;

    len = strlen(src);

    for (i = 0; i < len; i++) {
        if (!(src[i] >= '0' && src[i] <= '9') &&
            src[i] != '.') {
            return -EINVAL;
        }
    }

    if (family == AF_INET) {
        struct in_addr *addr = (struct in_addr *)dst;
        memset(addr, 0, sizeof(struct in_addr));

        for (i = 0; i < sizeof(struct in_addr); i++) {
            char *endptr;

            addr->s4_addr[i] = strtol(src, &endptr, 10);

            src = ++endptr;
        }
    } else if (family == AF_INET6)  {
        struct in6_addr *addr = (struct in6_addr *)dst;
        memset(addr, 0, sizeof(struct in6_addr));

        for (i = 0; i < sizeof(struct in6_addr); i++) {
            char *endptr;

            addr->s6_addr[i] = strtol(src, &endptr, 10);

            src = ++endptr;
        }
    }

    return 0;
}


static int _v4addr_aton(const char *cp, unsigned int *addr)
{
    unsigned int val;
    unsigned char base;
    char c;
    unsigned int parts[4];
    unsigned int *pp = parts;

    c = *cp;

    for (;;) {
        /*
         * Collect number up to ``.''.
         * Values are specified as for C:
         * 0x=hex, 0=octal, 1-9=decimal.
         */
        if (!isdigit(c)) {
            return 0;
        }

        val = 0;
        base = 10;

        if (c == '0') {
            c = *++cp;

            if (c == 'x' || c == 'X') {
                base = 16;
                c = *++cp;
            } else {
                base = 8;
            }
        }

        for (;;) {
            if (isdigit(c)) {
                val = (val * base) + (int)(c - '0');
                c = *++cp;
            } else if (base == 16 && isxdigit(c)) {
                val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            } else {
                break;
            }
        }

        if (c == '.') {
            /*
             * Internet format:
             *  a.b.c.d
             *  a.b.c   (with c treated as 16 bits)
             *  a.b (with b treated as 24 bits)
             */
            if (pp >= parts + 3) {
                return 0;
            }

            *pp++ = val;
            c = *++cp;
        } else {
            break;
        }
    }

    /*
     * Check for trailing characters.
     */
    if (c != '\0' && !isspace(c)) {
        return 0;
    }

    /*
     * Concoct the address according to
     * the number of parts specified.
     */
    switch (pp - parts + 1) {

        case 0:
            return 0;       /* initial nondigit */

        case 1:             /* a -- 32 bits */
            break;

        case 2:             /* a.b -- 8.24 bits */
            if (val > 0xffffffUL) {
                return 0;
            }

            if (parts[0] > 0xff) {
                return 0;
            }

            val |= parts[0] << 24;
            break;

        case 3:             /* a.b.c -- 8.8.16 bits */
            if (val > 0xffff) {
                return 0;
            }

            if ((parts[0] > 0xff) || (parts[1] > 0xff)) {
                return 0;
            }

            val |= (parts[0] << 24) | (parts[1] << 16);
            break;

        case 4:             /* a.b.c.d -- 8.8.8.8 bits */
            if (val > 0xff) {
                return 0;
            }

            if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
                return 0;
            }

            val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
            break;

        default:
            assert(0);
            break;
    }

    if (addr) {
        *addr = htonl(val);
    }

    return 1;
}


static int _get_ipv4_len_from_ipv4v6(char *ipv4v6_str)
{
    int i;
    char *str_start;
    char *str_temp = ipv4v6_str;

    for (i = 0; i < 4; i++) {
        str_start = strchr(str_temp, '.');

        if (NULL == str_start) {
            return 0;
        }

        str_temp = str_start + 1;
    }

    return (int)(str_start - ipv4v6_str);
}

static int _get_ip_form_zgipdns(char *buff, unsigned int *ip)
{
    int ipv4_len = _get_ipv4_len_from_ipv4v6(buff);

    if (ipv4_len != 0) {
        *(buff + ipv4_len) = '\0';
    }

    return _v4addr_aton(buff, ip);
}
#endif

static char *_strnchr(const char *s, int c, int n)
{
    char *match = NULL;
    int i = 0;

    do {
        if (*s == (char)c) {
            i++;

            if (i == n) {
                match = (char *)s;
                break;
            }
        }
    } while (*s++);

    return match;
}


int _get_ip46_form_zgipdns(char *buff, void *ip, void *ipv6)
{
    char *tmp = _strnchr(buff, '.', 4);

    if (tmp == NULL) {
        return -1;
    }

    *tmp = '\0';

    //v4
    if (inet_pton(AF_INET, buff, ip) != 1) {
        return -1;
    }

    //v6
    if (inet_pton(AF_INET6, tmp + 1, ipv6) != 1) {
        return -1;
    }

    return 0;
}

static int _move_to_next_param(const char *at_buf, char **next_param)
{
    const char *str_cur = at_buf;

    if (NULL == str_cur || NULL == next_param) {
        return -1;
    }

    str_cur = _skip_cmd_head_blanks(str_cur);

    if (_cmd_is_end(str_cur)) {
        return -1;
    }

    if (',' != *str_cur) {
        return -1;
    }

    str_cur++;
    *next_param = (char *)_skip_cmd_head_blanks(str_cur);
    return 0;
}


static int mdm_at_send_msg(mdm_at_msg_t *msg)
{
    int ret;

    if (g_mdm_msg_queue.hdl == NULL || msg == NULL) {
        return -1;
    }

    //LOGI(TAG, "mdm_at_send_msg msg->type=%d", msg->type);
    
    aos_queue_send(&g_mdm_msg_queue, (void *)msg, sizeof(mdm_at_msg_t));

    if (ret != 0) {
        LOGE(TAG, "failed (%d)", ret);

        return ret;
    }

    return 0;
}


static int mdm_send_from_cp(void *data, size_t size)
{
    mdm_at_msg_t mdm_msg;
    int ret;

    /* Shoud free where finish proc atcmd */
    char *cmd = aos_malloc_check(size);

    if (cmd == NULL) {
        return -1;
    }

    memcpy(cmd, data, size);

    /* Post event to msgq */
    mdm_msg.type = MDM_AT_FROM_MDM;
    mdm_msg.cmd  = cmd;
    mdm_msg.size = size;
    ret = mdm_at_send_msg(&mdm_msg);

    if (ret != 0) {
        aos_free(cmd);
        LOGE(TAG, "mdm_at_send_msg failed");
        return -1;
    }

    return 0;
}

/* Execute in irq, do not delay or print */
static void mdm_recv_atfree(void *data, unsigned short len)
{
    at_icp_msg_t *msg = (at_icp_msg_t *)data;

    assert(sizeof(*msg) == len);
    msg->data = (void *)oss_bus2phy((oss_bus_t)msg->data);

    if (oss_event_post(OSS_EVENT_FREE, (unsigned int)(msg->data))) {
        LOGE(TAG, "mdm_recv_atfree failed");
    }

    //mdm_psm_unref();
}

/* Called by at task, send free signal to mdm */
static void mdm_send_atfree(const void *buf, size_t size)
{
    int try_times = 0;
    at_icp_msg_t msg;

    msg.data = (void *)oss_phy2bus((oss_phy_t)buf);
    msg.size = size;

    while (oss_icp_channel_send(OSS_ICP_CHANNEL_AT_FREE, &msg, sizeof(msg)) != 0) {
        try_times ++;
        LOGE(TAG, "oss_icp_channel_send failed counter(%d)", try_times);
        aos_msleep(10);
    }
}

/* Execute in oss task */
static void mdm_recv_atsend(void *data, unsigned int len)
{
    at_icp_msg_t *msg = (at_icp_msg_t *)data;
    int ret;

    assert(sizeof(*msg) == len);
    msg->data = (void *)oss_bus2phy((oss_bus_t)msg->data);

    LOGD(TAG, "recv from cp, len:%d, cmd: %s",
         msg->size, (char *)msg->data);

    if ((0 != strncmp((char *)msg->data, "\r\n", 2))
        || (msg->size != strlen((char *)msg->data) + 1)) {
        LOGE(TAG, "mdm_recv_atsend recv atcmd format error");
        assert(0);
    }

    /* Send atcmd to at proc task */
    ret = mdm_send_from_cp(msg->data, msg->size);

    if (ret != 0) {
        LOGE(TAG, "mdm_send_from_cp failed");
    }

    /* Inform CP to free data */
    mdm_send_atfree(msg->data, msg->size);
}

/* Called by at task, send at cmd to mdm */
static void mdm_send_atsend(const void *buf, size_t size)
{
    int try_times = 0;
    at_icp_msg_t msg;

    msg.data = (void *)oss_phy2bus((oss_phy_t)buf);
    msg.size = size;

    LOGD(TAG, "send to cp, len:%d ,cmd:\n%s\n", size, (char *)buf);

    while (oss_icp_channel_send(OSS_ICP_CHANNEL_AT_SEND, &msg, sizeof(msg)) != 0) {
        try_times ++;
        LOGE(TAG, "oss_icp_channel_send failed counter(%d)", try_times);
        aos_msleep(10);
    }

    //mdm_psm_ref(MDM_CMD_SEND);
}

static int mdm_proc_zdatopt_info(char *at_buf)
{
    at_cur_plane = atoi(at_buf);

    if (at_cur_plane == 3) {
        at_cur_plane = 1;
    } else if (at_cur_plane == 4) {
        at_cur_plane = 2;
    }

    if (at_cur_plane <= 0 || at_cur_plane > 4) {
        at_cur_plane = 1;
    }

    mdm_modflag_set(at_cur_plane);
    return 0;
}

static int mdm_proc_zmsri_rsp(char *at_buf)
{
    char *msg = strdup("AT+CFUN=1\r");

    if (msg == NULL) {
        LOGE(TAG, "mdm_proc_zmsri_rsp strdup failed");
        return -1;
    }

    mdm_send_atsend(msg, strlen(msg) + 1);

#ifdef CONFIG_YOC_LPM
    uint8_t is_at_ready = 1;
    oss_nv_write(RECVDATA_BASE + AT_OFFSET(global_cfg_t, is_at_ready), (void *)&is_at_ready, 1);
#endif
    return 0;
}

static int mdm_proc_ereg_info(char *at_buf)
{
    int mod = atoi(at_buf);

    if (mod == 0 || mod == 4) {
        aos_post_event(EV_NET, CODE_NET_ON_DISCONNECT, VALUE_NET_NBIOT);
    }

    return 0;
}

static int mdm_proc_iccid_info(char *at_buf)
{
    strlcpy(iccid, at_buf, ICCID_LEN);

    return 0;
}

char *_next_time_val(char *start)
{
    while (*start != '/' && *start != ':' && *start != ',' && *start != '\n' && *start != '\r' && *start != '"' && *start != 0) {
        start ++;
    }

    return start + 1;
}

static int mdm_proc_zmmi_info(char *at_buf)
{
    char *at_paras = at_buf;
    char *str_time_start;
    int time_zone = atoi(at_buf) % 24;
    int time_val[6], i;
    struct tm timetm = {0};
    struct timeval tv;

    at_paras = _strnchr(at_paras, ',', 4);
    str_time_start = strchr(at_paras, '"');

    if (str_time_start == NULL) {
        return -1;
    }

    str_time_start += 1;

    for (i = 0; i < 6; i++) {
        time_val[i] = atoi(str_time_start);
        str_time_start = _next_time_val(str_time_start);
    }

    timetm.tm_year = time_val[0] + 100;
    timetm.tm_mon = time_val[1] - 1;
    timetm.tm_mday = time_val[2];
    timetm.tm_hour = time_val[3] + time_zone;
    timetm.tm_min = time_val[4];
    timetm.tm_sec = time_val[5];
    timetm.tm_isdst = 0;

    time_t second = mktime(&timetm);

    tv.tv_sec = second;
    tv.tv_usec = 0;

    LOGD(TAG, "time sec: %d", second);

    if (settimeofday(&tv, NULL) != 0) {
        return -1;
    }

    return 0;
}

static int mdm_proc_cgev_rsp(char *at_buf)
{
    char *at_paras = strstr(at_buf, "PDN DEACT");

    if (at_paras != NULL) {
        int cid = 0;
        _get_cmd_param_int(at_paras + strlen("PDN DEACT"), &cid, &at_paras);
        mdm_netif_pdp_deact(cid, 0);
    }

    return 0;
}

static int mdm_proc_zgipdns_rsp(char *at_buf)
{
    char *at_paras = at_buf;
    char buff[AT_IPV4V6_MAX_SIZE] = { 0 };
    struct pdp_active_info pdp_info = { 0 };
    unsigned int *ip_addrs[4] = {&pdp_info.act_info.ip,
                                 &pdp_info.act_info.gateway,
                                 &pdp_info.act_info.pri_dns,
                                 &pdp_info.act_info.sec_dns
                                };
    int cid_num = 0;
    int cid = 0;
    int i;

    _get_cmd_param_int(at_paras, &cid_num, &at_paras);

    if (cid_num > 0 && pdp_info.c_id == 0) {
        _move_to_next_param(at_paras, &at_paras);
        _get_cmd_param_int(at_paras, &cid, &at_paras);
        pdp_info.c_id = (unsigned char)cid;

        if (pdp_info.c_id == 0) {
            return -1;
        }

        _move_to_next_param(at_paras, &at_paras);
        _get_cmd_param_str(at_paras, buff, AT_IPV4V6_MAX_SIZE, &at_paras);
        _move_to_next_param(at_paras, &at_paras);


        if (strcmp("IPV6", buff) == 0) {
            pdp_info.act_info.ip46flag = V6_VALID;
            memset(buff, 0, sizeof(buff));
            _get_cmd_param_str(at_paras, buff, AT_IPV4V6_MAX_SIZE, &at_paras);

            if (inet_pton(AF_INET6, buff, &pdp_info.act_info.ipv6) != 1) {
                return -1;
            }
        } else if (strcmp("IP", buff) == 0) {
            pdp_info.act_info.ip46flag = V4_VALID;

            for (i = 0; i < 4; i++) {
                memset(buff, 0, sizeof(buff));
                _get_cmd_param_str(at_paras, buff, AT_IPV4V6_MAX_SIZE, &at_paras);

                if (inet_pton(AF_INET, buff, ip_addrs[i]) != 1) {
                    return -1;
                }

                _move_to_next_param(at_paras, &at_paras);
            }
        } else if (strcmp("IPV4V6", buff) == 0) {
            pdp_info.act_info.ip46flag = V46_VALID;

            for (i = 0; i < 4; i++) {
                memset(buff, 0, sizeof(buff));
                _get_cmd_param_str(at_paras, buff, AT_IPV4V6_MAX_SIZE, &at_paras);

                if (i == 0) {
                    if (_get_ip46_form_zgipdns(buff, ip_addrs[i], &pdp_info.act_info.ipv6) != 0) {
                        return -1;
                    }
                } else {
                    if (inet_pton(AF_INET, buff, ip_addrs[i]) != 1) {
                        return -1;
                    }
                }

                _move_to_next_param(at_paras, &at_paras);
            }
        } else if (strcmp("Non-IP", buff) == 0) {

            return 0;
        } else {
            assert(0);
        }

        pdp_info.mod_flags = at_cur_plane;
        pdp_info.pdp_type = PDP_NORMAL;
        mdm_netif_pdp_act(&pdp_info);
    }

    return 0;
}


static int mdm_proc_atcmd(const char *atcmd, const mdm_at_proc_table_t *table)
{
    int   tab_idx;
    char *at_prefix;

    for (tab_idx = 0; table[tab_idx].proc != NULL; tab_idx++) {
        at_prefix = strstr(atcmd, table[tab_idx].at_prefix);

        if (at_prefix != NULL) {
            return table[tab_idx].proc(at_prefix + strlen(table[tab_idx].at_prefix));
        }
    }

    return -1;
}

static int is_result_at(char *str)
{
    return ((strstr(str, "OK\r") != NULL) || (strstr(str, "\r\n+CME ERROR") != NULL)
            || (strstr(str, "\r\n+CMS ERROR") != NULL) || (strstr(str, "ERROR\r") != NULL)
            || (strstr(str, "ERROR:") != NULL));
}

/* Called by at task, proc at cmd from mdm */
static int mdm_rsp_mdm(char *atcmd, size_t size)
{
    int need_filter;

    /* Proc the cmd, success if retrun 0 */
    need_filter = mdm_proc_atcmd(atcmd, rsp_proc_table);

    if (is_result_at(atcmd)) {

        if (g_mdm_recv_ctx.from == FROME_API) {
            if (g_mdm_recv_ctx.cb) {
                g_mdm_recv_ctx.cb(atcmd);
            }
            aos_sem_signal(&g_mdm_sem_handle);
        } else if (g_mdm_recv_ctx.from == FROME_USER) {
            need_filter = 0;
            aos_sem_signal(&g_mdm_sem_handle);
        }

        g_mdm_recv_ctx.from = FROM_NONE;
    }

    if (need_filter == 0) {
#if defined (CONFIG_AT)
        /* Send the cmd to user */
        if (at_event_send(AT_MSG_CMD, AT_EVENT_CMD_BACK, 0, 0, atcmd)) {
            LOGE(TAG, "at_event_send failed");
            aos_free(atcmd);
        }
#else
        aos_free(atcmd);
#endif
    }  else {
        aos_free(atcmd);
    }

    return need_filter;
}

/* MDM at task for proc CP atcmd and proc user atcmd */
static void mdm_at_task(void *argv)
{
    int ret;
    mdm_at_msg_t msg;

    while (1) {
        /* Get msg for queue */
        aos_queue_recv(&g_mdm_msg_queue, AOS_WAIT_FOREVER, &msg, sizeof(mdm_at_msg_t));

        if (ret != 0) {
            LOGE(TAG, "mdm_at_task msgq_get failed (%d)", ret);
            continue;
        }

        switch (msg.type) {
            case MDM_AT_FROM_USER:

                /* Pass to CP, if recv from user */
                if (msg.cmd != NULL && (strlen(msg.cmd) + 1) == msg.size) {
                    mdm_send_atsend(msg.cmd, msg.size);
                }

                break;

            case MDM_AT_FROM_MDM:
                /* Respond to CP, if recv from CP */
                mdm_rsp_mdm(msg.cmd, msg.size);
                break;

            default:
                LOGE(TAG, "worry type");
                assert(0);
                break;
        }
    }

    aos_task_exit(0);
}

int mdm_at_init(void)
{
    int ret;

#ifdef CONFIG_AMT
    extern int is_amt_mode(void);

    if (is_amt_mode()) {
        return 0;
    }

#endif

    /* Register cb to oss layer */
    ret = oss_icp_channel_register_cb(OSS_ICP_CHANNEL_AT_SEND,
                                      (oss_icp_cb_t)mdm_recv_atsend);

    if (ret) {
        LOGE(TAG, "mdm_at_init register failed");
        return -1;
    }

    /* Register cb to oss layer */
    ret = oss_icp_channel_register_cb(OSS_ICP_CHANNEL_AT_FREE,
                                      (oss_icp_cb_t)mdm_recv_atfree);

    if (ret) {
        oss_icp_channel_deregister_cb(OSS_ICP_CHANNEL_AT_SEND);
        LOGE(TAG, "mdm_at_init register failed");
        return -1;
    }

    aos_queue_new(&g_mdm_msg_queue, g_msg_buf, sizeof(g_msg_buf), sizeof(mdm_at_msg_t));

    if (g_mdm_msg_queue.hdl == NULL) {
        LOGE(TAG, "mdm_at_init csi_kernel_msgq_new failed");
        goto failed;
    }

    ret = aos_task_new_ext(&g_mdmat_tsk_hdl, "mdm_at_task", mdm_at_task,
                           NULL, AT_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);

    if (ret) {
        aos_queue_free(&g_mdm_msg_queue);
        LOGE(TAG, "mdm_at_init task new failed");
        goto failed;
    }

    aos_sem_new(&g_mdm_sem_handle, 1);

    if (g_mdm_sem_handle.hdl == NULL) {
        aos_sem_free(&g_mdm_msg_queue);
        LOGE(TAG, "mutex");
        goto failed;
    }

    if (mdm_api_init()) {
        aos_queue_free(&g_mdm_msg_queue);
        aos_sem_free(&g_mdm_sem_handle);
        LOGE(TAG, "api init");
        goto failed;
    }

#ifdef CONFIG_YOC_LPM
    uint8_t is_at_ready = 0;

    oss_nv_read(RECVDATA_BASE + AT_OFFSET(global_cfg_t, is_at_ready), &is_at_ready, 1);

    if (is_at_ready == 1) {
        char *msg = strdup("AT+CFUN=1\r");

        if (msg == NULL) {
            LOGE(TAG, "mdm_proc_zmsri_rsp strdup failed");
            return -1;
        }

        mdm_send_atsend(msg, strlen(msg) + 1);
    }

#endif

    return 0;

failed:
    oss_icp_channel_deregister_cb(OSS_ICP_CHANNEL_AT_SEND);
    oss_icp_channel_deregister_cb(OSS_ICP_CHANNEL_AT_FREE);

    return -1;
}

int mdm_at_deinit(void)
{
    oss_icp_channel_deregister_cb(OSS_ICP_CHANNEL_AT_SEND);
    oss_icp_channel_deregister_cb(OSS_ICP_CHANNEL_AT_FREE);

    aos_queue_free(&g_mdm_msg_queue);

    g_mdm_msg_queue.hdl = NULL;

    aos_sem_free(&g_mdm_sem_handle);

    return 0;
}

/* Called by user, send at cmd to at task */
int mdm_send_from_user(const char *atcmd, size_t size)
{
    int ret;
    mdm_at_msg_t msg;
    char *cmd;

    if (atcmd == NULL || (strlen(atcmd)) != size) {
        return -1;
    }

    /* Malloc the atcmd, will be free in mdm_recv_atfree */
    cmd = strdup(atcmd);

    if (cmd == NULL) {
        return -1;
    }

    aos_sem_wait(&g_mdm_sem_handle, AOS_WAIT_FOREVER);

    /* Post event to msgq */
    msg.type = MDM_AT_FROM_USER;
    msg.cmd  = cmd;
    msg.size = size + 1;
    ret = mdm_at_send_msg(&msg);

    if (ret != 0) {
        aos_free(cmd);
        LOGE(TAG, "mdm_at_send_msg failed");
        aos_sem_signal(&g_mdm_sem_handle);
        return -1;
    }

    g_mdm_recv_ctx.from = FROME_USER;
    g_mdm_recv_ctx.cmd = cmd;
    //g_mdm_recv_ctx.cb = cb;

    return 0;
}

/* Called by user, send at cmd to at task */
int mdm_send_from_api(const char *atcmd, size_t size, mdm_at_send_cb cb)
{
    int ret;
    mdm_at_msg_t msg;
    char *cmd;

    if (atcmd == NULL || (strlen(atcmd)) != size) {
        return -1;
    }

    /* Malloc the atcmd, will be free in mdm_recv_atfree */
    cmd = strdup(atcmd);

    if (cmd == NULL) {
        return -1;
    }

    aos_sem_wait(&g_mdm_sem_handle, AOS_WAIT_FOREVER);

    /* Post event to msgq */
    msg.type = MDM_AT_FROM_USER;
    msg.cmd  = cmd;
    msg.size = size + 1;
    ret = mdm_at_send_msg(&msg);

    if (ret != 0) {
        aos_free(cmd);
        LOGE(TAG, "mdm_at_send_msg failed");
        aos_sem_signal(&g_mdm_sem_handle);
        return -1;
    }

    g_mdm_recv_ctx.from = FROME_API;
    g_mdm_recv_ctx.cb = cb;
    g_mdm_recv_ctx.cmd = cmd;

    return 0;
}
