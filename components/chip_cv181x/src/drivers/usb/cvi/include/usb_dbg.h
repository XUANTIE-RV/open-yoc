#ifndef USB_DBG_H
#define USB_DBG_H

#include <stdio.h>

/* DEBUG level */
#define USB_DBG_ERR   0
#define USB_DBG_INFO  1
#define USB_DBG_LOG   2

#ifndef USB_LOG_LEVEL
#define USB_LOG_LEVEL USB_DBG_LOG
#endif

#define USB_LOG_RAW printf
#if (USB_LOG_LEVEL >= USB_DBG_LOG)
#define USB_LOG_DBG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define USB_LOG_DBG(...)
#endif

#if (USB_LOG_LEVEL >= USB_DBG_INFO)
#define USB_LOG_INFO(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define USB_LOG_INFO(...)
#endif

#if (USB_LOG_LEVEL >= USB_DBG_ERR)
#define USB_LOG_ERR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define USB_LOG_ERR(...)
#endif

#endif /* USB_DBG_H */
