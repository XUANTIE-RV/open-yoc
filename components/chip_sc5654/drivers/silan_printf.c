
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#if 0
//#include "silan_uart.h"
#include <drv/usart.h>

#define    CFG_PBSIZE    384

//#pragma import(__use_no_semihosting_swi)

#define is_digit(c)    ((c) >= '0' && (c) <= '9')

static uint8_t on_printf = 1;

#define ZEROPAD    1        /* pad with zero */
#define SIGN    2        /* unsigned/signed long */
#define PLUS    4        /* show plus */
#define SPACE    8        /* space if plus */
#define LEFT    16        /* left justified */
#define SPECIAL    32        /* 0x */
#define LARGE    64        /* use 'ABCDEF' instead of 'abcdef' */

#define do_div(n,base) ({ \
        int __res; \
        __res = ((unsigned long) n) % (unsigned) base; \
        n = ((unsigned long) n) / (unsigned) base; \
        __res; })

extern usart_handle_t console_handle;
extern int32_t csi_usart_getchar(usart_handle_t handle, uint8_t *ch);
void serial_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            //while(serial_putc('\r') == -1);
            csi_usart_putchar(console_handle, '\r');
        }

        csi_usart_putchar(console_handle, *s);
        //while(serial_putc (*s) == -1);
        s++;
    }
}

static void sl_puts(const char *s)
{
    serial_puts(s);
}

void sl_printf_onoff(uint8_t onoff)
{
    on_printf = onoff;
}

void sl_printf(const char *fmt, ...)
{

    va_list args;
//    unsigned int   i;
    char printbuffer[CFG_PBSIZE];

    if (on_printf) {
        va_start(args, fmt);

        /* For this to work, printbuffer must be larger than
         * anything we ever want to print.
         */
        vsprintf(printbuffer, fmt, args);
        va_end(args);

        /* Print the string */
        sl_puts(printbuffer);
    }
}

void assert_report(const char *file_name, const char *function_name, unsigned int line_no)
{
    sl_printf("[ERROR]file_name: %s, function_name: %s, line_no: %d\n", file_name, function_name, line_no);
}


//#endif /*__PRINTF_SUPPORT__*/

#endif