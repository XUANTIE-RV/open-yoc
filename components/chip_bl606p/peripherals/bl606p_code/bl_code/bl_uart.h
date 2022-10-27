#ifndef __BL_UART_H__
#define __BL_UART_H__
#include <stdint.h>
#define BL_UART_BUFFER_SIZE_MIN   (128)
#define BL_UART_BUFFER_SIZE_MASK  (128 - 1)

/**
 *  @brief uart config cmd type definition
 */
typedef enum {
    bl_uart_set_baud_cmd,
    bl_uart_set_data_bit_cmd,
    bl_uart_set_parity_bit_cmd,
    bl_uart_set_stop_bit_cmd,
    bl_uart_set_bit_inverse_cmd,
    bl_uart_set_flow_cmd
} uart_cfg_type;

/**
 *  @brief UART interrupt type definition
 */
typedef enum {
    bl_uart_irq_tx_end,      /*!< UART tx transfer end interrupt */
    bl_uart_irq_rx_end,      /*!< UART rx transfer end interrupt */
    bl_uart_irq_tx_fifo_req, /*!< UART tx fifo interrupt when tx fifo count reaches,auto clear */
    bl_uart_irq_rx_fifo_req, /*!< UART rx fifo interrupt when rx fifo count reaches,auto clear */
    bl_uart_irq_rto,         /*!< UART rx time-out interrupt */
    bl_uart_irq_pce,         /*!< UART rx parity check error interrupt */
    bl_uart_irq_tx_fer,      /*!< UART tx fifo overflow/underflow error interrupt */
    bl_uart_irq_rx_fer,      /*!< UART rx fifo overflow/underflow error interrupt */
    bl_uart_irq_lse,         /*!< UART rx lin mode sync field error interrupt */
    bl_uart_irq_bcr,         /*!< UART rx byte count reached interrupt */
    bl_uart_irq_startbit,    /*!< UART rx auto baudrate detection finish interrupt using start bit */
    bl_uart_irq_0x55,        /*!< UART rx auto baudrate detection finish interrupt using codeword 0x55 */
    bl_uart_irq_all,         /*!< All the interrupt */
} uart_irq_type;

/**
 * @brief UART configuration
 */
typedef struct {
    uint8_t                   uart_id;      /**< @brief UART id */
    uint8_t                   tx_pin;       /**< @brief UART tx pin */
    uint8_t                   rx_pin;       /**< @brief UART rx pin */
    uint8_t                   cts_pin;      /**< @brief UART cts pin */
    uint8_t                   rts_pin;      /**< @brief UART rts pin */
    uint32_t                  baud_rate;    /**< @brief UART baud rate */
    uint8_t                   data_width;   /**< @brief UART data width */
    uint8_t                   parity;       /**< @brief UART parity bit */
    uint8_t                   stop_bits;    /**< @brief UART stop btis */
    uint8_t                   bitInverse;   /**< @brief UART LSB-first or  MSB-first*/
} bl_uart_config_t;

/**
 *  @brief UART FIFO configuration structure type definition
 */
typedef struct
{
#define bl_uart_dam_fifo_lth_max  32
    uint8_t txFifoDmaThreshold;  /*!< TX FIFO threshold, [28:24] dma tx request will not be asserted if tx fifo count is less than this value */
    uint8_t rxFifoDmaThreshold;  /*!< RX FIFO threshold, [20:16] dma rx request will not be asserted if rx fifo count is less than this value */
    BL_Fun_Type txFifoDmaEnable; /*!< Enable or disable tx dma req/ack interface */
    BL_Fun_Type rxFifoDmaEnable; /*!< Enable or disable rx dma req/ack interface */
} uart_dma_fifo_cfg_type;


typedef void (*cb_uart_notify_t)(void *arg);
void bl_uart_port_enable(uint8_t id);
void bl_uart_port_disable(uint8_t id);

int bl_uart_gpio_init(uint8_t id, uint8_t tx, uint8_t rx, uint8_t rts, uint8_t cts, int baudrate);

int bl_uart_simple_init(uint8_t id, uint8_t tx_pin, uint8_t rx_pin, uint8_t cts_pin, uint8_t rts_pin, uint32_t baudrate);
int bl_uart_debug_early_init(uint32_t baudrate);
int bl_uart_early_init(uint8_t id, uint8_t tx_pin, uint32_t baudrate);
int bl_uart_init(uint8_t id, uint8_t tx_pin, uint8_t rx_pin, uint8_t cts_pin, uint8_t rts_pin, uint32_t baudrate);

int32_t bl_uart_config(uint8_t uart_id, uart_cfg_type cfg_set, bl_uart_config_t* cfg);
uint32_t bl_uart_get_txfifo_count(uint8_t uartId);
uint32_t bl_uart_get_rxfifo_count(uint8_t uartId);

int bl_uart_string_send(uint8_t id, char *data);
int bl_uart_flush(uint8_t id);
void bl_uart_setbaud(uint8_t id, uint32_t baud);
int bl_uart_buff_send(uint8_t id, uint8_t data);
int bl_uart_buff_recv(uint8_t id);
int bl_uart_buffs_send(uint8_t id, uint8_t *data, int len);
int bl_uart_buffs_recv(uint8_t id, uint8_t *data, int len);

int bl_uart_irq_rx_enable(uint8_t id);
int bl_uart_irq_rx_disable(uint8_t id);
int bl_uart_irq_tx_enable(uint8_t id);
int bl_uart_irq_tx_disable(uint8_t id);
int bl_uart_irq_enable(uint8_t id);
int bl_uart_irq_disable(uint8_t id);
int bl_uart_irq_rx_notify_register(uint8_t id, cb_uart_notify_t cb, void *arg);
int bl_uart_irq_tx_notify_register(uint8_t id, cb_uart_notify_t cb, void *arg);
int bl_uart_irq_rx_notify_unregister(uint8_t id, cb_uart_notify_t cb, void *arg);
int bl_uart_irq_tx_notify_unregister(uint8_t id, cb_uart_notify_t cb, void *arg);
#endif
