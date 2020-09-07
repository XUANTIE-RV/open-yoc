# 概述

websocket是一个用C实现的轻量级的websocket客户端，支持TLS。

# 示例代码

## 创建并连接一个websocket
```c
static rws_socket _socket = NULL;
static int state_flags = 0;

static void on_socket_received_text(rws_socket socket, const char *text, const unsigned int length, bool is_finish)
{
    char *buff = NULL;

    if (!socket || !text || !length) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)aos_malloc(length + 1);
    if (!buff) {
        DBG("%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    state_flags &= (~WEBSOCKET_DATA_NOT_RECVED);

    memcpy(buff, text, length);
    buff[length] = 0;

    DBG("%s: Socket text: %s", __FUNCTION__, buff);
    aos_free(buff);
    buff = NULL;
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length, bool is_finish)
{
    char *buff = NULL;

    if (!socket || !data || !length) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)aos_malloc(length + 1);
    if (!buff) {
        DBG("%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    state_flags &= ~WEBSOCKET_DATA_NOT_RECVED;

    memcpy(buff, data, length);
    buff[length] = 0;

    DBG("%s: Socket bin: \n%s", __FUNCTION__, buff);
    aos_free(buff);
    buff = NULL;
}

static void on_socket_received_pong(rws_socket socket)
{
    if (!socket) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }
    DBG("received pong!!!!!!!!!!!");
}

static void on_socket_connected(rws_socket socket)
{
    const char * test_send_text =
        "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";

    DBG("%s: Socket connected", __FUNCTION__);

    state_flags |= WEBSOCKET_CONNECTED;
    state_flags &= ~WEBSOCKET_DISCONNECTED;
    rws_socket_send_text(socket, test_send_text);
}

static void on_socket_disconnected(rws_socket socket)
{
    rws_error error = rws_socket_get_error(socket);
    if (error) {
        DBG("%s: Socket disconnect with code, error: %i, %s",
            __FUNCTION__,
            rws_error_get_code(error),
            rws_error_get_description(error));
    }

    state_flags &= ~WEBSOCKET_CONNECTED;
    state_flags |= WEBSOCKET_DISCONNECTED;
    _socket = NULL;
}


int websoc_cli_test_int(const char *scheme, const char *host,
                        const char *path, const int port,
                        const char *cert)
{
    int sleep_count = 0;

    if (!scheme || !host || !path) {
        DBG("%s: Invalid parameter(s).", __FUNCTION__);
        return -1;
    }

    if (_socket) {
        DBG("%s: Socket is not closed.", __FUNCTION__);
        return -2;
    }

    _socket = rws_socket_create(); // create and store socket handle
    CHECK_RET_WITH_RET(_socket, -1);

    state_flags = 0;
    state_flags |= WEBSOCKET_DATA_NOT_RECVED;

    rws_socket_set_scheme(_socket, scheme);
    rws_socket_set_host(_socket, host);
    rws_socket_set_path(_socket, path);
    rws_socket_set_port(_socket, port);

#ifdef WEBSOCKET_SSL_ENABLE
    if (cert) {
        rws_socket_set_server_cert(_socket, cert, strlen(cert) + 1);
    }
#endif

    rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
    rws_socket_set_on_connected(_socket, &on_socket_connected);
    rws_socket_set_on_received_text(_socket, &on_socket_received_text);
    rws_socket_set_on_received_bin(_socket, &on_socket_received_bin);
    rws_socket_set_on_received_pong(_socket, &on_socket_received_pong);

    rws_socket_connect(_socket);

    /* Connecting */
    while ((!(state_flags & WEBSOCKET_CONNECTED)) &&
           (!(state_flags & WEBSOCKET_DISCONNECTED))) {
        rws_thread_sleep(1000);
        sleep_count++;
        if (30 == sleep_count) {
            break;
        }
    }

    /* Receiving data */
    sleep_count = 0;
    if (state_flags & WEBSOCKET_CONNECTED) {
        while ((state_flags & WEBSOCKET_DATA_NOT_RECVED) && _socket &&
               (rws_true == rws_socket_is_connected(_socket))) {
            rws_thread_sleep(1000);
            sleep_count++;
            if (20 == sleep_count) {
                break;
            }
        }
    }

    if (_socket) {
        rws_socket_disconnect_and_release(_socket);
    }

    _socket = NULL;

    return state_flags & WEBSOCKET_DATA_NOT_RECVED ? -3 : 0;
}

```

# 使用说明

## 创建一个websocket并获得句柄
```c
  // Define variable or field for socket handle
  rws_socket _socket = NULL;
  ............
  // Create socket object
  _socket = rws_socket_create();
```
## 设置websocket连接的url
```c
// Combined url: "ws://echo.websocket.org:80/"
rws_socket_set_scheme(_socket, "ws");
rws_socket_set_host(_socket, "echo.websocket.org");
rws_socket_set_port(_socket, 80);
rws_socket_set_path(_socket, "/");
```

## 设置websocket的监听回调函数
警告: ```rws_socket_set_on_disconnected``` 这个回调是必须设置的
```c
// 主要的回调函数
// socket断开时调用
static void on_socket_disconnected(rws_socket socket) {
  // process error
  rws_error error = rws_socket_get_error(socket);
  if (error) { 
    printf("\nSocket disconnect with code, error: %i, %s", rws_error_get_code(error), rws_error_get_description(error)); 
  }
  // forget about this socket object, due to next disconnection sequence
  _socket = NULL;
}
// socket连接，握手成功触发调用
static void on_socket_connected(rws_socket socket) {
  printf("\nSocket connected");
}
// 接收到文本信息时触发调用
static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length) {
  printf("\nSocket text: %s", text);
}
..................
// 设置socket回调函数
rws_socket_set_on_disconnected(_socket, &on_socket_disconnected); // 必须的
rws_socket_set_on_connected(_socket, &on_socket_connected);
rws_socket_set_on_received_text(_socket, &on_socket_received_text);
```

## 连接
```c
rws_socket_connect(_socket);
```

### 发送消息
```c
const char * example_text =
  "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";
rws_socket_send_text(_socket, example_text);
```

## 断开连接，删除websocket
```c
rws_socket_disconnect_and_release(_socket);
_socket = NULL;
```