## 简介

atempo_icore_cp，即核间音效处理器atempo_icore(inter-core audio atempoer)与之匹配的协处理器(cp)库。该库在cp侧监听ap侧发来的atempo核间处理请求，屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构audio atempoer运算能力。

###  核间atempo使用示例

```c

void main()
{
    /* 初始化核间atempo cp侧代码，初始化后即在cp侧监听ap侧的atempo请求 */
	return atempo_icore_cp_init();
}

```

