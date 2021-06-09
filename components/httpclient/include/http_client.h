// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_HTTP_BUF_SIZE (512)

typedef struct http_client *http_client_handle_t;
typedef struct http_client_event *http_client_event_handle_t;


typedef enum  {
    HTTP_CLI_OK     = 0,
    HTTP_CLI_FAIL   = -1,
    HTTP_CLI_ERR_NO_MEM              = 0x101,   /*!< Out of memory */
    HTTP_CLI_ERR_INVALID_ARG         = 0x102,   /*!< Invalid argument */
    HTTP_CLI_ERR_INVALID_STATE       = 0x103,   /*!< Invalid state */
    HTTP_CLI_ERR_INVALID_SIZE        = 0x104,   /*!< Invalid size */
    HTTP_CLI_ERR_NOT_FOUND           = 0x105,   /*!< Requested resource not found */
    HTTP_CLI_ERR_NOT_SUPPORTED       = 0x106,   /*!< Operation or feature not supported */
    HTTP_CLI_ERR_TIMEOUT             = 0x107,   /*!< Operation timed out */
    HTTP_CLI_ERR_INVALID_RESPONSE    = 0x108,   /*!< Received response was invalid */
    HTTP_CLI_ERR_INVALID_CRC         = 0x109,   /*!< CRC or checksum was invalid */
    HTTP_CLI_ERR_INVALID_VERSION     = 0x10A,   /*!< Version was invalid */
    HTTP_CLI_ERR_INVALID_MAC         = 0x10B    /*!< MAC address was invalid */
} http_errors_t;

/**
 * @brief   HTTP Client events id
 */
typedef enum {
    HTTP_EVENT_ERROR = 0,       /*!< This event occurs when there are any errors during execution */
    HTTP_EVENT_ON_CONNECTED,    /*!< Once the HTTP has been connected to the server, no data exchange has been performed */
    HTTP_EVENT_HEADER_SENT,     /*!< After sending all the headers to the server */
    HTTP_EVENT_ON_HEADER,       /*!< Occurs when receiving each header sent from the server */
    HTTP_EVENT_ON_DATA,         /*!< Occurs when receiving data from the server, possibly multiple portions of the packet */
    HTTP_EVENT_ON_FINISH,       /*!< Occurs when finish a HTTP session */
    HTTP_EVENT_DISCONNECTED,    /*!< The connection has been disconnected */
} http_client_event_id_t;

/**
 * @brief      HTTP Client events data
 */
typedef struct http_client_event {
    http_client_event_id_t event_id;        /*!< event_id, to know the cause of the event */
    http_client_handle_t client;            /*!< http_client_handle_t context */
    void *data;                             /*!< data of the event */
    int data_len;                           /*!< data length of data */
    void *user_data;                        /*!< user_data context, from http_client_config_t user_data */
    char *header_key;                       /*!< For HTTP_EVENT_ON_HEADER event_id, it's store current http header key */
    char *header_value;                     /*!< For HTTP_EVENT_ON_HEADER event_id, it's store current http header value */
} http_client_event_t;


/**
 * @brief      HTTP Client transport
 */
typedef enum {
    HTTP_TRANSPORT_UNKNOWN = 0x0,   /*!< Unknown */
    HTTP_TRANSPORT_OVER_TCP,        /*!< Transport over tcp */
    HTTP_TRANSPORT_OVER_SSL,        /*!< Transport over ssl */
} http_client_transport_t;

typedef int (*http_event_handle_cb)(http_client_event_t *evt);

/**
 * @brief HTTP method
 */
typedef enum {
    HTTP_METHOD_GET = 0,    /*!< HTTP GET Method */
    HTTP_METHOD_POST,       /*!< HTTP POST Method */
    HTTP_METHOD_PUT,        /*!< HTTP PUT Method */
    HTTP_METHOD_PATCH,      /*!< HTTP PATCH Method */
    HTTP_METHOD_DELETE,     /*!< HTTP DELETE Method */
    HTTP_METHOD_HEAD,       /*!< HTTP HEAD Method */
    HTTP_METHOD_NOTIFY,     /*!< HTTP NOTIFY Method */
    HTTP_METHOD_SUBSCRIBE,  /*!< HTTP SUBSCRIBE Method */
    HTTP_METHOD_UNSUBSCRIBE,/*!< HTTP UNSUBSCRIBE Method */
    HTTP_METHOD_OPTIONS,    /*!< HTTP OPTIONS Method */
    HTTP_METHOD_MAX,
} http_client_method_t;

/**
 * @brief HTTP Authentication type
 */
typedef enum {
    HTTP_AUTH_TYPE_NONE = 0,    /*!< No authention */
    HTTP_AUTH_TYPE_BASIC,       /*!< HTTP Basic authentication */
    HTTP_AUTH_TYPE_DIGEST,      /*!< HTTP Disgest authentication */
} http_client_auth_type_t;

/**
 * @brief HTTP configuration
 */
typedef struct {
    const char                  *url;                /*!< HTTP URL, the information on the URL is most important, it overrides the other fields below, if any */
    const char                  *host;               /*!< Domain or IP as string */
    int                         port;                /*!< Port to connect, default depend on http_client_transport_t (80 or 443) */
    const char                  *username;           /*!< Using for Http authentication */
    const char                  *password;           /*!< Using for Http authentication */
    http_client_auth_type_t     auth_type;           /*!< Http authentication type, see `http_client_auth_type_t` */
    const char                  *path;               /*!< HTTP Path, if not set, default is `/` */
    const char                  *query;              /*!< HTTP query */
    const char                  *cert_pem;           /*!< SSL server certification, PEM format as string, if the client requires to verify server */
    const char                  *client_cert_pem;    /*!< SSL client certification, PEM format as string, if the server requires to verify client */
    const char                  *client_key_pem;     /*!< SSL client key, PEM format as string, if the server requires to verify client */
    http_client_method_t        method;                   /*!< HTTP Method */
    int                         timeout_ms;               /*!< Network timeout in milliseconds */
    bool                        disable_auto_redirect;    /*!< Disable HTTP automatic redirects */
    int                         max_redirection_count;    /*!< Max redirection number, using default value if zero*/
    http_event_handle_cb        event_handler;             /*!< HTTP Event Handle */
    http_client_transport_t     transport_type;           /*!< HTTP transport type, see `http_client_transport_t` */
    int                         buffer_size;              /*!< HTTP buffer size (both send and receive) */
    void                        *user_data;               /*!< HTTP user_data context */
    bool                        is_async;                 /*!< Set asynchronous mode, only supported with HTTPS for now */
    bool                        use_global_ca_store;      /*!< Use a global ca_store for all the connections in which this bool is set. */
} http_client_config_t;

/**
 * Enum for the HTTP status codes.
 */
typedef enum {
    /* 3xx - Redirection */
    HttpStatus_MovedPermanently  = 301,
    HttpStatus_Found             = 302,
    HttpStatus_TemporaryRedirect = 307,

    /* 4xx - Client Error */
    HttpStatus_Unauthorized      = 401,
    HttpStatus_Forbidden         = 403,
    HttpStatus_NotFound          = 404,

    /* 5xx - Server Error */
    HttpStatus_InternalError     = 500
} HttpStatus_Code;

/**
 * @brief      Start a HTTP session
 *             This function must be the first function to call,
 *             and it returns a http_client_handle_t that you must use as input to other functions in the interface.
 *             This call MUST have a corresponding call to http_client_cleanup when the operation is complete.
 *
 * @param[in]  config   The configurations, see `http_client_config_t`
 *
 * @return
 *     - `http_client_handle_t`
 *     - NULL if any errors
 */
http_client_handle_t http_client_init(const http_client_config_t *config);

/**
 * @brief      Invoke this function after `http_client_init` and all the options calls are made, and will perform the
 *             transfer as described in the options. It must be called with the same http_client_handle_t as input as the http_client_init call returned.
 *             http_client_perform performs the entire request in either blocking or non-blocking manner. By default, the API performs request in a blocking manner and returns when done,
 *             or if it failed, and in non-blocking manner, it returns if EAGAIN/EWOULDBLOCK or EINPROGRESS is encountered, or if it failed. And in case of non-blocking request,
 *             the user may call this API multiple times unless request & response is complete or there is a failure. To enable non-blocking http_client_perform(), `is_async` member of http_client_config_t
 *             must be set while making a call to http_client_init() API.
 *             You can do any amount of calls to http_client_perform while using the same http_client_handle_t. The underlying connection may be kept open if the server allows it.
 *             If you intend to transfer more than one file, you are even encouraged to do so.
 *             http_client will then attempt to re-use the same connection for the following transfers, thus making the operations faster, less CPU intense and using less network resources.
 *             Just note that you will have to use `http_client_set_**` between the invokes to set options for the following http_client_perform.
 *
 * @note       You must never call this function simultaneously from two places using the same client handle.
 *             Let the function return first before invoking it another time.
 *             If you want parallel transfers, you must use several http_client_handle_t.
 *             This function include `http_client_open` -> `http_client_write` -> `http_client_fetch_headers` -> `http_client_read` (and option) `http_client_close`.
 *
 * @param      client  The http_client handle
 *
 * @return
 *  - HTTP_CLI_OK on successful
 *  - HTTP_CLI_FAIL on error
 */
http_errors_t http_client_perform(http_client_handle_t client);

/**
 * @brief      Set URL for client, when performing this behavior, the options in the URL will replace the old ones
 *
 * @param[in]  client  The http_client handle
 * @param[in]  url     The url
 *
 * @return
 *  - HTTP_CLI_OK
 *  - HTTP_CLI_FAIL
 */
http_errors_t http_client_set_url(http_client_handle_t client, const char *url);

/**
 * @brief      Set post data, this function must be called before `http_client_perform`.
 *             Note: The data parameter passed to this function is a pointer and this function will not copy the data
 *
 * @param[in]  client  The http_client handle
 * @param[in]  data    post data pointer
 * @param[in]  len     post length
 *
 * @return
 *  - HTTP_CLI_OK
 *  - HTTP_CLI_FAIL
 */
http_errors_t http_client_set_post_field(http_client_handle_t client, const char *data, int len);

/**
 * @brief      Get current post field information
 *
 * @param[in]  client  The client
 * @param[out] data    Point to post data pointer
 *
 * @return     Size of post data
 */
int http_client_get_post_field(http_client_handle_t client, char **data);

/**
 * @brief      Set http request header, this function must be called after http_client_init and before any
 *             perform function
 *
 * @param[in]  client  The http_client handle
 * @param[in]  key     The header key
 * @param[in]  value   The header value
 *
 * @return
 *  - HTTP_CLI_OK
 *  - HTTP_CLI_FAIL
 */
http_errors_t http_client_set_header(http_client_handle_t client, const char *key, const char *value);

/**
 * @brief      Get http request header.
 *             The value parameter will be set to NULL if there is no header which is same as
 *             the key specified, otherwise the address of header value will be assigned to value parameter.
 *             This function must be called after `http_client_init`.
 *
 * @param[in]  client  The http_client handle
 * @param[in]  key     The header key
 * @param[out] value   The header value
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_FAIL
 */
http_errors_t http_client_get_header(http_client_handle_t client, const char *key, char **value);

/**
 * @brief      Get http request username.
 *             The address of username buffer will be assigned to value parameter.
 *             This function must be called after `http_client_init`.
 *
 * @param[in]  client  The http_client handle
 * @param[out] value   The username value
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_ERR_INVALID_ARG
 */
http_errors_t http_client_get_username(http_client_handle_t client, char **value);

/**
 * @brief      Get http request password.
 *             The address of password buffer will be assigned to value parameter.
 *             This function must be called after `http_client_init`.
 *
 * @param[in]  client  The http_client handle
 * @param[out] value   The password value
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_ERR_INVALID_ARG
 */
http_errors_t http_client_get_password(http_client_handle_t client, char **value);

/**
 * @brief      Set http request method
 *
 * @param[in]  client  The http_client handle
 * @param[in]  method  The method
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_ERR_INVALID_ARG
 */
http_errors_t http_client_set_method(http_client_handle_t client, http_client_method_t method);

/**
 * @brief      Delete http request header
 *
 * @param[in]  client  The http_client handle
 * @param[in]  key     The key
 *
 * @return
 *  - HTTP_CLI_OK
 *  - HTTP_CLI_FAIL
 */
http_errors_t http_client_delete_header(http_client_handle_t client, const char *key);

/**
 * @brief      This function will be open the connection, write all header strings and return
 *
 * @param[in]  client     The http_client handle
 * @param[in]  write_len  HTTP Content length need to write to the server
 *
 * @return
 *  - HTTP_CLI_OK
 *  - HTTP_CLI_FAIL
 */
http_errors_t http_client_open(http_client_handle_t client, int write_len);

/**
 * @brief     This function will write data to the HTTP connection previously opened by http_client_open()
 *
 * @param[in]  client  The http_client handle
 * @param      buffer  The buffer
 * @param[in]  len     This value must not be larger than the write_len parameter provided to http_client_open()
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data written
 */
int http_client_write(http_client_handle_t client, const char *buffer, int len);

/**
 * @brief      This function need to call after http_client_open, it will read from http stream, process all receive headers
 *
 * @param[in]  client  The http_client handle
 *
 * @return
 *     - (0) if stream doesn't contain content-length header, or chunked encoding (checked by `http_client_is_chunked` response)
 *     - (-1: HTTP_CLI_FAIL) if any errors
 *     - Download data length defined by content-length header
 */
int http_client_fetch_headers(http_client_handle_t client);


/**
 * @brief      Check response data is chunked
 *
 * @param[in]  client  The http_client handle
 *
 * @return     true or false
 */
bool http_client_is_chunked_response(http_client_handle_t client);

/**
 * @brief      Read data from http stream
 *
 * @param[in]  client  The http_client handle
 * @param      buffer  The buffer
 * @param[in]  len     The length
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data was read
 */
int http_client_read(http_client_handle_t client, char *buffer, int len);


/**
 * @brief      Get http response status code, the valid value if this function invoke after `http_client_perform`
 *
 * @param[in]  client  The http_client handle
 *
 * @return     Status code
 */
int http_client_get_status_code(http_client_handle_t client);

/**
 * @brief      Get http response content length (from header Content-Length)
 *             the valid value if this function invoke after `http_client_perform`
 *
 * @param[in]  client  The http_client handle
 *
 * @return
 *     - (-1) Chunked transfer
 *     - Content-Length value as bytes
 */
int http_client_get_content_length(http_client_handle_t client);

/**
 * @brief      Close http connection, still kept all http request resources
 *
 * @param[in]  client  The http_client handle
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_FAIL
 */
http_errors_t http_client_close(http_client_handle_t client);

/**
 * @brief      This function must be the last function to call for an session.
 *             It is the opposite of the http_client_init function and must be called with the same handle as input that a http_client_init call returned.
 *             This might close all connections this handle has used and possibly has kept open until now.
 *             Don't call this function if you intend to transfer more files, re-using handles is a key to good performance with http_client.
 *
 * @param[in]  client  The http_client handle
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_FAIL
 */
http_errors_t http_client_cleanup(http_client_handle_t client);

/**
 * @brief      Get transport type
 *
 * @param[in]  client   The http_client handle
 *
 * @return
 *     - HTTP_TRANSPORT_UNKNOWN
 *     - HTTP_TRANSPORT_OVER_TCP
 *     - HTTP_TRANSPORT_OVER_SSL
 */
http_client_transport_t http_client_get_transport_type(http_client_handle_t client);

/**
 * @brief      Set redirection URL.
 *             When received the 30x code from the server, the client stores the redirect URL provided by the server.
 *             This function will set the current URL to redirect to enable client to execute the redirection request.
 *
 * @param[in]  client  The http_client handle
 *
 * @return
 *     - HTTP_CLI_OK
 *     - HTTP_CLI_FAIL
 */
http_errors_t http_client_set_redirection(http_client_handle_t client);

/**
 * @brief      Checks if entire data in the response has been read without any error.
 *
 * @param[in]  client   The http_client handle
 *
 * @return
 *     - true
 *     - false
 */
bool http_client_is_complete_data_received(http_client_handle_t client);

/**
 * @brief      Helper API to read larger data chunks
 *             This is a helper API which internally calls `http_client_read` multiple times till the end of data is reached or till the buffer gets full.
 *
 * @param[in]  client   The http_client handle
 * @param      buffer   The buffer
 * @param[in]  len      The buffer length
 *
 * @return
 *     - Length of data was read
 */

int http_client_read_response(http_client_handle_t client, char *buffer, int len);

#ifdef __cplusplus
}
#endif


#endif
