#include <stdint.h>
#include <string.h>
#include <soc.h>
#include <aos/console_uart.h>
#include <devices/uart.h>

#if defined(DEBUG_LAST_WORD_ENABLE) && (DEBUG_LAST_WORD_ENABLE > 0)

int clear_silent_reboot_flag(void)
{
    // do nothing
    return 0;
}

int set_silent_reboot_flag(void)
{
    // do nothing
    return 0;
}

void k_dcache_clean(uint32_t addr, uint32_t len)
{
    csi_dcache_clean_range((uint32_t *)addr, (int32_t)len);
}

/* set ram attributes for some mcu*/
void alios_debug_lastword_init_hook()
{

}
#endif

#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
int alios_debug_print(const char *buf, int size)
{
    if (buf == NULL || size <= 0) {
        return -1;
    }

    if (!console_get_uart()) {
        return 0;
    }

#if defined(CONFIG_AOS_NEWLINE_SUPPORT) && (CONFIG_AOS_NEWLINE_SUPPORT > 0)
    if((size >= 2 && buf[size - 1] == '\n' && buf[size - 2] != '\r')) {
        rvm_hal_uart_send_poll(console_get_uart(), buf, size - 1);
        rvm_hal_uart_send_poll(console_get_uart(), "\r\n", 2);
    } else if(size == 1 && buf[0] == '\n') {
        rvm_hal_uart_send_poll(console_get_uart(), "\r\n", 2);
    } else {
        rvm_hal_uart_send_poll(console_get_uart(), buf, size);
    }
#else
    rvm_hal_uart_send_poll(console_get_uart(), buf, size);
#endif /*CONFIG_AOS_NEWLINE_SUPPORT*/

    return size;
}
#endif

#if defined(AOS_COMP_CLI) && (AOS_COMP_CLI > 0)
int uart_input_read()
{
    int ret;
    uint8_t rx_byte;

    if (!console_get_uart()) {
        return 0;
    }

    ret = rvm_hal_uart_recv_poll(console_get_uart(), &rx_byte, 1);

    return ret > 0 ? rx_byte : 0;
}
#endif
