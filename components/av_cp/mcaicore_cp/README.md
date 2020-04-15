## 简介

mcaicore_cp，即核间mcaicore(inter-core mca)与之匹配的协处理器(cp)库。该库在cp侧监听ap侧发来的mca核间处理请求，屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构inter-core mca运算能力。

###  核间mca使用示例

```c

int app_main()
{
    /* 初始化核间mca cp侧代码，初始化后即在cp侧监听ap侧的mca请求 */
	return mcaicore_cp_init();
}

```

