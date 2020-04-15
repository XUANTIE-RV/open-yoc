## 简介

Opus是一个完全开放的、免费的、多功能的音频编解码器。 它在交互式的语音和音乐在互联网中的传输方面有着无与伦比的优势，但是同样致力于存储和流媒体应用程序。它是由互联网工程任务组(IETF)制定的标准，标准格式为RFC 6716，由Skype的SILK编解码器和Xiph.Org的CELT编解码器合并发展而来，号称音频编解码器中的瑞士军刀。

Opus可以处理广泛的音频应用程序，包括IP电话、视频会议、游戏内聊天、甚至远程现场音乐表演。从低比特率窄带语音到非常高质量的立体声音乐，它都可以适用。

官方网址：http://opus-codec.org/

Opus具有如下特点：

- 6 kb /s到510 kb / s的比特率
- 采样率从8 kHz（窄带）到48 kHz（全频）
- 帧大小从2.5毫秒到60毫秒
- 支持恒定比特率（CBR）和可变比特率（VBR）
- 从窄带到全频段的音频带宽
- 支持语音和音乐
- 支持单声道和立体声
- 支持多达255个频道（多数据流的帧）
- 可动态调节比特率，音频带宽和帧大小
- 良好的鲁棒性丢失率和数据包丢失隐藏（PLC）
- 浮点和定点实现

## 如何使用

### libopus库的编译

- 下载opus-1.2.1.tar.gz并解压。
- 执行build脚本，编译并安装到当前目录output文件夹下。build脚本如下：

```c
#!/bin/sh                                                                                                                                                                          
CROSS_COMPILE=
ARCH=

TARGET_DIR=$(cd `dirname $0`; pwd)/output
CURRENT_DIR=$(cd `dirname $0`; pwd)

build_opus() {
    cd opus-1.2.1
    ./configure --prefix=$TARGET_DIR --disable-asm --disable-rtcd  --disable-intrinsics --enable-fixed-point \
        CFLAGS="-g -O0" \
        CC=$CROSS_COMPILE\gcc \
        STRIP=$CROSS_COMPILE\strip \
        RANLIB=$CROSS_COMPILE\ranlib \
        OBJDUMP=$CROSS_COMPILE\objdump \
        NM=$CROSS_COMPILE\nm \
        AR=$CROSS_COMPILE\ar \
        AS=$CROSS_COMPILE\as ;

    make ; make install || exit 1
    cd -
}

build_opus
```

- 编译出的结果如下：

```c
yingc@yingc:~/thirdparty/opus/opus/output$ tree
.
├── include
│   └── opus
│       ├── opus_defines.h
│       ├── opus.h
│       ├── opus_multistream.h
│       └── opus_types.h
├── lib
│   ├── libopus.a
│   ├── libopus.la
│   ├── libopus.so
│   ├── libopus.so.0
│   ├── libopus.so.0.6.1
│   └── pkgconfig
│       └── opus.pc
└── share
    └── aclocal
        └── opus.m4

6 directories, 11 files
yingc@yingc:~/thirdparty/opus/opus/output$ 
```

### opus编解码示例

- 示例中对一段16k/mono/16bits的pcm先做编码再解码。主要代码如下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opus.h"
#include "opus_defines.h"
#include <unistd.h>
#include <sys/time.h>

#include "hello_raw.h"
#include "hello_opus.h"

#define PCM_SIZE_MAX  (4*1024*1024)
#define OPUS_SIZE_MAX (1*1024*1024)

int main()
{
    int ret1, ret2;
    int err, per_frame, pcm_count = 0, opus_len = 0;
    int raw_len, cnt = 0, enc_size, raw_offset = 0, remain, frame_total, frame_cnt = 0, channel = 1;
    int frame_sum = 0;
    char *raw, *opus;
    OpusEncoder *enc = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &err);
    OpusDecoder *dec = opus_decoder_create(16000, 1, &err);

    raw = malloc(PCM_SIZE_MAX);
    memset(raw, 0, PCM_SIZE_MAX);
    opus = malloc(OPUS_SIZE_MAX);
    memset(opus, 0, OPUS_SIZE_MAX);

    raw_len = hello_raw_len;
    frame_total = hello_raw_len/sizeof(short)/channel;
    remain = frame_total;
    per_frame = ((16000/1000) * 2.5);//以2.5ms为帧间隔
    enc_size = (hello_raw_len > per_frame) ? per_frame : hello_raw_len;
    frame_cnt = (frame_total > per_frame) ? per_frame : frame_total;
    while (remain > 0) {
        ret1 = opus_encode(enc, hello_raw + raw_offset, frame_cnt, opus + opus_len, OPUS_SIZE_MAX - opus_len);//先做编码，编码后的数据存放在opus内存中
        if (ret1 <= 0) {
            break;
        }
        frame_sum += frame_cnt;
        raw_offset += frame_cnt * sizeof(short) * channel;
        remain = frame_total - frame_sum;
        frame_cnt = (remain > per_frame) ? per_frame : remain;
        
#if 1
        {
            ret2 = opus_decode(dec, opus + opus_len, ret1, raw + pcm_count, PCM_SIZE_MAX - pcm_count, 0);//紧接着做解码，解码后的数据存放在raw内存中
            if (ret2 <= 0) {
                break;
            }
            pcm_count += ret2 * sizeof(short) * channel;
        }
        //printf("=======>>>[%5d]:ret1 = %d, ret2 = %d\n", cnt++, ret1, ret2);
#endif
        opus_len += ret1;
    }
    free(raw);
    free(opus);

    opus_encoder_destroy(enc);
    opus_decoder_destroy(dec);

    return 0;
}

```