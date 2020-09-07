# 概述

transport对socket操作做了一层封装，管理多个连接。为`httpclient`和`websocket`提供统一的接口。

# 示例

包含头文件：

```C
#include <transport.h>
```

初始化：

```C
client->transport_list = transport_list_init();
tcp = transport_tcp_init();
transport_set_default_port(tcp, DEFAULT_HTTP_PORT);
transport_list_add(client->transport_list, tcp, "http");
```

常规操作：

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

销毁：

```C
transport_list_clean(client->transport_list);
transport_destroy(tcp);
```