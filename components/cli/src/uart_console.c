#include <stdint.h>
#include <string.h>
#include <aos/kernel.h>
#include <cli_console.h>
#include <aos/console_uart.h>
#include <devices/uart.h>

int32_t g_cli_direct_read = 0;

/* uart_input_read depends on mcu*/
__attribute__((weak)) int uart_input_read()
{
    return 1;
}

int uart_console_init(void *private_data);
int uart_console_deinit(void *private_data);
int uart_console_write(const void *buf, size_t len, void *privata_data);
int uart_console_read(void *buf, size_t len, void *privata_data);

static device_console uart_console = {
    .name   = "uart-console",
    .fd     = -1,
    .write  = uart_console_write,
    .read   = uart_console_read,
    .init   = uart_console_init,
    .deinit = uart_console_deinit
};

int uart_console_write(const void *buf, size_t len, void *privata_data)
{
    if (buf == NULL) {
        return 0;
    }

    if (!console_get_uart()) {
        return 0;
    }

    rvm_hal_uart_send(console_get_uart(), buf, len, AOS_WAIT_FOREVER);
    return len;
}

int uart_console_read(void *buf, size_t len, void *privata_data)
{
    int ret = -1;
    char *inbuf = (char *)buf;

    unsigned int recv_size = 0;
	unsigned char ch       = 0;

    if (buf == NULL) {
        return 0;
    }

    if (!console_get_uart()) {
        return 0;
    }

    if ( g_cli_direct_read == 0 ) {
        ret = rvm_hal_uart_recv(console_get_uart(), inbuf, 1, AOS_WAIT_FOREVER);
        if (ret == 1) {
            recv_size = 1;
            ret = 0;
        }
        if ((ret == 0) && (recv_size == 1)) {
            return recv_size;
        } else {
            return 0;
        }
    } else {
        do {ch = uart_input_read();}while(ch == 0);
        *inbuf = ch;
        return 1;
    }
}

int uart_console_init(void *private_data)
{
    return 0;
}

int uart_console_deinit(void *private_data)
{
    return 0;
}

cli_console cli_uart_console = {
    .i_list = {0},
    .name = "cli-uart",
    .dev_console = &uart_console,
    .init_flag = 0,
    .exit_flag = 0,
    .alive = 1,
    .private_data = NULL,
    .cli_tag = {0},
    .cli_tag_len = 0,
    .task_list = {0},
    .finsh_callback = NULL,
    .start_callback = NULL,
};
