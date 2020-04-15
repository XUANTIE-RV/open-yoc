# 概述
microcoap是一个可运行在微控制器的小型coap服务端
## COAP协议简介
Coap（Constrained Application Protocol）是一种在物联网世界的类web协议，它的详细规范定义在 RFC 7252。COAP名字翻译来就是“受限应用协议”，顾名思义，使用在资源受限的物联网设备上。物联网设备的ram，rom都通常非常小，运行TCP和HTTP是不可以接受的。
## COAP协议特点
- COAP协议网络传输层由TCP改为UDP。
- 它基于REST，server的资源地址和互联网一样也有类似url的格式，客户端同样有POST，GET,PUT,DELETE方法来访问server，对HTTP做了简化。
- COAP是二进制格式的，HTTP是文本格式的，COAP比HTTP更加紧凑。
- 轻量化，COAP最小长度仅仅4B，一个HTTP的头都几十个B了。
- 支持可靠传输，数据重传，块传输。 确保数据可靠到达。
- 支持IP多播, 即可以同时向多个设备发送请求。
- 非长连接通信，适用于低功耗物联网场景。
# 示例代码
```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>

#include "coap.h"

#define PORT 5683

int main(int argc, char **argv)
{
    int fd;
#ifdef IPV6
    struct sockaddr_in6 servaddr, cliaddr;
#else /* IPV6 */
    struct sockaddr_in servaddr, cliaddr;
#endif /* IPV6 */
    uint8_t buf[4096];
    uint8_t scratch_raw[4096];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

#ifdef IPV6
    fd = socket(AF_INET6,SOCK_DGRAM,0);
#else /* IPV6 */
    fd = socket(AF_INET,SOCK_DGRAM,0);
#endif /* IPV6 */

    bzero(&servaddr,sizeof(servaddr));
#ifdef IPV6
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(PORT);
#else /* IPV6 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
#endif /* IPV6 */
    bind(fd,(struct sockaddr *)&servaddr, sizeof(servaddr));

    endpoint_setup();

    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(cliaddr);
        coap_packet_t pkt;

        n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &len);
#ifdef DEBUG
        printf("Received: ");
        coap_dump(buf, n, true);
        printf("\n");
#endif

        if (0 != (rc = coap_parse(&pkt, buf, n)))
            printf("Bad packet rc=%d\n", rc);
        else
        {
            size_t rsplen = sizeof(buf);
            coap_packet_t rsppkt;
#ifdef DEBUG
            coap_dumpPacket(&pkt);
#endif
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            if (0 != (rc = coap_build(buf, &rsplen, &rsppkt)))
                printf("coap_build failed rc=%d\n", rc);
            else
            {
#ifdef DEBUG
                printf("Sending: ");
                coap_dump(buf, rsplen, true);
                printf("\n");
#endif
#ifdef DEBUG
                coap_dumpPacket(&rsppkt);
#endif

                sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            }
        }
    }
}
```
# 参考文档
[rfc7252](http://tools.ietf.org/html/rfc7252)
