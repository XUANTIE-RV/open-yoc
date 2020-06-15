

#ifndef __WEB_ERRNO_H__
#define __WEB_ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int web_err_t;

/* Definitions for error constants. */
#define WEB_OK          0       /*!< web_err_t value indicating success (no error) */
#define WEB_FAIL        -1      /*!< Generic web_err_t code indicating failure */

#define WEB_ERR_NO_MEM              0x101   /*!< Out of memory */
#define WEB_ERR_INVALID_ARG         0x102   /*!< Invalid argument */
#define WEB_ERR_INVALID_STATE       0x103   /*!< Invalid state */
#define WEB_ERR_INVALID_SIZE        0x104   /*!< Invalid size */
#define WEB_ERR_NOT_FOUND           0x105   /*!< Requested resource not found */
#define WEB_ERR_NOT_SUPPORTED       0x106   /*!< Operation or feature not supported */
#define WEB_ERR_TIMEOUT             0x107   /*!< Operation timed out */
#define WEB_ERR_INVALID_RESPONSE    0x108   /*!< Received response was invalid */
#define WEB_ERR_INVALID_CRC         0x109   /*!< CRC or checksum was invalid */
#define WEB_ERR_INVALID_VERSION     0x10A   /*!< Version was invalid */
#define WEB_ERR_INVALID_MAC         0x10B   /*!< MAC address was invalid */

#define WEB_ERR_WIFI_BASE           0x3000  /*!< Starting number of WiFi error codes */
#define WEB_ERR_MESH_BASE           0x4000  /*!< Starting number of MESH error codes */

#ifdef __cplusplus
}
#endif

#endif /* __WEB_ERRNO_H__ */

