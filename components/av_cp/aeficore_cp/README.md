## 简介

aeficore_cp，即核间音效处理器aeficore(inter-core audio effecter)与之匹配的协处理器(cp)库。该库在cp侧监听ap侧发来的aef核间处理请求，屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构audio effecter运算能力。

###  核间aef使用示例

```c

void main()
{
    /* 初始化核间aef cp侧代码，初始化后即在cp侧监听ap侧的aef请求 */
	return aeficore_cp_init();
}

```

