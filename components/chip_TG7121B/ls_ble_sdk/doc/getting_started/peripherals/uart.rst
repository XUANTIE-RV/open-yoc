.. _uart_ref:

UART
======

UART（Universal Asynchronous Receiver/Transmitter）通用异步收发传输器，UART 作为异步串口通信协议的一种，工作原理是将传输数据的每个字符一位接一位地传输。是在应用程序开发过程中使用频率最高的数据总线。

初始化
-------

#. 为UART模块配置IO

    调用IO 的初始化接口，可以将任意IO配置UART的TX或者RX，与其他设备进行通信。

.. code ::

    void uart1_io_init(uint8_t txd,uint8_t rxd);
    void uart2_io_init(uint8_t txd,uint8_t rxd);
    void uart3_io_init(uint8_t txd,uint8_t rxd);

.. note ::

    芯片的IO 一共有34个，具体情况需根据封装图来定义。
    | 为了避免不必要的bug，在使用UART通信的时候，请先初始化IO，再进行下诉参数的配置。

#. 设置UART模块参数变量

    设置UART模块的参数变量，其结构体的参数原型如下：

.. code ::

    typedef struct
    {
        app_uart_baudrate_t BaudRate;                  /*!< This member configures the UART communication baud rate.*/

        uint8_t     WordLength:2,                /*!< Specifies the number of data bits transmitted or received in a frame.
                                                      This parameter can be a value of @ref UART_Word_Length */

                    StopBits:1,                  /*!< Specifies the number of stop bits transmitted.
                                               This parameter can be a value of @ref UART_Stop_Bits */

                    Parity:2,                    /*!< Specifies the parity mode.
                                               This parameter can be a value of @ref UART_Parity
                                               @note When parity is enabled, the computed parity is inserted
                                                     at the MSB position of the transmitted data (9th bit when
                                                     the word length is set to 9 data bits; 8th bit when the
                                                     word length is set to 8 data bits). */
                    MSBEN:1,
                    HwFlowCtl:1;                 /*!< Specifies whether the hardware flow control mode is enabled or disabled.
                                                     This parameter can be a value of @ref UART_Hardware_Flow_Control */
        uint8_t     Tx_DMA: 1,                   /**< Default DMA Setting for TX. */
                    Rx_DMA: 1;                   /**< Default DMA Setting for RX . */
    } UART_InitTypeDef;

提供的配置参数可取值的为如下宏定义：

.. code ::

    //UART communication baud rate.
    typedef enum
    {
        UART_BAUDRATE_1200   = UART_BUADRATE_ENUM_GEN(1200),
        UART_BAUDRATE_2400   = UART_BUADRATE_ENUM_GEN(2400),
        UART_BAUDRATE_4800   = UART_BUADRATE_ENUM_GEN(4800),
        UART_BAUDRATE_9600   = UART_BUADRATE_ENUM_GEN(9600),
        UART_BAUDRATE_14400 = UART_BUADRATE_ENUM_GEN(14400),
        UART_BAUDRATE_19200 = UART_BUADRATE_ENUM_GEN(19200),
        UART_BAUDRATE_28800 = UART_BUADRATE_ENUM_GEN(28800),
        UART_BAUDRATE_38400  = UART_BUADRATE_ENUM_GEN(38400),
        UART_BAUDRATE_57600 = UART_BUADRATE_ENUM_GEN(57600),
        UART_BAUDRATE_76800  = UART_BUADRATE_ENUM_GEN(76800),
        UART_BAUDRATE_115200 = UART_BUADRATE_ENUM_GEN(115200),
        UART_BAUDRATE_230400 = UART_BUADRATE_ENUM_GEN(230400),
        UART_BAUDRATE_250000 = UART_BUADRATE_ENUM_GEN(250000),
        UART_BAUDRATE_500000 = UART_BUADRATE_ENUM_GEN(500000),
        UART_BAUDRATE_460800 = UART_BUADRATE_ENUM_GEN(460800),
        UART_BAUDRATE_750000=  UART_BUADRATE_ENUM_GEN(750000),
        UART_BAUDRATE_921600 = UART_BUADRATE_ENUM_GEN(921600),
        UART_BAUDRATE_1000000= UART_BUADRATE_ENUM_GEN(1000000),
        UART_BAUDRATE_2000000= UART_BUADRATE_ENUM_GEN(2000000),
    }app_uart_baudrate_t;
    //Parity
    #define UART_NOPARITY       0x0     // Parity diable
    #define UART_ODDPARITY      0x1     // Parity Odd
    #define UART_EVENPARITY     0x3     // Parity Even
    //UART_BYTESIZE
    #define UART_BYTESIZE5      0X0     // Byte size 5 bits
    #define UART_BYTESIZE6      0X1     // Byte size 6 bits
    #define UART_BYTESIZE7      0X2     // Byte size 7 bits
    #define UART_BYTESIZE8      0X3     // Byte size 8 bits
    //UART_STOPBITS
    #define UART_STOPBITS1      0x0     // Stop 1 bits
    #define UART_STOPBITS2      0x1     // Stop 2 bits
    //UART_BIT_ORDER
    #define UART_LSB      0x0     // LSBEN
    #define UART_MSB      0x1     // MSBEN

#. 初始化UART模块

    通过初始化接口，应用程序可以对串口设备进行参数配置。

.. code ::

    HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);

反初始化
---------

#. 反初始化UART模块

    通过反初始化接口，应用程序可以关闭UART 外设，从而在运行BLE的程序的时候，降低系统的功耗。

.. code ::

    HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);

#. 反初始化UART IO

    反初始化IO接口的主要目的是为了避免在进入低功耗模式时，IO上产生漏电，或者给对接设备发送不必要的数据。
    |调用此接口后,会默认的将UART的TX IO 配置成高电平（但是无输出能力），RX配置成浮空输入模式。

.. code ::

    void uart1_io_deinit(void);
    void uart2_io_deinit(void);
    void uart3_io_deinit(void);

.. note ::

    UART初始化动作会向系统注册UART进入工作状态，当系统检测到有任一外设处于工作状态时，都不会进入低功耗休眠。
    因此，UART使用完毕，需要进入低功耗状态之前，必须反初始化UART。

串口数据接收和发送数据的模式分为 3 种：非阻塞（中断）模式、阻塞模式、DMA 模式。在使用的时候，这 3 种模式只能选其一，若串口的打开参数 oflags 没有指定使用中断模式或者 DMA 模式，则默认使用轮询模式。

数据收发——非阻塞（中断）方式
-----------------------------

以非阻塞（中断）方式接收发送模式使用串口设备的接口如下所示：

.. code ::

    HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, void *tx_arg);
    HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size,  void *rx_arg);


数据收发——阻塞方式
---------------------------

以非阻塞（中断）方式接收发送模式使用串口设备的接口如下所示：

.. code ::

    HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size,uint32_t Timeout);


初始化以及非阻塞（中断）模式收发的示例如下：

.. code ::

    #include "io_config.h"
    #include "lsuart.h"
    #define TEST_ZONE_SIZE 512
    uint8_t uart_rx_buf[TEST_ZONE_SIZE * 2] ;
    uint8_t uart_tx_buf[TEST_ZONE_SIZE * 2] ;

    UART_HandleTypeDef UART_Config; 

    // UART Transmit complete callback 
    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart,void *tx_arg)
    {
        //UART disable
        // HAL_UART_DeInit(huart);
        // uart1_io_deinit();
        /*code */
    
        /* user code end */
    
    }
    //UART Receive Complete Callback
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart,void *rx_arg)
    {
         //UART disable
        // HAL_UART_DeInit(huart);
        // uart1_io_deinit();
        /*code */
    
        /* user code end */
    }

    void uart1_init()
    {
        uart1_io_init(PB00,PB01);                                       // init step 1
        UART_Config.UARTX = UART1;
        UART_Config.Init.BaudRate = UART_BAUDRATE_115200;
        UART_Config.Init.MSBEN = 0;
        UART_Config.Init.Parity = UART_NOPARITY;
        UART_Config.Init.StopBits = UART_STOPBITS1;
        UART_Config.Init.WordLength = UART_BYTESIZE8;                  // init step 2
        HAL_UART_Init(&UART_Config);                                   // init step 3
    }

    void uart1_deinit()
    {
        HAL_UART_DeInit(&UART_Config);                                // deinit step 1
        uart1_io_deinit();                                            // deinit step 2
    }

    static void uart_test()
    {
        HAL_UART_Transmit_IT(&UART_Config,uart_tx_buf,1,NULL);
        HAL_UART_Receive_IT(&UART_Config,uart_rx_buf,1,NULL);
    }

    int main()
    {
        uart1_init();
        uart1_test();
        while(1)
        {
        }
    }