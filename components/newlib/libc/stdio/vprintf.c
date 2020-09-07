#include <stdarg.h>
#include <devices/uart.h>

#include "serf/minilibc_stdio.h"
#include <devices/uart.h>
#include <csi_core.h>

extern aos_dev_t *g_console_handle;
extern void os_critical_enter();
extern void os_critical_exit();

static int __stdio_outs(const char *s,size_t len) {
#if 0
    uint32_t cur_psr = __get_PSR();
    int flag = 0;

    if (cur_psr & 0x00FF0000) {
        flag = 1;
    } else if ((cur_psr & 0x00000140) != 0x00000140) {
        flag = 1;
    }
    if (flag == 0) {
        if (g_console_handle != NULL) {
            int idx;
            const char *ptr = s;
            while (len) {
                for (idx = 0; idx < len; idx++) {
                    if ('\n' == *(ptr+idx)) {
                        break;
                    }
                }
                if (idx < len) {
                    uart_send(g_console_handle, ptr, idx);
                    uart_send(g_console_handle, "\r\n", 2);
                    len -= idx + 1;
                    ptr += idx + 1;
                } else {
                    uart_send(g_console_handle, ptr, idx);
                    break;
                }
            }
        }
    } else 
#endif    
    {
         int i;

         for(i = 0; i < len; i++) {
            fputc(*(s+i), stdout);
         }
    }
    return 1;
}

int vprintf(const char *format, va_list ap)
{
  struct arg_printf _ap = { 0, (int(*)(void*,size_t,void*)) __stdio_outs };
  return yoc__v_printf(&_ap,format,ap);
}

