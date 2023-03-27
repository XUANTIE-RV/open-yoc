#include <drv/uart.h>
#include <stdio.h>
#include <aos/cli.h>
#include <drv/pin.h>
#include <posix/timer.h>
#include <unistd.h>
#include <pthread.h>

typedef enum {
    TEST_UART_NONE  = 0,
    TEST_UART_SET,                 //set uart param
    TEST_UART_SEND,                //tx str
    TEST_UART_RECEIVE,             //rx str
    TEST_UART_CIRCLE,            //tx --> rx
} test_uart_mode_t;

static csi_uart_t  g_uart_hd[4];
static pthread_t Rx_task;

static int send_str(csi_uart_t *uart,const char *buf,int size)
{
    int ret = 0;

    ret = csi_uart_send(uart, buf, size,100);
    ret = csi_uart_send(uart, "\n", 1, 1);
    if (ret == -1) {
        aos_cli_printf("csi_uart_send fail\r\n");
    }
    return ret;
}

//timeout_ms  -1 :block
static int receive_str(csi_uart_t *uart,char *buf,int size,int timeout_ms)
{
    int ret = 0;
    unsigned int to_ms = 10;
    unsigned int timeout_count = 0;
    while (1) {
        ret = csi_uart_receive(uart, buf, size, to_ms);
        if (ret == -1) {
            aos_cli_printf("csi_uart_receive fail\r\n");
            break;
        } else if (ret > 0) {
            break;
        }
        usleep(1*1000);
        if(timeout_ms == -1)    //block read
            continue;

        if (++timeout_count > (timeout_ms/to_ms)) {
            aos_cli_printf("receive timeout %d ms\r\n",timeout_ms);
            break;
        }
    }
    return ret;
}

static void* rx_str_proc(void *pArgs)
{
    csi_uart_t *puart = (csi_uart_t *)pArgs;
    char buf[256] = {0,};

    if (0 >= receive_str(puart,buf,256,2000)) {    //2s block
        aos_cli_printf("receive_str fail\r\n");
        return (void *)NULL;
    }
    aos_cli_printf("receive: %s\r\n",buf);
    //aos_cli_printf("exit rx_str_proc\r\n");
    return (void *)NULL;
}

static int send_receive_str(csi_uart_t *uart,const char *buf)
{
    int ret = 0;

	if (pthread_create(&Rx_task, NULL, rx_str_proc, (void*)uart)) {
		aos_cli_printf(" recv str rx_str_proc failed\r\n");
		return -1;
	}
    usleep(200*1000);
    ret = send_str(uart,buf,strlen(buf));
    if (ret == -1) {
        aos_cli_printf(" send_str failed\r\n");
    }

    if(Rx_task != 0) {
        pthread_join(Rx_task,NULL);
        Rx_task = 0;
    }
    return ret;
}

static csi_uart_data_bits_t trans_data_bit(int data_bits)
{
    csi_uart_data_bits_t enDataBit;

    if(data_bits == 5)
        enDataBit = UART_DATA_BITS_5;
    else if (data_bits == 6)
        enDataBit = UART_DATA_BITS_6;
    else if (data_bits == 7)
        enDataBit = UART_DATA_BITS_7;
    else if (data_bits == 8)
        enDataBit = UART_DATA_BITS_8;
    else {
        aos_cli_printf("not support data bits:[%d],user default 8bit\r\n",data_bits);
        enDataBit = UART_DATA_BITS_8;
    }

    return enDataBit;
}

static csi_uart_data_bits_t trans_stop_bit(float stop_bits)
{
    csi_uart_stop_bits_t enStopBit;

    if(stop_bits == 1.0f)
        enStopBit = UART_STOP_BITS_1;
    else if (stop_bits == 2.0f)
        enStopBit = UART_STOP_BITS_2;
    else if (stop_bits == 1.5f)
        enStopBit = UART_STOP_BITS_1_5;
    else {
        aos_cli_printf("not support stop bits:[%0.1f],user default 1bit\r\n",stop_bits);
        enStopBit = UART_STOP_BITS_1;
    }
    return enStopBit;
}


void print_usage_uart(void)
{
    printf("\r\n");
    printf("\t testuart [uart_num] set [baud] [data_bits] [stop_bits] [parity] [flowctrl]\r\n");
    printf("\t su: testuart 1 set 115200 8 1 0 0 \r\n");
    printf("\t su: testuart 1 set 115200 \r\n");

    printf("\t testuart [uart_num] tx  [str] \r\n");
    printf("\t testuart [uart_num] rx  [size]\r\n");
    printf("\t testuart [uart_num] cc  [str] \r\n");
}

void test_uart(int32_t argc, char **argv)
{
	/* test uart
	 * for example:
	 * testuart num(0~3) baud data_bits stop_bits parity flowctrl
	 */

    csi_error_t ret_status;
    int num,baud,flowctrl,data_bits;
    float stop_bits;
    csi_uart_parity_t parity;
    csi_uart_data_bits_t en_data_bits;
    csi_uart_stop_bits_t en_stop_bits;
    test_uart_mode_t enMode = TEST_UART_NONE;

	if (argc < 3) {
        printf("please param less than 3: \r\n");
        print_usage_uart();
		return;
	}

    num = atoi(argv[1]);
    if ((num > 4) || (num <= 0)) {
        aos_cli_printf("error: num[%d] invaild,must(0~4) \r\n", num);
        return ;
    }

    if (strcmp(argv[2], "set") == 0) {
		enMode = TEST_UART_SET;
    } else if (strcmp(argv[2], "tx") == 0) {
        enMode = TEST_UART_SEND;
    } else if (strcmp(argv[2], "rx") == 0) {
        enMode = TEST_UART_RECEIVE;
    } else if (strcmp(argv[2], "cc") == 0) {
        enMode = TEST_UART_CIRCLE;
    } else {
        aos_cli_printf("invaild param: %s\r\n",argv[2]);
        return ;
    }

    switch (enMode) {
        case TEST_UART_SET: {
            baud      = argv[3] == NULL ? 115200 : atoi(argv[3]);
            data_bits = argv[4] == NULL ? 8      : atoi(argv[4]);
            stop_bits = argv[5] == NULL ? 1      : atof(argv[5]);
            parity    = argv[6] == NULL ? 0      : atoi(argv[6]);
            flowctrl  = argv[7] == NULL ? 0      : atoi(argv[7]);
 
            aos_cli_printf("data_bits:[%d],stop_bits:[%0.1f],parity:[%d],flowctrl:[%d] \r\n", data_bits, stop_bits,parity,flowctrl);
            en_data_bits = trans_data_bit(data_bits);
            en_stop_bits = trans_stop_bit(stop_bits);
            //aos_cli_printf("en_data_bits:[%d],en_stop_bits:[%d] \r\n", en_data_bits, en_stop_bits);

            ret_status = csi_uart_init(&g_uart_hd[num], num);
            if (ret_status == -1) {
                aos_cli_printf("csi_uart_init[%d] fail\r\n", num);
                return ;
            }
            ret_status = csi_uart_baud(&g_uart_hd[num], baud);
            if (ret_status == -1) {
                aos_cli_printf("csi_uart_baud[%d] fail\r\n", num);
            }
            ret_status = csi_uart_format(&g_uart_hd[num], en_data_bits, parity, en_stop_bits);
            if (ret_status == -1) {
                aos_cli_printf("csi_uart_format[%d] fail\r\n", num);
            }
            ret_status = csi_uart_flowctrl(&g_uart_hd[num], flowctrl);
            if (ret_status == -1) {
                aos_cli_printf("csi_uart_flowctrl[%d] fail\r\n", num);
            }
            aos_cli_printf("csi_uart_init[%d] success\r\n", num);
        }
        break;

        case TEST_UART_SEND: {
            if (argv[3] == NULL) {
                aos_cli_printf("error :send buf(argv[3]) is NULL\r\n");
                return ;
            }
            send_str(&g_uart_hd[num],argv[3],strlen(argv[3]));
        }
        break;

        case TEST_UART_RECEIVE: {
            char buf[256] = {0,};
            int size = argv[3] == NULL ? 100 : atoi(argv[3]);
            receive_str(&g_uart_hd[num],buf,size,-1);
            aos_cli_printf("receive: %s \r\n",buf);
        }
        break;

        case TEST_UART_CIRCLE: {
            if (argv[3] == NULL) {
                aos_cli_printf("error :send buf(argv[3]) is NULL\r\n");
                return ;
            }
            send_receive_str(&g_uart_hd[num],argv[3]);
        }
        break;

        default :
            break;
    }
}

ALIOS_CLI_CMD_REGISTER(test_uart, testuart, test uart function);