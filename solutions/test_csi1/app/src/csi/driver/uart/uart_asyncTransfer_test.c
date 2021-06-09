
#include <uart_test.h>

volatile static uint8_t receive_block = 0;

static void uart_callback(int32_t idx, usart_event_e event)
{
    if (event == USART_EVENT_RECEIVE_COMPLETE) {
        receive_block = 0;
    }
}



int test_uart_syncSend(void *args)
{
	usart_handle_t hd;
	test_uart_args_t td;
	int32_t ret;
    uint8_t i=0;
	uint8_t send_data_num;
	usart_status_t ret_status;

	td.uart_idx = (uint8_t)*((uint32_t *)args);
	td.baudrate = *((uint32_t *)args+1);
	td.data_bits = (uint8_t)*((uint32_t *)args+2);
	td.parity = (uint8_t)*((uint32_t *)args+3);
	td.stop_bits = (uint8_t)*((uint32_t *)args+4);
	td.uart_mode = (uint8_t)*((uint32_t *)args+5);
	td.uart_transfer_len = (uint32_t)*((uint32_t *)args+6);

	hd = csi_usart_initialize(td.uart_idx, NULL);
	TEST_CASE_ASSERT_QUIT(hd != NULL, "uart %d init fail", td.uart_idx);

	ret = csi_usart_config_baudrate(hd, td.baudrate);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config baudrate fail", td.uart_idx);

	ret = csi_usart_config_mode(hd, td.uart_mode);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config mode fail", td.uart_idx);

	ret = csi_usart_config_parity(hd, td.parity);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config parity fail", td.uart_idx);

	ret = csi_usart_config_stopbits(hd, td.stop_bits);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config stop bits fail", td.uart_idx);

	ret = csi_usart_config_databits(hd, td.data_bits);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config data bits fail", td.uart_idx);


	ret = csi_usart_set_interrupt(hd, USART_INTR_WRITE, 0);
	TEST_CASE_ASSERT(ret == 0, "uart %d config write interrupt fail", td.uart_idx);

	ret = csi_usart_set_interrupt(hd, USART_INTR_READ, 0);
    TEST_CASE_ASSERT(ret == 0, "uart %d config write interrupt fail", td.uart_idx);


	ret_status = csi_usart_get_status(hd);
	if ((ret_status.tx_busy != 0) || (ret_status.rx_busy != 0)) {
		TEST_CASE_ASSERT(1 == 0, "uart %d call get state fail", td.uart_idx);
	}

	char *send_data = NULL;
    send_data = (char *)malloc(td.uart_transfer_len);
	
    if (send_data == NULL) {
		free(send_data);
        csi_usart_uninitialize(hd);
        TEST_CASE_WARN_QUIT("transfer size too long, malloc error");
    }
	transfer_data(send_data, td.uart_transfer_len);

	TEST_CASE_READY();
	
	ret = csi_usart_flush(hd,USART_FLUSH_WRITE);
    TEST_CASE_ASSERT(ret == 0, "uart %d flush send data fail", td.uart_idx);

	ret = csi_usart_send(hd, send_data, td.uart_transfer_len);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d call sync send fail", td.uart_idx);

	do {
		ret_status = csi_usart_get_status(hd);
	}while(ret_status.tx_busy != 0);

	free(send_data);
	csi_usart_uninitialize(hd);
	return 0;
}


int test_uart_syncReceive(void *args)
{
	usart_handle_t hd;
    test_uart_args_t td;
    int32_t ret;
	uint8_t recv_data_num;
	char *send_data = NULL;
	char *receive_data = NULL;
    usart_status_t ret_status;

    td.uart_idx = (uint8_t)*((uint32_t *)args);
    td.baudrate = *((uint32_t *)args+1);
    td.data_bits = (uint8_t)*((uint32_t *)args+2);
    td.parity = (uint8_t)*((uint32_t *)args+3);
    td.stop_bits = (uint8_t)*((uint32_t *)args+4);
	td.uart_mode = (uint8_t)*((uint32_t *)args+5);
	td.uart_transfer_len = (uint8_t)*((uint32_t *)args+6);

//    send_data = (char *)malloc(td.uart_transfer_len);
	receive_data = (char *)malloc(td.uart_transfer_len);
    if ((send_data == NULL) || (receive_data == NULL)) {
		free(send_data);
		free(receive_data);
        csi_usart_uninitialize(hd);
        TEST_CASE_WARN_QUIT("transfer size too long, malloc error");
    }
	transfer_data(send_data, td.uart_transfer_len);	
    
    hd = csi_usart_initialize(td.uart_idx, uart_callback);
    TEST_CASE_ASSERT_QUIT(hd != NULL, "uart %d init fail", td.uart_idx);

    ret = csi_usart_config_baudrate(hd, td.baudrate);
    TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config baudrate fail", td.uart_idx);


    ret = csi_usart_config_mode(hd, td.uart_mode);
    TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config mode fail", td.uart_idx);


    ret = csi_usart_config_parity(hd, td.parity);
    TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config parity fail", td.uart_idx);

    ret = csi_usart_config_stopbits(hd, td.stop_bits);
    TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config stop bits fail", td.uart_idx);

    ret = csi_usart_config_databits(hd, td.data_bits);
     TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d config data bits fail", td.uart_idx);

    ret = csi_usart_set_interrupt(hd, USART_INTR_WRITE, 0);
    TEST_CASE_ASSERT(ret == 0, "uart %d config write interrupt fail", td.uart_idx);

	ret = csi_usart_set_interrupt(hd, USART_INTR_READ, 1);
    TEST_CASE_ASSERT(ret == 0, "uart %d config write interrupt fail", td.uart_idx);


    ret_status = csi_usart_get_status(hd);
    if ((ret_status.tx_busy != 0) || (ret_status.rx_busy != 0)) {
        TEST_CASE_ASSERT(1 == 0, "uart %d call get state fail", td.uart_idx);
    }

	TEST_CASE_READY();
	receive_block = 1;
	
	ret = csi_usart_flush(hd,USART_FLUSH_READ);
    TEST_CASE_ASSERT(ret == 0, "uart %d flush receive data fail", td.uart_idx);

	ret = csi_usart_receive(hd, receive_data, td.uart_transfer_len);
	TEST_CASE_ASSERT_QUIT(ret == 0, "uart %d call sync receive fail", td.uart_idx);

    TEST_CASE_TIPS("recv_data is %s\n",receive_data);
    TEST_CASE_TIPS("send_data is %s\n",send_data);

	while(receive_block == 1) ;

	ret = memcmp(send_data,receive_data,td.uart_transfer_len);
	TEST_CASE_ASSERT(ret == 0,"uart %d  transfer error,data_send is not equal to data_receive\n");

	if(receive_data == NULL){
		TEST_CASE_ASSERT(1 == 0, "uart %d  transfer error,data_send is not equal to data_receive", td.uart_idx);
	}

//	free(send_data);
	free(receive_data);
	csi_usart_uninitialize(hd);
	return 0;
}
