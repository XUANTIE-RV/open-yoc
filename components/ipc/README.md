# 概述

随着信息技术的发展与需求的提升，传统单核 SoC 不能满足需求，促进了同构多核与异构多核的发展，在异构多核框架中，整个系统中由多个不同的处理器、性能与用途格不相同的多个核心组成，各个核心发挥各自的计算优势，实现资源的最佳配置，很好的提升了整体性能，降低功耗，简化开发模式，因此多核间的通信机制，以及软件编程模型对多核性能的发挥尤为重要。本文介绍异构核间通信机制，考虑的因素有：

* 轻量级，可应用于 NOS / RTOS 的应用场景
* 简化各核心的开发模式
* 针对核间协同工作的特点，划分为：通用计算、并行计算、流式数据、帧式数据，应用于多种异构场景
* 可移植性，框架可以方便移植到 Linux，不同的 RTOS 中

异构多核处理器大多采用主从式的结构。主从式结构根据不同核的功能把处理器核分为主核和从核。主核的结构和功能一般较为复杂，负责全局资源、任务的管理和调度并完成从核的引导加载。从核主要接受主核的管理，负责运行主核分配的任务，并具有本地任务调度与管理功能。在多核处理器中，根据不同核的结构，各个核可运行相同或不同的操作系统。

# 示例代码 
- AP

```c
#define IPC_BUF_LEN (4096*2)

typedef struct {
    ipc_t *ipc;
    char data[IPC_BUF_LEN] __attribute__ ((aligned(64)));
} ipc_test_t;

ipc_test_t g_test[2];

#define TAG "AP"

int ipc_sync(void)
{
    message_t msg;

    msg.command = 103;
    msg.flag    = MESSAGE_SYNC;
    msg.service_id = 20;
    msg.resp_data = NULL;
    msg.resp_len = 0;

    int snd_len = 4096;
    int offset = 0;
    char *send = (char *)src_audio_music_raw;


    while (offset < src_audio_music_raw_len) {
        msg.req_data    = send + offset;

        snd_len = 4096 < (src_audio_music_raw_len - offset)? 4096 : (src_audio_music_raw_len - offset);
        msg.req_len     = snd_len;

        ipc_message_send(g_test[0].ipc, &msg, AOS_WAIT_FOREVER);

        offset += snd_len;
    }

    printf("ipc sync done\n");
    return 0;
}

// async message demo

static char *s[] = {
    "00000000",
    "11111111",
    "22222222",
    "33333333",
    "44444444",
    "55555555",
    "66666666",
    "77777777",
    "88888888",
    "99999999",
};
int ipc_async(void)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.command = 104;
    msg.flag   = MESSAGE_ASYNC;

    msg.service_id = 20;

    for (int i = 0; i < 100; i++) {
        msg.req_data    = s[i%10];
        msg.req_len     = strlen(s[i%10]) + 1;

        ipc_message_send(g_test[0].ipc, &msg, AOS_WAIT_FOREVER);
    }

    printf("ipc async done\n");

    return 0;
}

int ipc_server_init(void)
{
    ipc_test_t *i = &g_test[0];
    i->ipc = ipc_get(2);

    ipc_add_service(i->ipc, 20, NULL, NULL);

    return 0;
}
```

  

- CP

```c
#define TAG "CP"
#define IPC_BUF_LEN (1024)

typedef struct {
    ipc_t *ipc;
    char data[IPC_BUF_LEN] __attribute__ ((aligned(64)));
} ipc_test_t;

ipc_test_t g_test[2];

static char *s[] = {
    "00000000",
    "11111111",
    "22222222",
    "33333333",
    "44444444",
    "55555555",
    "66666666",
    "77777777",
    "88888888",
    "99999999",
};

static void cli_ipc_process(ipc_t *ipc, message_t *msg, void *priv)
{
    switch (msg->command) {

        case 104: {
            static int offset = 0;
            int ret = memcmp(s[offset % 10], msg->req_data, msg->req_len);

            offset ++;

            if (ret != 0) {
                printf("ipc async err!!!\n");
            }

            if (offset == 100) {
                printf("ipc async ok!!!\n");
                offset = 0;
            }
            break;
        }

        case 103: {
            char *music = (char *)src_audio_music_raw;
            static int music_len = 0;

            int ret = memcmp(music + music_len, msg->req_data, msg->req_len);
            music_len += msg->req_len;

            if (ret != 0) {
                printf("ipc sync err!!!\n");
            }
            if (music_len == src_audio_music_raw_len) {
                printf("music recv ok, total:(%d)\n", music_len);
                music_len = 0;
            }
            ipc_message_ack(ipc, msg, AOS_WAIT_FOREVER);

        }

        default :
            break;
    }
}

int ipc_cli_init(void)
{
    ipc_test_t *i = &g_test[1];
    i->ipc = ipc_get(0);

    ipc_add_service(i->ipc, 20, cli_ipc_process, i);

    return 0;
}

```
