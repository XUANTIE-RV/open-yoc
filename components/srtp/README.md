# 概述


libsrtp是一个实现SRTP协议(Secure Real-time Transport Protocol，安全实时传输协议)的库。RTP 是实时传输协议，它是由 RFC 3550 定义的实时数据传输( 例如电话。音频和视频)的标准。 安全 RTP ( SRTP ) 是一个RT P配置文件，用于为RTP数据和对RTP报头和负载的认证提供机密性。 SRTP是在 RFC 3711 中定义的，它是在IETF的音频/视频传输( AVT ) 工作组中定义的。 这里库支持SRTP的所有强制功能，但并非所有可选功能。 有关详细信息，请参阅支持的功能部分。


# 示例代码
本节提供了如何使用libSRTP的简单示例。 示例代码缺少错误检查，但它是功能。 这里我们假设值SSRC已经设置为描述我们发送的流的SSRC，并且函数 get_rtp_packet() 和 send_srtp_packet() 可以用。 前者将一个RTP数据包放入缓冲区，并返回写入该缓冲区的八位字节数。 后者在缓冲区中发送RTP数据包，它的长度为第二个参数

```c
srtp_t session;
srtp_policy_t policy;

// Set key to predetermined value
uint8_t key[30] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                   0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D};

// initialize libSRTP
srtp_init();

// default policy values
memset(&policy, 0x0, sizeof(srtp_policy_t));

// set policy to describe a policy for an SRTP stream
srtp_crypto_policy_set_rtp_default(&policy.rtp);
srtp_crypto_policy_set_rtcp_default(&policy.rtcp);
policy.ssrc = ssrc;
policy.key  = key;
policy.next = NULL;

// allocate and initialize the SRTP session
srtp_create(&session, &policy);

// main loop: get rtp packets, send srtp packets
while (1) {
  char rtp_buffer[2048];
  unsigned len;

  len = get_rtp_packet(rtp_buffer);
  srtp_protect(session, rtp_buffer, &len);
  send_srtp_packet(rtp_buffer, len);
}
```
