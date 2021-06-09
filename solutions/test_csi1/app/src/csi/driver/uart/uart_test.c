#include <uart_test.h>


test_func_info_t uart_test_funcs_map[] = {
	{"UART_ASYNC_SEND",test_uart_syncSend,7},
	{"UART_ASYNC_RECEIVE",test_uart_syncReceive,7},
	{"UART_ASYNC_SENDCHAR",test_uart_async_send_char,7},
	{"UART_ASYNC_RECVCHAR",test_uart_async_receive_char,7},
	{"UART_INTERFACE",test_uart_interface,1},
	{"UART_GETCHAR",test_uart_getChar,6},
	{"UART_PUTCHAR",test_uart_putChar,6},
//	{"UART_TX_COUNT",test_uart_txcount,7},
//	{"UART_RX_COUNT",test_uart_rxcount,7}
};


int test_uart_main(char *args)
{
	uint8_t i;
	void *args_value = NULL;
	int ret;

	for(i=0;i<sizeof(uart_test_funcs_map)/sizeof(test_func_info_t);i++){
		if(!strcmp((void *)_mc_name, uart_test_funcs_map[i].name)){
			args_value = malloc(sizeof(uint32_t)*uart_test_funcs_map[i].args_num);
			if (args_value == NULL){
				TEST_CASE_WARN_QUIT("malloc space failed, unparsed parameter");
			}
			ret = args_parsing(args, (uint32_t *)args_value, uart_test_funcs_map[i].args_num);
			if (ret != 0){
				free(args_value);
				TEST_CASE_WARN_QUIT("parameter resolution error");
			}
			(*(uart_test_funcs_map[i].function))(args_value);
			free(args_value);
			return 0;
		}
	}

	TEST_CASE_TIPS("UART module don't support this command.");
	return -1;
}
