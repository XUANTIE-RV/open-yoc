#ifndef __HTTP_TLS_H__
#define __HTTP_TLS_H__

#if defined(CONFIG_USING_TLS)
#include <transport/tls.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief      Create a new blocking TLS/SSL connection with a given "HTTP" url
 *
 * The behaviour is same as tls_conn_new() API. However this API accepts host's url.
 *
 * @param[in]  url  url of host.
 * @param[in]  cfg  TLS configuration as tls_cfg_t. If you wish to open
 *                  non-TLS connection, keep this NULL. For TLS connection,
 *                  a pass pointer to 'tls_cfg_t'. At a minimum, this
 *                  structure should be zero-initialized.
 * @return pointer to tls_t, or NULL if connection couldn't be opened.
 */
tls_t *tls_conn_http_new(const char *url, const tls_cfg_t *cfg);

/**
 * @brief      Create a new non-blocking TLS/SSL connection with a given "HTTP" url
 *
 * The behaviour is same as tls_conn_new() API. However this API accepts host's url.
 *
 * @param[in]  url     url of host.
 * @param[in]  cfg     TLS configuration as tls_cfg_t.
 * @param[in]  tls     pointer to tls as tls handle.
 *
 * @return
 *             - -1     If connection establishment fails.
 *             -  0     If connection establishment is in progress.
 *             -  1     If connection establishment is successful.
 */
int tls_conn_http_new_async(const char *url, const tls_cfg_t *cfg, tls_t *tls);

#ifdef __cplusplus
}
#endif
#endif
#endif