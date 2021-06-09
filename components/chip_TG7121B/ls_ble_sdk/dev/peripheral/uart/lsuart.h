#ifndef LSUART_H_
#define LSUART_H_
#include <stdbool.h>
#include "reg_uart.h"
#include "HAL_def.h"
#include "sdk_config.h"

#if FPGA 
#define UART_CLOCK   16000000
#else 
#define UART_CLOCK   (SDK_PCLK_MHZ*1000000)

#endif 

#define UART_BUADRATE_ENUM_GEN(BAUD)  ((((UART_CLOCK<<4)/BAUD) +8)>>4)

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


/** @defgroup UART_Exported_Types UART Exported Types
  * @{
  */

/**
  * @brief UART Init Structure definition
  */
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
    uint8_t    Tx_DMA: 1,                   /**< Default DMA Setting for TX. */
               Rx_DMA: 1;                   /**< Default DMA Setting for RX . */
} UART_InitTypeDef;

/**
  * @brief HAL UART State structures definition
  * @note  HAL UART State value is a combination of 2 different substates: gState and RxState.
  *        - gState contains UART state information related to global Handle management
  *          and also information related to Tx operations.
  *          gState value coding follow below described bitmap :
  *          b7-b6  Error information
  *             00 : No Error
  *             01 : (Not Used)
  *             10 : Timeout
  *             11 : Error
  *          b5     Peripheral initialization status
  *             0  : Reset (Peripheral not initialized)
  *             1  : Init done (Peripheral not initialized. HAL UART Init function already called)
  *          b4-b3  (not used)
  *             xx : Should be set to 00
  *          b2     Intrinsic process state
  *             0  : Ready
  *             1  : Busy (Peripheral busy with some configuration or internal operations)
  *          b1     (not used)
  *             x  : Should be set to 0
  *          b0     Tx state
  *             0  : Ready (no Tx operation ongoing)
  *             1  : Busy (Tx operation ongoing)
  *        - RxState contains information related to Rx operations.
  *          RxState value coding follow below described bitmap :
  *          b7-b6  (not used)
  *             xx : Should be set to 00
  *          b5     Peripheral initialization status
  *             0  : Reset (Peripheral not initialized)
  *             1  : Init done (Peripheral not initialized)
  *          b4-b2  (not used)
  *            xxx : Should be set to 000
  *          b1     Rx state
  *             0  : Ready (no Rx operation ongoing)
  *             1  : Busy (Rx operation ongoing)
  *          b0     (not used)
  *             x  : Should be set to 0.
  */
typedef enum
{
    HAL_UART_STATE_RESET             = 0x00U,    /*!< Peripheral is not yet Initialized
                                                    Value is allowed for gState and RxState */
    HAL_UART_STATE_READY             = 0x20U,    /*!< Peripheral Initialized and ready for use
                                                    Value is allowed for gState and RxState */
    HAL_UART_STATE_BUSY              = 0x24U,    /*!< an internal process is ongoing
                                                    // Value is allowed for gState only */
    HAL_UART_STATE_BUSY_TX           = 0x21U,    /*!< Data Transmission process is ongoing
                                                    Value is allowed for gState only */
    HAL_UART_STATE_BUSY_RX           = 0x22U,    /*!< Data Reception process is ongoing
                                                    Value is allowed for RxState only */
    HAL_UART_STATE_BUSY_TX_RX        = 0x23U,    /*!< Data Transmission and Reception process is ongoing
                                                    Not to be used for neither gState nor RxState.
                                                    Value is result of combination (Or) between gState and RxState values */
    HAL_UART_STATE_TIMEOUT           = 0xA0U,    /*!< Timeout state
                                                    Value is allowed for gState only */
    HAL_UART_STATE_ERROR             = 0xE0U     /*!< Error
                                                    Value is allowed for gState only */
} HAL_UART_StateTypeDef;


/**
  * @brief  UART handle Structure definition
  */
typedef struct __UART_HandleTypeDef
{
    reg_uart_t                    *UARTX;           /*!< UART registers base address        */
    UART_InitTypeDef              Init;             /*!< UART communication parameters      */

    uint8_t                       *pTxBuffPtr;      /*!< Pointer to UART Tx transfer Buffer */

    uint8_t                       *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */

    uint16_t                      TxXferCount;      /*!< UART Tx Transfer Counter           */

    uint16_t                      RxXferCount;      /*!< UART Rx Transfer Counter           */

    HAL_UART_StateTypeDef         gState;           /*!< UART state information related to global Handle management
                                                                and also related to Tx operations.
                                                                This parameter can be a value of @ref HAL_UART_StateTypeDef */

    HAL_UART_StateTypeDef         RxState;          /*!< UART state information related to Rx operations.*/
        
    uint32_t                 ErrorCode;        /*!< UART Error code                    */

    void *rx_arg;
    void *tx_arg;
} UART_HandleTypeDef;

/** @defgroup UART_Interrupt_definition  UART Interrupt Definitions
  *        Elements values convention: 0xY000XXXX
  *           - XXXX  : Interrupt mask (16 bits) in the Y register
  *           - Y  : Interrupt source register (2bits)
  *                   - 0001: CR1 register
  *                   - 0010: CR2 register
  *                   - 0011: CR3 register
  * @{
  */
#define HAL_UART_ERROR_NONE              0x00000000U   /*!< No error            */
#define HAL_UART_ERROR_OE                0x00000002U   /*!< Overrun error       */
#define HAL_UART_ERROR_PE                0x00000004U   /*!< Parity error        */
#define HAL_UART_ERROR_FE                0x00000008U   /*!< Frame error         */
#define HAL_UART_ERROR_DMA               0x00000010U   /*!< DMA transfer error  */

#define UART_IT_RXRD                     (0X0001)
#define UART_IT_TXS                      (0x0002)
#define UART_IT_RXS                      (0x0004)
#define UART_IT_MDDS                     (0x0008)
#define UART_IT_RTO                      (0x0010)
#define UART_IT_BUSY                     (0x0020)
#define UART_IT_ABE                      (0x0040)
#define UART_IT_ABTO                     (0x0080)
#define UART_ITLINBK                     (0x0100)
#define UART_IT_TC                       (0x0200)
#define UART_IT_EOB                      (0x0400)
#define UART_IT_CM                       (0x0800)

#define UART_SR_DR                       (0X0001)
#define UART_SR_OE                       (0X0002)
#define UART_SR_PE                       (0X0004)
#define UART_SR_FE                       (0X0008)
#define UART_SR_BI                       (0X0010)
#define UART_SR_TBEM                     (0X0020)
#define UART_SR_TEMT                     (0X0040)
#define UART_SR_RFE                      (0X0080)
#define UART_SR_BUSY                     (0X0100)
#define UART_SR_TFNF                     (0X0200)
#define UART_SR_TFEN                     (0X0400)
#define UART_SR_RFNE                     (0X0800)
#define UART_SR_RFF                      (0X1000)
#define UART_SR_DCTS                     (0X2000)
#define UART_SR_CTS                      (0X4000)

#define UART_FCR_RX_DEPTH                (0X0)

#define UART_NOPARITY       0x0     // Parity diable
#define UART_ODDPARITY      0x1     // Parity Odd
#define UART_EVENPARITY     0x3     // Parity Even

#define UART_BYTESIZE5      0X0     // Byte size 5 bits
#define UART_BYTESIZE6      0X1     // Byte size 6 bits
#define UART_BYTESIZE7      0X2     // Byte size 7 bits
#define UART_BYTESIZE8      0X3     // Byte size 8 bits

#define UART_STOPBITS1      0x0     // Stop 1 bits
#define UART_STOPBITS2      0x1     // Stop 2 bits

// UART FIFO Control - FCR
#define UART_RXFIFORST      0x2     // Receiver FIFO reset
#define UART_TXFIFORST      0x4     // Transmit FIFO reset
#define UART_FIFO_RL_1      0x0     // FIFO trigger level   
#define UART_FIFO_RL_8      0x2
#define UART_FIFO_TL_0      0x0     // FIFO trigger level 
#define UART_FIFO_TL_2      0x1     // FIFO trigger level 
#define UART_FIFO_TL_4      0x2     // FIFO trigger level 
#define UART_FIFO_TL_8      0x3


HAL_StatusTypeDef HAL_UART_AutoBaudRate_Detect(UART_HandleTypeDef *huart,uint8_t mode);
HAL_StatusTypeDef HAL_UART_AutoBaudRate_Detect_IT(UART_HandleTypeDef * huart,uint8_t mode);


/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size,uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, void *tx_arg);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size,  void *rx_arg);

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
void HAL_UARTx_IRQHandler(UART_HandleTypeDef *huart);



#endif // _UART_BASE_H_

