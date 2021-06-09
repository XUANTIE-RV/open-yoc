#include <uart_test.h>

/*
void uart_event_cb()
{
    TEST_CASE_TIPS("enter uart cb");
}
*/
int test_uart_interface(void *args)
{	
    usart_handle_t hd;
//	test_uart_args_t td;
    int32_t ret;
	usart_status_t ret_status;

    //get_capabilities
//    usart_capabilities_t cap;
//    cap = csi_usart_get_capabilities(2);

    hd = csi_usart_initialize(1,NULL);
    TEST_CASE_ASSERT(hd != NULL,"hd == NULL act_val=%d",hd);

//    hd = csi_usart_initialize(0,NULL);
//    TEST_CASE_ASSERT(hd != NULL,"hd == NULL act_val=%d",hd);
   
    // power control
    ret = csi_usart_power_control(NULL, DRV_POWER_FULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    
//    ret = csi_usart_power_control(hd, 5);
//    TEST_CASE_ASSERT(ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    //uninitialize
    ret = csi_usart_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

//配置
    ret = csi_usart_config_flowctrl(NULL, USART_FLOWCTRL_CTS_RTS );//配置 usart 实例的流控。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    
    ret = csi_usart_config_clock(NULL, USART_CPOL0 , USART_CPHA0);//配置 usart 实例的通信极性与相位。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
        
    ret = csi_usart_control_break(NULL, 1 );//控制 usart 实例的 break 帧发送。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    

	ret = csi_usart_config_baudrate(NULL, 115200);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_usart_config_mode(NULL, USART_MODE_ASYNCHRONOUS);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_usart_config_parity(NULL, 0);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_usart_config_stopbits(NULL, 1);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_usart_config_databits(NULL, 8);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_set_interrupt(NULL, USART_INTR_WRITE, 0);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_usart_set_interrupt(NULL, USART_INTR_READ, 0);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    char *source ="agc";
    char ch='a';

    ret = csi_usart_control_tx(NULL, 1);//控制 usart 实例的发送使能。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_send(NULL, source, 3);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_abort_send(NULL);//数据发送终止
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_putchar( NULL, ch);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
    
    ret = csi_usart_get_tx_count(NULL);//获取设备实例上次已发送的数据个数。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_control_break(NULL, 0 );//控制 usart 实例的 break 帧发送。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_set_interrupt(NULL, USART_INTR_WRITE, 0);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

	ret = csi_usart_set_interrupt(NULL, USART_INTR_READ, 0);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret_status = csi_usart_get_status(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_flush(NULL, USART_FLUSH_WRITE);//清楚数据缓存
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_control_rx(NULL, 1);//接收使能
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    char receive_buffer[32];

    ret = csi_usart_transfer( NULL, receive_buffer, source, 3);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_abort_transfer(NULL);//usart 启动数据传输，注意是同步传输
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_receive(NULL, receive_buffer, sizeof(receive_buffer));
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_receive_query( NULL, receive_buffer, sizeof(receive_buffer));//查询方式从 USART 读取一定量数据。
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_abort_receive(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_get_rx_count(NULL);//获取设备实例上一次已接收的数据个数
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);
  
    ret_status = csi_usart_get_status(NULL);
    TEST_CASE_ASSERT((ret_status.rx_busy == 0 ),"ret_status.rx_busy != 0"); 

    ret = csi_usart_uninitialize(NULL);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_USART_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_usart_uninitialize(hd);
    return 0;
}
