# 超文本传输协议（HTTP）

## 概述

HTTP(HyperText Transfer Protocol)是一款用于传输超文本的应用层协议。YOC系统的网络组件包含了HTTP组件，提供HTTP客户端标准能力。开发者可以通过组件API，在设备端快速实现与HTTP服务端的数据交互。

- HTTP Core：HTTP核心模块，主要包括连接建立、请求创建、请求发送、回复接收、回复解析。 该模块实现HTTP主流程，对外提供标准的HTTP协议行为。 
- Method API：请求方法接口模块，主要包括get、post、put、delete等基本请求方法接口。该模块封装了HTTP核心模块的发送请求、接收响应的过程，以简化用户的调用。 

## 组件安装

```bash
yoc init
yoc install http
```

## 配置
无。

## 接口列表

http接口如下所示：

| 函数 | 说明 |
| :--- | :--- |
| httpclient_get | 发起GET请求，并接收响应 |
| httpclient_head | 发起HEAD请求，并接收响应 |
| httpclient_post | 发起POST请求，并接收响应 |
| httpclient_put | 发起PUT请求，并接收响应 |
| httpclient_delete | 发起DELETE请求，并接收响应 |
| httpclient_prepare | 分配HTTP请求头部缓存和响应缓存 |
| httpclient_unprepare | 释放HTTP请求头部缓存和响应缓存 |
| httpclient_reset | 重置HTTP请求头部缓存和响应缓存 |
| httpclient_conn | 发起HTTP连接 |
| httpclient_send | 发送HTTP请求 |
| httpclient_recv | 接收HTTP响应 |
| httpclient_clse | 关闭HTTP连接 |
| httpclient_set_custom_header | 设置HTTP用户定义头部  |
| httpclient_get_response_code | 获取HTTP响应码 |
| httpclient_get_response_header_value | 获取HTTP响应头部字段值 |
| httpclient_formdata_addtext | 添加文本form data |
| httpclient_formdata_addfile | 添加文件form data |

## 接口详细说明

### 枚举定义

#### HTTP_REQUEST_TYPE
| 类型 | 说明 |
| :--- | :--- |
| HTTP_GET | GET |
| HTTP_POST | POST |
| HTTP_PUT | PUT |
| HTTP_DELETE | DELETE |
| HTTP_HEAD | HEAD |

### 结构体定义

#### httpclient_data_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| is_more | bool | 指示是否需要接收更多数据 |
| is_chunked | bool | 指示收到的数据是否分块 |
| retrieve_len | int | 需要获取的总长度 |
| response_content_len | int | 响应内容的长度 |
| content_block_len | int | 单块长度 |
| post_buf_len | int | post数据缓存大小 |
| response_buf_len | int | 响应主体缓存大小 |
| header_buf_len | int | 响应头部缓存大小 |
| post_content_type | char* | post数据类型 |
| post_buf | char* | post数据缓存地址 |
| response_buf | char* | 响应主体缓存地址 |
| header_buf | char* | 响应头部缓存地址 |
| is_redirected | bool | 是否重定向URL？如果1，则有重定向url；如果0，则没有重定向url |
| redirect_url | char* | 重定向url |

#### httpclient_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| socket | int | 套接字标识 |
| remote_port | int | 端口 |
| response_code | int | 响应码 |
| header | char* | HTTP请求头部 |
| auth_user | char* | 基本身份验证用户名 |
| auth_password | char* | 基本身份验证密码 |
| is_http | bool | 是否是http连接？如果1，http；如果0，https |
| server_cert | const char* | 服务端证书地址 |
| client_cert | const char* | 客户端证书地址 |
| client_pk | const char* | 客户端私钥 |
| server_cert_len | int | 服务端证书长度 |
| client_cert_len | int | 客户端证书长度 |
| client_pk_len | int | 客户端私钥长度 |
| ssl | void* | ssl上下文地址 |

接口的具体形式如下：

### httpclient_get
`HTTPC_RESULT httpclient_get(httpclient_t *client, const char *url, httpclient_data_t *client_data);`

- 功能描述:
   - 发起GET请求，并接收响应。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 请求的url。
   - `client_data`: 请求返回的数据。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_head
`HTTPC_RESULT httpclient_head(httpclient_t *client, const char *url, httpclient_data_t *client_data);`

- 功能描述:
   - 发起HEAD请求，并接收响应。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 请求的url。
   - `client_data`: 请求返回的数据。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_post
`HTTPC_RESULT httpclient_post(httpclient_t *client, const char *url, httpclient_data_t *client_data);`

- 功能描述:
   - 发起POST请求，并接收响应。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 请求的url。
   - `client_data`: 请求返回的数据。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_put
`HTTPC_RESULT httpclient_put(httpclient_t *client, const char *url, httpclient_data_t *client_data);`

- 功能描述:
   - 发起PUT请求，并接收响应。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 请求的url。
   - `client_data`: 请求返回的数据。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_delete
`HTTPC_RESULT httpclient_delete(httpclient_t *client, const char *url, httpclient_data_t *client_data);`

- 功能描述:
   - 发起DELETE请求，并接收响应。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 请求的url。
   - `client_data`: 请求返回的数据。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_prepare
`HTTPC_RESULT httpclient_prepare(httpclient_data_t *client_data, int header_size, int resp_size);`

- 功能描述:
   - 分配HTTP请求头部缓存和响应缓存。

- 参数:
   - `client_data`: 指向httpclient_data_t的指针。
   - `header_size`: 头缓冲区大小。
   - `resp_size`: 响应缓冲区大小。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_unprepare
`HTTPC_RESULT httpclient_unprepare(httpclient_data_t *client_data);`

- 功能描述:
   - 释放HTTP请求头部缓存和响应缓存。

- 参数:
   - `client_data`: 指向httpclient_data_t的指针。
 
- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_reset
`void httpclient_reset(httpclient_data_t *client_data);`

- 功能描述:
   - 重置HTTP请求头部缓存和响应缓存。

- 参数:
   - `client_data`: 指向httpclient_data_t的指针。

- 返回值:
   - 无。

### httpclient_conn
`HTTPC_RESULT httpclient_conn(httpclient_t *client, const char *url);`

- 功能描述:
   - 发起HTTP连接。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 远程URL。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_send
`HTTPC_RESULT httpclient_send(httpclient_t *client, const char *url, int method, httpclient_data_t *client_data);`

- 功能描述:
   - 发送HTTP请求。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `url`: 远程URL。
   - `method`: http请求方法。
   - `client_data`: 指向httpclient_data_t的指针。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_recv
`HTTPC_RESULT httpclient_recv(httpclient_t *client, httpclient_data_t *client_data);`

- 功能描述:
   - 接收HTTP响应。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `client_data`: 指向httpclient_data_t的指针。

- 返回值:
   - 参考HTTPC_RESULT枚举定义。

### httpclient_clse
`void httpclient_clse(httpclient_t *client);`

- 功能描述:
   - 关闭HTTP连接。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。

- 返回值:
   - 无。

### httpclient_set_custom_header
`void httpclient_set_custom_header(httpclient_t *client, char *header);`

- 功能描述:
   - 设置HTTP用户定义头部。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。
   - `header`: 自定义的头字符串。

- 返回值:
   - 无。

### httpclient_get_response_code
`int httpclient_get_response_code(httpclient_t *client);`

- 功能描述:
   - 获取HTTP响应码。

- 参数:
   - `client`: HTTP client上下文，包含配置参数，如服务端口号、服务端证书等。

- 返回值:
   - 返回请求的响应码。

### httpclient_get_response_header_value
`int httpclient_get_response_header_value(char *header_buf, char *name, int *val_pos, int *val_len);`

- 功能描述:
   - 获取HTTP响应头部字段值。

- 参数:
   - `header_buf`: 响应头缓冲区。
   - `name`: http响应头名称。
   - `val_pos`: 响应头在缓冲区的位置。
   - `val_len`: 响应头值的长度。

- 返回值:
   - 0: 成功。
   - 非0: 失败。

### httpclient_formdata_addtext
`int httpclient_formdata_addtext(httpclient_data_t* client_data, char* content_disposition, char* content_type, char* name, char* data, int data_len);`

- 功能描述:
   - 添加文本form data。

- 参数:
   - `client_data`: 指向httpclient_data_t的指针。
   - `content_disposition`: 指向内容处置字符串的指针。
   - `content_type`: 指向内容类型字符串的指针。
   - `name`: 指向名称字符串的指针。
   - `data`: 数据指针。
   - `data_len`: 数据长度。

- 返回值:
   - 返回请求的响应码。

### httpclient_formdata_addtext
`int httpclient_formdata_addfile(httpclient_data_t* client_data, char* content_disposition, char* name, char* content_type, char* file_path);`

- 功能描述:
   - 添加文件form data。

- 参数:
   - `client_data`: 指向httpclient_data_t的指针。
   - `content_disposition`: 指向内容处置字符串的指针。
   - `name`: 指向名称字符串的指针。
   - `content_type`: 指向内容类型字符串的指针。
   - `file_path`: 文件路径。

- 返回值:
   - 返回请求的响应码。

## 示例

```c
#include <aos/aos.h>

static char* g_url;

void test()
{
#define REQ_BUF_SIZE 2048
    static char req_buf[REQ_BUF_SIZE];
#define RSP_BUF_SIZE 2048
    static char rsp_buf[RSP_BUF_SIZE];
    httpclient_t client = { 0 };
    httpclient_data_t client_data = {0};
    int ret, total_len = 0;
    int recv_len = 0, recv_total_len = 0;
    char * customer_header = "Accept: */*\r\n";
    long long start = aos_now_ms();

    memset(req_buf, 0, sizeof(req_buf));
    client_data.header_buf = req_buf;
    client_data.header_buf_len = sizeof(req_buf);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    ret = httpclient_conn(&client, g_url);
    if(HTTP_SUCCESS != ret ) {
        LOGE(TAG, "http connect failed");
        return;
    }

    httpclient_set_custom_header(&client, customer_header);

    ret = httpclient_send(&client, g_url, HTTP_GET, &client_data);
    if(HTTP_SUCCESS != ret) {
        LOGE(TAG, "http send request failed");
        return;
    }

    while (total_len == 0 || recv_total_len < total_len) {
        ret = httpclient_recv(&client, &client_data);
        if (ret == HTTP_SUCCESS || ret == HTTP_EAGAIN) {
            recv_len = client_data.content_block_len;
            /*
             * Fetch data from client_data.response_buf
             * according to recv_len
             */
        } else {
            recv_len = 0;
        }

        if (ret < 0) {
            break;
        }

        /* first time */
        if (total_len <= 0) {
            int val_pos, val_len;
            if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-Length", (int *)&val_pos, (int *)&val_len)) {
                sscanf(client_data.header_buf + val_pos, "%d", &total_len);
            }
        }

        recv_total_len += recv_len;

        LOGI(TAG, "Total recv %d / %d \n", recv_total_len, total_len);
    }

    LOGI(TAG,"Finished. Duration %d ms\n", (int)(aos_now_ms() - start));

    httpclient_clse(&client);
    aos_freep(&g_url);
    return;
}
```

## 诊断错误码
| 错误码 | 错误码说明 |
| :--- | :--- |
| HTTP_EAGAIN | 要检索的更多数据 |
| HTTP_SUCCESS | 操作成功 |
| HTTP_ENOBUFS | 缓存错误 |
| HTTP_EARG | 非法参数 |
| HTTP_ENOTSUPP | 不支持 |
| HTTP_EDNS | DNS解析错误 |
| HTTP_ECONN | 连接失败 |
| HTTP_ESEND | 发送数据失败 |
| HTTP_ECLSD | 连接关闭 |
| HTTP_ERECV | 接收数据失败 |
| HTTP_EPARSE | url解析错误 |
| HTTP_EPROTO | 协议错误 |
| HTTP_EUNKOWN | 未知错误 |
| HTTP_ETIMEOUT | 超时 |

## 运行资源
无。

## 依赖资源

  - csi
  - aos

## 组件参考
无。
