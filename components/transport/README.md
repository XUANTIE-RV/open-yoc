# 传输层网络协议（transport）

## 概述

transport对socket操作做了一层封装，管理多个连接。为`httpclient`和`websocket`提供统一的接口。

## 组件安装

```bash
yoc init
yoc install transport
```

## 配置
无。

## 接口列表

transport接口如下所示：

### tls接口

| 函数 | 说明 |
| :--- | :--- |
| tls_conn_new | 创建新的阻塞TLS/SSL连接 |
| tls_conn_new_async | 创建新的非阻塞TLS/SSL连接 |
| tls_conn_delete | 关闭TLS/SSL连接并释放所有分配的资源 |
| tls_get_bytes_avail | 返回要从当前记录中读取的应用程序数据剩余字节数 |
| tls_init_global_ca_store | 创建一个全局CA存储，初始为空 |
| tls_set_global_ca_store | 使用pem格式提供的缓冲区设置全局CA存储 |
| tls_get_global_ca_store | 获取指向当前正在使用的全局CA存储的指针 |
| tls_free_global_ca_store | 释放当前正在使用的全局CA存储 |

### transport接口

| 函数 | 说明 |
| :--- | :--- |
| transport_list_init | 创建传输列表 |
| transport_list_destroy | 清除并释放所有传输，包括自身，此函数将调用传输，销毁所有已添加此列表的传输 |
| transport_list_add | 将传输添加到列表中，并定义一个方案以在列表中标识此传输 |
| transport_list_clean | 此函数将从列表中删除所有传输，调用传输并销毁已将此添加到列表中的每个传输 |
| transport_list_get_transport | 获取在调用函数“transport_list_add”时定义的传输方式方案 |
| transport_init | 初始化传输句柄对象 |
| transport_destroy | 清除并释放传输中的内存 |
| transport_get_default_port | 获取此传输使用的默认端口号 |
| transport_set_default_port | 设置此传输可以使用的默认端口号 |
| transport_connect | 传输连接功能，用于与服务器建立连接 |
| transport_connect_async | 非阻塞传输连接功能，与服务器建立连接 |
| transport_read | 传输读取功能 |
| transport_poll_read | 轮询传输直到可读或超时 |
| transport_write | 传输写函数 |
| transport_poll_write | 轮询传输直到可写或超时 |
| transport_close | 传输关闭 |
| transport_get_context_data | 获取此传输的用户数据上下文 |
| transport_get_payload_transport_handle | 获取可直接访问此协议负载的基础协议的传输句柄（用于多次接收较长的消息） |
| transport_set_context_data | 设置此传输的用户上下文数据 |
| transport_set_func | 设置传输句柄的传输功能 |
| transport_set_async_connect_func | 设置传输句柄的异步传输功能 |
| transport_set_parent_transport_func | 将父传输函数设置为句柄 |

### transport_ssl接口

| 函数 | 说明 |
| :--- | :--- |
| transport_ssl_init | 创建新的SSL传输时，传输句柄必须释放 transport_destroy 回调 |
| transport_ssl_set_cert_data | 设置SSL证书数据（PEM格式）。请注意，此函数存储指向数据的指针，而不是制作副本。 因此，在清除连接之前，此数据必须保持有效 |
| transport_ssl_enable_global_ca_store | 为SSL连接启用全局CA存储 |
| transport_ssl_set_client_cert_data | 为相互身份验证设置SSL客户端证书数据（PEM格式）。请注意，此函数存储指向数据的指针，而不是制作副本。 因此，在清除连接之前，此数据必须保持有效 |
| transport_ssl_set_client_key_data | 为相互身份验证设置SSL客户端密钥数据（PEM格式）。请注意，此函数存储指向数据的指针，而不是收到。那么在清除连接之前，此数据必须保持有效 |

### transport_tcp接口

| 函数 | 说明 |
| :--- | :--- |
| transport_tcp_init | 创建TCP传输时，传输句柄必须释放 transport_destroy 回调 |

### transport_utils接口

| 函数 | 说明 |
| :--- | :--- |
| transport_utils_ms_to_timeval | 将毫秒转换为timeval结构 |
| transport_utils_random | 生成指定长度的随机数 |

### transport_ws接口

| 函数 | 说明 |
| :--- | :--- |
| transport_ws_init | 创建web套接字传输 |
| transport_ws_set_path | 设置路径 |
| transport_ws_set_sendtype | 设置发送类型 |

## 接口详细说明

### 枚举定义

#### tls_conn_state_t
| 类型 | 说明 |
| :--- | :--- |
| TLS_INIT | 初始化 |
| TLS_CONNECTING | 连接中 |
| TLS_HANDSHAKE | 握手 |
| TLS_FAIL | 失败 |
| TLS_DONE | 完成 |

### 结构体定义

#### tls_cfg_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| alpn_protos | const char** | HTTP2所需的应用程序协议。如果需要HTTP2/ALPN支持，则应协商好的格式是长度后跟协议名称。 |
| cacert_pem_buf | const unsigned char* | 缓冲区中证书颁发机构的证书 |
| cacert_pem_bytes | unsigned int | cacert_pem_buf指向的证书颁发机构证书的大小 |
| clientcert_pem_buf | const unsigned char* | 缓冲区中的客户端证书 |
| clientcert_pem_bytes | unsigned int | clientcert_pem_buf指向的客户端证书的大小 |
| clientkey_pem_buf | const unsigned char* | 缓冲区中的客户端密钥 |
| clientkey_pem_bytes | unsigned int | clientkey_pem_buf指向的客户端密钥的大小 |
| clientkey_password | const unsigned char* | 客户端密钥解密密码字符串 |
| clientkey_password_len | unsigned int | clientkey_password指向的密码的字符串长度 |
| non_block | bool | 配置非阻塞模式。如果设置为true，则在tls会话建立后，将在非阻塞模式下配置下面的套接字 |
| timeout_ms | int | 网络超时时间（毫秒） |
| use_global_ca_store | bool | 对设置此bool的所有连接使用全局ca存储 |

#### tls_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| ssl | mbedtls_ssl_context | TLS/SSL上下文 |
| entropy | mbedtls_entropy_context | mbedTLS熵上下文结构 |
| ctr_drbg | mbedtls_ctr_drbg_context | mbedTLS ctr drbg上下文结构 |
| conf | mbedtls_ssl_config | 在mbedtls_ssl上下文结构之间共享的TLS/SSL配置 |
| server_fd | mbedtls_net_context | 套接字的mbedTLS包装类型 |
| cacert | mbedtls_x509_crt | X.509 CA证书的容器 |
| cacert_ptr | mbedtls_x509_crt* | 指向正在使用的cacert的指针 |
| clientcert | mbedtls_x509_crt | X.509客户端证书的容器 |
| clientkey | mbedtls_pk_context | 客户端证书私钥的容器 |
| sockfd | int | 底层套接字文件描述符 |
| read | ssize_t | 用于从TLS/SSL连接读取数据的回调函数 |
| write | ssize_t | 用于将数据写入TLS/SSL连接的回调函数 |
| conn_state | tls_conn_state_t | TLS连接状态 |
| rset | fd_set | 读取文件描述符 |
| wset | fd_set | 写入文件描述符 |
| is_tls | bool | 表示连接类型（TLS或非TLS） |

#### transport_handle_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| port | int | 端口 |
| socket | int | 此传输中要使用的套接字 |
| scheme | char* | 标记名 |
| context | void* | 上下文数据 |
| data | void* | 附加传输数据 |
| _connect | connect_func | 连接此传输的功能 |
| _read | io_read_func | 读 |
| _write | io_func | 写 |
| _close | trans_func | 关闭 |
| _poll_read | poll_func | 轮询并读取 |
| _poll_write | poll_func | 轮询和写入 |
| _destroy | trans_func | 销毁和释放传输 |
| _connect_async | connect_async_func | 此传输的非阻塞连接功能 |
| _parent_transfer | payload_transfer_func | 返回底层传输层功能 |
| next | STAILQ_ENTRY(transport_item_t) |  |

接口的具体形式如下：

### tls接口

### tls_conn_new
`tls_t *tls_conn_new(const char *hostname, int hostlen, int port, const tls_cfg_t *cfg);`

- 功能描述:
   - 创建新的阻塞TLS/SSL连接。

- 参数:
   - `hostname`: 主机的主机名。
   - `hostlen`: 主机名长度。
   - `port`: 主机的端口号。
   - `cfg`: TLS配置为tls_cfg_t。

- 返回值:
   - tls_t指针: 成功。
   - NULL: 失败。

### tls_conn_new_async
`int tls_conn_new_async(const char *hostname, int hostlen, int port, const tls_cfg_t *cfg, tls_t *tls);`

- 功能描述:
   - 创建新的非阻塞TLS/SSL连接。

- 参数:
   - `hostname`: 主机的主机名。
   - `hostlen`: 主机名长度。
   - `port`: 主机的端口号。
   - `cfg`: TLS配置为tls_cfg_t。
   - `tls`: 指向tls作为tls句柄的指针。

- 返回值:
   - -1: 失败。
   - 0: 连接建立中。
   - 1: 成功。

### tls_conn_delete
`void tls_conn_delete(tls_t *tls);`

- 功能描述:
   - 关闭TLS/SSL连接并释放所有分配的资源。

- 参数:
   - `tls`: 指向tls作为tls句柄的指针。

- 返回值:
   - 无。

### tls_get_bytes_avail
`size_t tls_get_bytes_avail(tls_t *tls);`

- 功能描述:
   - 关闭TLS/SSL连接并释放所有分配的资源。

- 参数:
   - `tls`: 指向tls作为tls句柄的指针。

- 返回值:
   - -1: 无效参数。
   - 大于0: 可用字节数。

### tls_init_global_ca_store
`web_err_t tls_init_global_ca_store();`

- 功能描述:
   - 创建一个全局CA存储，初始为空。

- 参数:
   - 无。

- 返回值:
   - WEB_OK: 成功。
   - WEB_ERR_NO_MEM: 连接建立中。

### tls_set_global_ca_store
`web_err_t tls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes);`

- 功能描述:
   - 使用pem格式提供的缓冲区设置全局CA存储。

- 参数:
   - `cacert_pem_buf`: 具有pem格式证书的缓冲区。
   - `cacert_pem_bytes`: 缓冲区长度。

- 返回值:
   - WEB_OK: 成功。
   - Other: 其他错误码。

### tls_get_global_ca_store
`mbedtls_x509_crt *tls_get_global_ca_store();`

- 功能描述:
   - 获取指向当前正在使用的全局CA存储的指针。

- 参数:
   - 无

- 返回值:
   - mbedtls_x509_crt指针: 成功。
   - NULL: 失败。

### tls_free_global_ca_store
`void tls_free_global_ca_store();`

- 功能描述:
   - 释放当前正在使用的全局CA存储。

- 参数:
   - 无。

- 返回值:
   - 无。

### transport接口

### transport_list_init
`transport_list_handle_t transport_list_init();`

- 功能描述:
   - 创建传输列表。

- 参数:
   - 无

- 返回值:
   - 返回句柄。

### transport_list_destroy
`web_err_t transport_list_destroy(transport_list_handle_t list);`

- 功能描述:
   - 清除并释放所有传输，包括自身，此函数将调用传输，销毁所有已添加此列表的传输。

- 参数:
   - `list`: 列表。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_list_add
`web_err_t transport_list_add(transport_list_handle_t list, transport_handle_t t, const char *scheme);`

- 功能描述:
   - 将传输添加到列表中，并定义一个方案以在列表中标识此传输。

- 参数:
   - `list`: 列表。
   - `t`: 传输。
   - `scheme`: 方案。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_list_clean
`web_err_t transport_list_clean(transport_list_handle_t list);`

- 功能描述:
   - 此函数将从列表中删除所有传输，调用传输并销毁已将此添加到列表中的每个传输。

- 参数:
   - `list`: 列表。

- 返回值:
   - WEB_OK: 成功。
   - WEB_ERR_INVALID_ARG: 无效参数。

### transport_list_get_transport
`transport_handle_t transport_list_get_transport(transport_list_handle_t list, const char *scheme);`

- 功能描述:
   - 获取在调用函数“transport_list_add”时定义的传输方式方案。

- 参数:
   - `list`: 列表。
   - `scheme`: 方案。

- 返回值:
   - 返回传输句柄。

### transport_init
`transport_handle_t transport_init();`

- 功能描述:
   - 初始化传输句柄对象。

- 参数:
   - 无。

- 返回值:
   - 返回传输句柄。

### transport_destroy
`web_err_t transport_destroy(transport_handle_t t);`

- 功能描述:
   - 清除并释放传输中的内存。

- 参数:
   - `t`: 句柄。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_get_default_port
`int transport_get_default_port(transport_handle_t t);`

- 功能描述:
   - 获取此传输使用的默认端口号。

- 参数:
   - `t`: 句柄。

- 返回值:
   - 返回端口号。

### transport_set_default_port
`web_err_t transport_set_default_port(transport_handle_t t, int port);`

- 功能描述:
   - 设置此传输可以使用的默认端口号。

- 参数:
   - `t`: 句柄。
   - `port`: 端口号。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_connect
`int transport_connect(transport_handle_t t, const char *host, int port, int timeout_ms);`

- 功能描述:
   - 传输连接功能，用于与服务器建立连接。

- 参数:
   - `t`: 句柄。
   - `host`: 主机名。
   - `port`: 端口号。
   - `timeout_ms`: 超时时间（毫秒）。

- 返回值:
   - socket: 成功。
   - -1: 失败。

### transport_connect_async
`int transport_connect_async(transport_handle_t t, const char *host, int port, int timeout_ms);`

- 功能描述:
   - 非阻塞传输连接功能，与服务器建立连接。

- 参数:
   - `t`: 句柄。
   - `port`: 端口号。

- 返回值:
   - socket: 成功。
   - -1: 失败。

### transport_read
`int transport_read(transport_handle_t t, char *buffer, int len, int timeout_ms);`

- 功能描述:
   - 传输读取功能。

- 参数:
   - `t`: 句柄。
   - `buffer`: 缓冲数据。
   - `len`: 数据长度。
   - `timeout_ms`: 超时时间（毫秒）。

- 返回值:
   - 大于0: 读取的字节数。
   - -1: 失败。

### transport_poll_read
`int transport_poll_read(transport_handle_t t, int timeout_ms);`

- 功能描述:
   - 轮询传输直到可读或超时。

- 参数:
   - `t`: 句柄。
   - `timeout_ms`: 超时时间（毫秒）。

- 返回值:
   - 0: 超时。
   - -1: 失败。
   - other: 传输可以读取。

### transport_write
`int transport_write(transport_handle_t t, const char *buffer, int len, int timeout_ms);`

- 功能描述:
   - 传输写函数。

- 参数:
   - `t`: 句柄。
   - `buffer`: 缓冲数据。
   - `len`: 数据长度。
   - `timeout_ms`: 超时时间（毫秒）。

- 返回值:
   - 大于0: 写入的字节数。
   - -1: 失败。

### transport_poll_write
`int transport_poll_write(transport_handle_t t, int timeout_ms);`

- 功能描述:
   - 轮询传输直到可写或超时。

- 参数:
   - `t`: 句柄。
   - `timeout_ms`: 超时时间（毫秒）。

- 返回值:
   - 0: 超时。
   - -1: 失败。
   - other: 传输可以写入。

### transport_close
`int transport_close(transport_handle_t t);`

- 功能描述:
   - 传输关闭。

- 参数:
   - `t`: 句柄。

- 返回值:
   - 0: 成功。
   - -1: 失败。

### transport_get_context_data
`void *transport_get_context_data(transport_handle_t t);`

- 功能描述:
   - 获取此传输的用户数据上下文。

- 参数:
   - `t`: 句柄。

- 返回值:
   - 返回用户数据上下文。

### transport_get_payload_transport_handle
`transport_handle_t transport_get_payload_transport_handle(transport_handle_t t);`

- 功能描述:
   - 获取可直接访问此协议负载的基础协议的传输句柄（用于多次接收较长的消息）。

- 参数:
   - `t`: 句柄。

- 返回值:
   - 返回载荷传输句柄。

### transport_set_context_data
`web_err_t transport_set_context_data(transport_handle_t t, void *data);`

- 功能描述:
   - 设置此传输的用户上下文数据。

- 参数:
   - `t`: 句柄。
   - `data`: 用户数据上下文。

- 返回值:
   - WEB_OK: 成功。

### transport_set_func
`web_err_t transport_set_func(transport_handle_t t, connect_func _connect, io_read_func _read, io_func _write, trans_func _close, poll_func _poll_read, poll_func _poll_write, trans_func _destroy);`

- 功能描述:
   - 设置传输句柄的传输功能。

- 参数:
   - `t`: 句柄。
   - `_connect`: 连接函数指针。
   - `_read`: 读取函数指针。
   - `_write`: 写函数指针。
   - `_close`: 关闭函数指针。
   - `_poll_read`: 轮询读取函数指针。
   - `_poll_write`: 轮询写入函数指针。
   - `_destroy`: 销毁函数指针。

- 返回值:
   - WEB_OK: 成功。

### transport_set_async_connect_func
`web_err_t transport_set_async_connect_func(transport_handle_t t, connect_async_func _connect_async_func);`

- 功能描述:
   - 传输关闭。

- 参数:
   - `t`: 句柄。
   - `_connect_async_func`: connect_async函数指针。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_set_parent_transport_func
`web_err_t transport_set_parent_transport_func(transport_handle_t t, payload_transfer_func _parent_transport);`

- 功能描述:
   - 将父传输函数设置为句柄。

- 参数:
   - `t`: 句柄。
   - `_parent_transport`: 底层传输getter指针。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_ssl接口

### transport_ssl_init
`transport_handle_t transport_ssl_init();`

- 功能描述:
   - 创建新的SSL传输时，传输句柄必须释放 transport_destroy 回调。

- 参数:
   - 无。

- 返回值:
   - 返回分配的传输句柄，如果无法分配句柄，则为NULL。

### transport_ssl_set_cert_data
`void transport_ssl_set_cert_data(transport_handle_t t, const char *data, int len);`

- 功能描述:
   - 设置SSL证书数据（PEM格式）。

- 参数:
   - `t`: 句柄。
   - `data`: 数据。
   - `len`: 长度。

- 返回值:
   - 无。

### transport_ssl_enable_global_ca_store
`void transport_ssl_enable_global_ca_store(transport_handle_t t);`

- 功能描述:
   - 为SSL连接启用全局CA存储。

- 参数:
   - `t`: 句柄。

- 返回值:
   - 无。

### transport_ssl_set_client_cert_data
`void transport_ssl_set_client_cert_data(transport_handle_t t, const char *data, int len);`

- 功能描述:
   - 为相互身份验证设置SSL客户端证书数据（PEM格式）。

- 参数:
   - `t`: 句柄。
   - `data`: 数据。
   - `len`: 长度。

- 返回值:
   - 无。

### transport_ssl_set_client_key_data
`void transport_ssl_set_client_key_data(transport_handle_t t, const char *data, int len);`

- 功能描述:
   - 为相互身份验证设置SSL客户端密钥数据（PEM格式）。

- 参数:
   - `t`: 句柄。
   - `data`: 数据。
   - `len`: 长度。

- 返回值:
   - 无。

### transport_tcp接口

### transport_utils_ms_to_timeval
`void transport_utils_ms_to_timeval(int timeout_ms, struct timeval *tv);`

- 功能描述:
   - 将毫秒转换为timeval结构。

- 参数:
   - `timeout_ms`: 超时时间。
   - `tv`: timeval指针。

- 返回值:
   - 无。

### transport_utils接口

### transport_utils_random
`void transport_utils_random(unsigned char *output, size_t output_len);`

- 功能描述:
   - 生成指定长度的随机数。

- 参数:
   - `output`: 数据指针。
   - `output_len`: 数据长度。

- 返回值:
   - 无。

### transport_ssl_init
`transport_handle_t transport_ssl_init();`

- 功能描述:
   - 将父传输函数设置为句柄。

- 参数:
   - `t`: 句柄。
   - `_parent_transport`: 底层传输getter指针。

- 返回值:
   - WEB_OK: 成功。
   - WEB_FAIL: 失败。

### transport_ws接口

### transport_ws_init
`transport_handle_t transport_ws_init(transport_handle_t parent_handle, const char *path, const char *protocol, int send_type, int buffer_size);`

- 功能描述:
   - 创建web套接字传输。

- 参数:
   - `parent_handle`: 句柄。
   - `path`: 底层传输getter指针。
   - `protocol`: 底层传输getter指针。
   - `send_type`: 底层传输getter指针。
   - `buffer_size`: 底层传输getter指针。

- 返回值:
   - transport句柄: 成功。
   - NULLL: 失败。

### transport_ws_set_path
`void transport_ws_set_path(transport_handle_t t, const char *path);`

- 功能描述:
   - 设置路径。

- 参数:
   - `t`: 句柄。
   - `path`: 路径。

- 返回值:
   - 无。

### transport_ws_set_sendtype
`void transport_ws_set_sendtype(transport_handle_t t, int send_type);`

- 功能描述:
   - 设置发送类型。

- 参数:
   - `t`: 句柄。
   - `send_type`: 发送类型。

- 返回值:
   - 无。

## 示例

包含头文件：

```C
#include <transport.h>
```

- 初始化：

```C
client->transport_list = transport_list_init();
tcp = transport_tcp_init();
transport_set_default_port(tcp, DEFAULT_HTTP_PORT);
transport_list_add(client->transport_list, tcp, "http");
```

- 常规操作：

```C
// 连接socket
transport_connect(tcp, "https://www.howsmyssl.com", DEFAULT_HTTP_PORT, 5000);
// 读
transport_read(tcp, buffer, 1024, 5000);
// 写
transport_write(tcp, buffer, 1024, 5000);
//关闭socket
transport_close(tcp);
```

- 销毁：

```C
transport_list_clean(client->transport_list);
transport_destroy(tcp);
```

## 诊断错误码
| 错误码 | 错误码说明 |
| :--- | :--- |
| WEB_OK | 成功 |
| WEB_FAIL | 失败 |
| WEB_ERR_NO_MEM | 内存不足 |
| WEB_ERR_INVALID_ARG | 无效参数 |
| WEB_ERR_INVALID_STATE | 无效状态 |
| WEB_ERR_INVALID_SIZE | 无效大小 |
| WEB_ERR_NOT_FOUND | 找不到请求的资源 |
| WEB_ERR_NOT_SUPPORTED | 不支持操作或功能 |
| WEB_ERR_TIMEOUT | 操作超时 |
| WEB_ERR_INVALID_RESPONSE | 收到的响应无效 |
| WEB_ERR_INVALID_CRC | CRC或校验和无效 |
| WEB_ERR_INVALID_VERSION | 版本无效 |
| WEB_ERR_INVALID_MAC | MAC地址无效 |
| WEB_ERR_WIFI_BASE | WiFi错误代码的起始数目 |
| WEB_ERR_MESH_BASE | MESH错误代码的起始数目 |

## 运行资源
无。

## 依赖资源

  - csi
  - aos

## 组件参考
无。
