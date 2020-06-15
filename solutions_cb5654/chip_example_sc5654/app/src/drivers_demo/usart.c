/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app_config.h>

#include <stdio.h>

#include <drv/usart.h>
#include <soc.h>
#include <pin_name.h>
#include <pinmux.h>

#include <aos/cli.h>

typedef struct {
    int32_t    tx;
    int32_t    rx;
    uint16_t cfg_idx;
    uint32_t   tx_func;
    uint32_t   rx_func;
} usart_pinmap_t;
const static usart_pinmap_t g_usart_pinmap[] = {
    {
        PB0,
        PB1,
        0,
        PB0_UART1_TX,
        PB1_UART1_RX
    },
    {
        PA4,
        PA5,
        1,
        PA4_UART2_TX,
        PA5_UART2_RX
    },
    {
        PD7,
        PD8,
        2,
        PD7_UART3_TX,
        PD8_UART3_RX
    },
    {
        PD9,
        PD10,
        3,
        PD9_UART4_TX,
        PD10_UART4_RX
    },
};

static usart_handle_t g_usart_handle;
static volatile uint8_t rx_async_flag = 0;
static volatile uint8_t tx_async_flag = 0;
static volatile uint8_t rx_trigger_flag = 0;

uint8_t  data[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

static int32_t usart_receive_sync(usart_handle_t usart, void *data, uint32_t num)
{
    int time_out = 0x7ffff;
    usart_status_t status;

    csi_usart_receive(usart, data, num);

    while (time_out) {
        time_out--;
        status = csi_usart_get_status(usart);

        if (!status.rx_busy) {
            break;
        }
    }

    if (0 == time_out) {
        return -1;
    }

    return 0;
}

static int32_t usart_send_sync(usart_handle_t usart, const void *data, uint32_t num)
{
    int time_out = 0x7ffff;
    usart_status_t status;

    csi_usart_send(usart, data, num);

    while (time_out) {
        time_out--;
        status = csi_usart_get_status(usart);

        if (!status.tx_busy) {
            break;
        }
    }

    if (0 == time_out) {
        return -1;
    }

    return 0;
}

static int32_t usart_receive_async(usart_handle_t usart, void *data, uint32_t num)
{
    int time_out = 0x7fffff;
    rx_async_flag = 0;

    csi_usart_receive(usart, data, num);

    while (time_out) {
        time_out--;

        if (rx_async_flag == 1) {
            break;
        }
    }

    if (0 == time_out) {
        return -1;
    }

    rx_async_flag = 0;
    return 0;
}

static int32_t usart_send_async(usart_handle_t usart, const void *data, uint32_t num)
{
    int time_out = 0x7ffff;
    tx_async_flag = 0;

    csi_usart_send(usart, data, num);

    while (time_out) {
        time_out--;

        if (tx_async_flag == 1) {
            break;
        }
    }

    if (0 == time_out) {
        return -1;
    }

    tx_async_flag = 0;
    return 0;
}

static void usart_event_cb(int32_t idx, uint32_t event)
{
    uint8_t g_data[15];

    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            tx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVE_COMPLETE:
            rx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVED:
            csi_usart_receive_query(g_usart_handle, g_data, 15);

        default:
            break;
    }
}

static void usart_event_cb_query(int32_t idx, uint32_t event)
{
    uint8_t g_data[15];
    uint8_t ret;

    switch (event) {
        case USART_EVENT_SEND_COMPLETE:
            tx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVE_COMPLETE:
            rx_async_flag = 1;
            break;

        case USART_EVENT_RECEIVED:
            ret = csi_usart_receive_query(g_usart_handle, g_data, 15);
            csi_usart_send(g_usart_handle, g_data, ret);
            rx_trigger_flag = 1;
            break;

        default:
            break;
    }
}

static int32_t usart_wait_reply_async(usart_handle_t usart)
{
    volatile uint32_t i;
    uint8_t ch;
    char answer[20];

    for (i = 0; i < 20; i++) {
        answer[i] = '\0';
    }

    i = 0;

    while (i < 20) {
        if (0 == usart_receive_async(usart, &ch, 1)) {
            if (ch == '\n' || ch == '\r') {
                answer[i] = '\0';
                break;
            }

            if (ch == 127 || ch == '\b') {
                if (i > 0) {
                    i--;
                    usart_send_async(usart, &ch, 1);
                }
            } else {
                answer[i++] = ch;
                usart_send_async(usart, &ch, 1);
            }
        }
    }

    if ((i == 1) && (answer[0] == 'y')) {
        return 1;
    } else if ((i == 1) && (answer[0] == 'n')) {
        return 0;
    }

    return 2;
}

static int32_t usart_wait_reply(usart_handle_t usart)
{
    volatile uint32_t i;
    uint8_t ch;
    char answer[20];

    for (i = 0; i < 20; i++) {
        answer[i] = '\0';
    }

    i = 0;

    while (i < 20) {
        if (0 == usart_receive_sync(usart, &ch, 1)) {
            if (ch == '\n' || ch == '\r') {

                answer[i] = '\0';
                break;
            }

            if (ch == 127 || ch == '\b') {
                if (i > 0) {
                    i--;
                    usart_send_sync(usart, &ch, 1);
                }
            } else {
                answer[i++] = ch;
                usart_send_sync(usart, &ch, 1);
            }
        }
    }

    if ((i == 1) && (answer[0] == 'y')) {
        return 1;
    } else if ((i == 1) && (answer[0] == 'n')) {
        return 0;
    }

    return 2;
}
/*****************************************************************************
usart_test_async_mode: Tests asynchronous mode of usart

INPUT: usart - Number of usart transminting data

RETURN: NULL

*****************************************************************************/
static int32_t usart_test_async_mode(usart_handle_t usart, int32_t uart_idx)
{
    uint32_t get;
    int32_t  ret;

    ///////////////////* async mode ///////////////////////
    /* Insure prompt information is displayed */
    /* Changes usart mode to interrupt mode */
    csi_usart_uninitialize(usart);
    usart = csi_usart_initialize(uart_idx, usart_event_cb);

    if (usart == NULL) {
        return -1;
    }

    g_usart_handle = usart;
    ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

    if (ret < 0) {
        printf("csi_usart_config error %x\n", ret);
        return -1;
    }

    printf("\r\t(async mode ): Output is---\n\t\t");
    usart_send_async(usart, data, sizeof(data));
    printf("- - - [y/n] ");

    while (1) {
        /* wait for the input*/
        get = usart_wait_reply_async(usart);

        if ((get == 1) || (get == 0)) {
            break;
        } else {
            printf("\n\tPlease enter 'y' or 'n'   ");
        }
    }

    if (get == 1) {
        printf("\t- - -PASS\n");
    } else {
        printf("\t- - -FAILURE\n");
        return -1;
    }

    return 0;
}
/*****************************************************************************
usart_test_sync_mode: Tests sync mode of usart

INPUT: usart - Number of usart transminting data

RETURN: NULL

*****************************************************************************/
static int32_t usart_test_sync_mode(usart_handle_t usart)
{
    uint32_t get;

///////////////////* sync mode *//////////////////////////
    printf("\n\n\t- - - Testing usart mode...\n");
    printf("\r\t(sync mode ): Output is---\n\t\t");

    usart_send_sync(usart, data, sizeof(data));

    printf("- - - [y/n] ");

    /* wait for the input*/
    while (1) {
        get = usart_wait_reply(usart);

        if ((get == 1) || (get == 0)) {
            break;
        } else {
            printf("\n\tPlease enter 'y' or 'n'   ");
        }
    }

    if (get == 1) {
        printf("\t- - -PASS\n");
    } else {
        printf("\t- - -FAILURE\n");
        return -1;
    }

    return 0;
}

#if 0
/*****************************************************************************
usart_test_baudrate: Tests baudrate of usart

INPUT: usart - Number of usart transminting data

RETURN: NULL

*****************************************************************************/
static int32_t usart_test_baudrate(usart_handle_t usart)
{
    uint32_t baudrate;
    uint32_t get;
    int32_t ret;

    uint32_t pB[] = {9600, 19200, 14400, 38400, 57600, 115200};

    printf("\n\t- - - Test usart baudrate.\n");

    /* set the baudrate */
    for (baudrate = 0; baudrate < sizeof(pB) / 4; baudrate++) {
        printf("\tBaudrate is %d? [y] ", pB[baudrate]);

        ret = csi_usart_config(usart, pB[baudrate], USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

        if (ret < 0) {
            printf("csi_usart_config error %x\n", ret);
            return -1;
        }

        while (usart_wait_reply(usart) != 1) {
            printf("\n\tPlease enter 'y'   ");
        }

        printf(" :Output is ---\n\t\t");
        usart_send_sync(usart, data, sizeof(data));

        printf("- - -[y/n] ");

        while (1) {
            /* wait for the input*/
            get = usart_wait_reply(usart);

            if (get == 1 || get == 0) {
                break;
            } else {
                printf("\n\tPlease enter 'y' or 'n'   ");
            }
        }

        if (get == 1) {
            printf("\t- - -PASS\n");
        } else {
            printf("\t- - -FAILURE\n");
            return -1;
        }

    }

    return 0;

}
#endif

/*****************************************************************************
usart_test_parity: Tests Parity of usarts

INPUT: usart - Number of usart transminting data

RETURN: NULL

*****************************************************************************/
static int32_t usart_test_parity(usart_handle_t usart)
{
    usart_parity_e parity;
    uint32_t get;
    int32_t ret;

    printf("\n\t- - - Test usart parity. (Parity: 0 --- ODD, 1 --- EVEN, 2 --- NONE)\n");

    /* set the parity */
    for (parity = 0; parity < 5; parity++) {
        switch (parity) {
            case 0:
                printf("\tParity is %d? [y] ", parity);
                ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_ODD, USART_STOP_BITS_1, USART_DATA_BITS_8);

                if (ret < 0) {
                    printf("csi_usart_config error %x\n", ret);
                    return -1;
                }

                break;

            case 1:
                printf("\tParity is %d? [y] ", parity);
                ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_EVEN, USART_STOP_BITS_1, USART_DATA_BITS_8);

                if (ret < 0) {
                    printf("csi_usart_config error %x\n", ret);
                    return -1;
                }

                break;

            case 2:
                printf("\tParity is %d? [y] ", parity);
                ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

                if (ret < 0) {
                    printf("csi_usart_config error %x\n", ret);
                    return -1;
                }

                break;

            default:
                break;
        }

        if (parity == 3 || parity == 4) {
            continue;
        }

        /* wait for the input*/
        while (usart_wait_reply(usart) != 1) {
            printf("\n\tPlease enter 'y'   ");
        }

        printf(" :Output is ---\n\t\t");
        usart_send_sync(usart, data, sizeof(data));

        printf("- - -[y/n] ");

        while (1) {
            get = usart_wait_reply(usart);

            if (get == 1 || get == 0) {
                break;
            } else {
                printf("\n\tPlease enter 'y' or 'n'   ");
            }
        }

        if (get == 1) {
            printf("\t- - -PASS\n");
        } else {
            printf("\t- - -FAILURE\n");
            return -1;
        }

    }

    return 0;

}

/*****************************************************************************
usart_test_databits: Tests data bits of usarts.

INPUT: usart - Number of usart transminting data

RETURN: NULL

*****************************************************************************/
static int32_t usart_test_databits(usart_handle_t usart)
{
    usart_data_bits_e databits;
    uint32_t get;
    int32_t ret;

    printf("\n\t- - - Test usart databits.\n");
    printf("\t0 --- WORD_SIZE_5,\n\t1 --- WORD_SIZE_6,\n"
           "\t2 --- WORD_SIZE_7,\n\t3 --- WORD_SIZE_8\n");

    /* set the databits */
    for (databits = 2; databits < 4; databits++) {
        printf("\tWordsize is %d? [y] ", databits);

        if (databits == 2) {
            ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_7);

            if (ret < 0) {
                printf("csi_usart_config error %x\n", ret);
                return -1;
            }
        } else if (databits == 3) {
            ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

            if (ret < 0) {
                printf("csi_usart_config error %x\n", ret);
                return -1;
            }
        }

        while (usart_wait_reply(usart) != 1) {
            printf("\n\tPlease enter 'y'   ");
        }

        printf(" :Output is ---\n\t\t");
        usart_send_sync(usart, data, sizeof(data));

        printf("- - -[y/n] ");

        while (1) {
            /* wait for the input*/
            get = usart_wait_reply(usart);

            if (get == 1 || get == 0) {
                break;
            } else {
                printf("\n\tPlease enter 'y' or 'n'   ");
            }
        }

        if (get == 1) {
            printf("\t- - -PASS\n");
        } else {
            printf("\t- - -FAILURE\n");
            return -1;
        }

    }

    return 0;
}

static int32_t usart_test_stopbits(usart_handle_t usart)
{
    usart_stop_bits_e stopbits;
    uint32_t get;
    int32_t ret;

    printf("\n\t- - - Test usart stopbits.\n");
    printf("\t0 --- USART_STOP_BITS_1,\n\t1 --- USART_STOP_BITS_2,\n");

    /* set the databits */
    for (stopbits = 0; stopbits < 2; stopbits++) {
        printf("\tstopbits is %d? [y] ", stopbits);

        if (stopbits == 0) {
            ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

            if (ret < 0) {
                printf("csi_usart_config error %x\n", ret);
                return -1;
            }

        } else if (stopbits == 1) {
            ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_2, USART_DATA_BITS_8);

            if (ret < 0) {
                printf("csi_usart_config error %x\n", ret);
                return -1;
            }
        }

        while (usart_wait_reply(usart) != 1) {
            printf("\n\tPlease enter 'y'   ");
        }

        printf(" :Output is ---\n\t\t");
        usart_send_sync(usart, data, sizeof(data));

        printf("- - -[y/n] ");

        while (1) {
            /* wait for the input*/
            get = usart_wait_reply(usart);

            if (get == 1 || get == 0) {
                break;
            } else {
                printf("\n\tPlease enter 'y' or 'n'   ");
            }
        }

        if (get == 1) {
            printf("\t- - -PASS\n");
        } else {
            printf("\t- - -FAILURE\n");
            return -1;
        }

    }

    return 0;
}

static int32_t usart_test_trigger_event(usart_handle_t usart, int32_t uart_idx)
{
    int32_t ret;

    printf("\r\t waitting uart  trigger event---\n\t\t");
    /* Insure prompt information is displayed */
    /* Changes usart mode to interrupt mode */
    csi_usart_uninitialize(usart);
    usart = csi_usart_initialize(uart_idx, usart_event_cb_query);

    if (usart == NULL) {
        return -1;
    }

    g_usart_handle = usart;

    ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

    if (ret != 0) {
        return -1;
    }

    rx_async_flag = 0;
    tx_async_flag = 0;
    rx_trigger_flag = 0;

    while (rx_trigger_flag == 0);

    while (tx_async_flag == 0);

    printf("----- usart trigger PASS\n");

    ret = csi_usart_uninitialize(usart);

    if (ret < 0) {
        printf("csi_usart_uninitialize error %x\n", ret);
        return -1;
    }

    return 0;
}

int _usart_test(usart_handle_t usart, int32_t uart_idx)
{
    uint32_t get;
    //char input_char1,input_char2;
    int32_t ret;

    printf("Testing usart...");
    printf("Default configure: Baudrate --- 115200,");
    printf("Parity --- NONE,");
    printf(" Wordsize --- 8. ");
    printf("\n- - -USART ready? [y] \n");

    while (1) {
        /* wait for input*/
        get = usart_wait_reply(usart);

        if ((get == 1)) {
            break;
        } else {
            printf("\nPlease enter 'y'");
        }
    }

    ret = usart_test_sync_mode(usart);

    if (ret < 0) {
        printf("usart_test_sync_mode error %x\n", ret);
        return -1;
    }

    /*
     * test usart Baudrate, Parity and WordSize are tested in
     * interrupt mode here */
    ret = usart_test_parity(usart);

    if (ret < 0) {
        printf("usart_test_parity error %x\n", ret);
        return -1;
    }

    ret = usart_test_databits(usart);

    if (ret < 0) {
        printf("usart_test_databits error %x\n", ret);
        return -1;
    }

    ret = usart_test_stopbits(usart);

    if (ret < 0) {
        printf("usart_test_stopbits error %x\n", ret);
        return -1;
    }

    /* test async mode */
    ret = usart_test_async_mode(usart, uart_idx);

    if (ret < 0) {
        printf("usart_test_async_mode error %x\n", ret);
        return -1;
    }

    ret = usart_test_trigger_event(usart, uart_idx);

    if (ret < 0) {
        printf("usart_test_trigger error %x\n", ret);
        return -1;
    }

    return 0;
}

void pinmap_usart_init(int32_t uart_idx)
{
    for(int i = 0; i < sizeof(g_usart_pinmap) / sizeof(usart_pinmap_t); i++) {
        if(g_usart_pinmap[i].cfg_idx == uart_idx) {
            drv_pinmux_config(g_usart_pinmap[uart_idx].tx, g_usart_pinmap[uart_idx].tx_func);
            drv_pinmux_config(g_usart_pinmap[uart_idx].rx, g_usart_pinmap[uart_idx].rx_func);
            break;
        }
    }
}


static int32_t usart_test_mode(int32_t uart_idx)
{
    usart_handle_t usart;
    int32_t  ret;
    int i = 0;
    uint8_t answer[11]= {'\0'};

    pinmap_usart_init(uart_idx);

    usart = csi_usart_initialize(uart_idx, usart_event_cb);

    if (usart == NULL) {
        return -1;
    }

    ret = csi_usart_config(usart, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);

    if (ret < 0) {
        printf("csi_usart_config error %x\n", ret);
        return -1;
    }

    ret = usart_send_async(usart, data, sizeof(data));
    if(ret != 0){
        printf("\r\tusart_send_async error---\n\t\t");
        return -1;
    }
    printf("usart_send_async data= %s\n",data);


    uint8_t ch;
    while(i < 10) {
            if (0 == usart_receive_async(usart, &ch, 1)) {
                if (ch == '\n' || ch == '\r') {
                    answer[i] = '\0';
                    break;
                }
                answer[i++] = ch;
                }
            }
    printf("usart_receive_async data = %s\n", answer);
    return 0;
}

static void cmd_usart_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;
    usart_test_mode(atoi(argv[1]));
}

void test_usart_cmd(void)
{
    static const struct cli_command cmd_info = {"usart", "usar idx", cmd_usart_func};

    aos_cli_register_command(&cmd_info);
}

