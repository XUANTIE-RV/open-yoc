/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __AT_UART_INTERNAL_H__
#define __AT_UART_INTERNAL_H__

#include <string.h>
#include <sys/types.h>
#include <yoc/atserver.h>

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_AT)
#define AT_LOGD(tag, format, ...) LOGD(tag, format, ##__VA_ARGS__)
#else
#define AT_LOGD(tag, format, ...)
#endif

#define AT_LOGI(tag, format, ...) LOGI(tag, format, ##__VA_ARGS__)
#define AT_LOGW(tag, format, ...) LOGW(tag, format, ##__VA_ARGS__)
#define AT_LOGE(tag, format, ...) LOGE(tag, format, ##__VA_ARGS__)

#define AT_OK              0      /*!< No error */
#define AT_ERR            (-1)    /*!< General fail code */
#define AT_ERR_NO_KEY     (-2)    /*!< Key not set */
#define AT_ERR_INVAL      (-3)    /*!< Invalid argument */
#define AT_ERR_STATUS     (-4)    /*!< Invalid status */
#define AT_ERR_RECV       (-5)    /*!< socket API: Fail to recv data */
#define AT_ERR_SEND       (-6)    /*!< socket API: Fail to send data */
#define AT_ERR_START      (-7)    /*!< socket API: Fail to start */
#define AT_ERR_CHECK      (-8)    /*!< data check error*/
#define AT_ERR_FOTA_NOVER (-9)    /* !< no version */
#define AT_ERR_FOTA_MDVER (-10)   /* !< version for module */
#define AT_ERR_PROC       (-11)   /* !< process failed*/

#define AT_OK_WAIT        (1)     /*!< Need to recv more data */

#define AT_CMD_MAX_LEN      (2048)
#define AT_CMD_MAX_CMD_LEN  (32)
#define AT_CMD_MAX_DATA_LEN (AT_CMD_MAX_LEN - AT_CMD_MAX_CMD_LEN - 2)

enum ATCMD_TYPE {
    AT_TEST_CMD = 0,
    AT_QUERY_CMD,
    AT_SETUP_CMD,
    AT_EXEC_CMD,
    AT_CMD_MAX,
};

typedef struct {
    int8_t id;                          /**< at_cmd ID */
    enum ATCMD_TYPE type;               /**< at_cmd type */
    uint8_t *at_data;                   /**< setup cmd: point to data */
    uint16_t cmd_len;                   /**< at cmd len */
    uint16_t data_len;                  /**< setup cmd: data len */
    uint16_t extra_len;                 /**< setup cmd / bin mode: extra_len = strlen(len str + ',') */
    uint16_t full_len;                  /**< at cmd full len */
    uint8_t is_mul_frame;               /**< multi-frame mode: if still wait to recv */
    uint16_t read_sz;                   /**< multi-frame mode: read data size */
} at_cmd_t;

typedef enum {
    EVENT_RECV = 1,
    EVENT_ACCEPT,
    EVENT_CONN,
    EVENT_DISCONN,
    EVENT_SLEEP,
    EVENT_TIMEOUT,
} sock_evt_t;


#define CRLF          "\r\n"
#define OK_CRLF       "OK\r\n"
#define CRLF_OK_CRLF  "\r\nOK\r\n"

#define ERR_CRLF      "ERROR\r\n"
#define CRLF_ERR_CRLF "\r\nERROR\r\n"

#define AT_BACK_CMD(cmd)                      atserver_send("%s\r\n", cmd + 2)
#define AT_BACK_OK()                          atserver_send("\r\nOK\r\n")
#define AT_BACK_ERR()                         atserver_send("\r\nERROR\r\n")
#define AT_BACK_ERRNO(errno)                  atserver_send("\r\nERROR: %d\r\n", errno)
#define AT_BACK_RET_OK(cmd, par1)             atserver_send("\r\n%s:%s\r\nOK\r\n", cmd + 2, par1)

#define AT_BACK_OK_INT(val)                   atserver_send("\r\n%d\r\nOK\r\n", val)
#define AT_BACK_OK_INT2(val1, val2)           atserver_send("\r\n%d,%d\r\nOK\r\n", val1, val2)
#define AT_BACK_RET_OK_INT(cmd, val)          atserver_send("\r\n%s:%d\r\nOK\r\n", cmd + 2, val)
#define AT_BACK_RET_OK_INT2(cmd, val1, val2)  atserver_send("\r\n%s:%d,%d\r\nOK\r\n", cmd + 2, val1, val2)
#define AT_BACK_RET_OK2(cmd, par1, par2)      atserver_send("\r\n%s:%s,%s\r\nOK\r\n", cmd + 2, par1, par2)
#define AT_BACK_RET_OK_HEAD(cmd, ret)         atserver_send("\r\n%s:%s", cmd + 2, ret)
#define AT_BACK_RET_OK_CMD_HEAD(cmd)          atserver_send("\r\n%s:", cmd + 2)
#define AT_BACK_RET_OK_END()                  atserver_send("\r\nOK\r\n")
#define AT_BACK_RET_ERR(cmd, errno)           atserver_send("\r\n%s:%d\r\nERROR\r\n", cmd + 2, errno)
#define AT_BACK_CME_ERR(errno)                atserver_send("\r\nCME ERROR: %d\r\n", errno)
#define AT_BACK_CIS_ERR(errno)                atserver_send("\r\n+CIS ERROR: %d\r\n", errno)
#define AT_BACK_STR(str)                      atserver_send("%s", str)

int32_t at_cmd_echo_on(void);
int32_t at_cmd_event_on(void);

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
int is_digit_str(const char *str);
int at_parse_param_safe(char *fmt, char *buf, void **pval);

#endif
