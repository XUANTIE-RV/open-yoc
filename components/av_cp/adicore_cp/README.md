## 简介

adicore_cp是一个支持mp3、aac等编码格式的多核异构核间解码库(inter-core audio decoder)。 该解码库运行在协处理器(cp)侧, 与adicore(运行在应用程序处理器侧)配合使用。该库屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构解码运算能力。

###  核间解码使用示例

```c
void main()
{
    /* 初始化核间解码cp侧代码，初始化后即在cp侧监听ap侧的解码请求 */
	return adicore_cp_init();
}

```

