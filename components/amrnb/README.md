## 简介

amrnb实现了AMR-NB语音格式的编解码.

AMR-NB 支持八种速率模式,分别为:模式0(4.75kbit/s)、模式1(5.15kbit/s)、模式2 (5.90kbit/s)、模式3(6.70kbit/s)、模式 4(7.40kbit/s)、模式 5(7.95kbit/s)、模式 6(10.2kbit/s)、模式 7(12.2kbit/s),其以更加智能的方式解决信源和信道编码的速率分配问题,根据无线信道和传输状况来自适应地选择一种编码模式进行传输,使得无线资源的配置与利用更加灵活有效。

## 相关接口

具体接口使用请参见gsmamr_dec.h中的说明。

## 参考资料

- [源码地址](https://github.com/gp-b2g/frameworks_base/tree/master/media/libstagefright/codecs/amrnb)


