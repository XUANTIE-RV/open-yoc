## 简介

resicore_cp，即核间音频重采样resicore(inter-core equalizer)与之匹配的协处理器(cp)库。该库在cp侧监听ap侧发来的重采样核间处理请求，屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构重采样运算能力。

###  核间重采样使用示例

```c
void main()
{
    /* 初始化核间重采样 cp侧代码，初始化后即在cp侧监听ap侧的重采样请求 */
	return resicore_cp_init();
}

```

