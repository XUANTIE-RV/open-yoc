## 简介

eqicore_cp，即核间音效均衡器eqicore(inter-core equalizer)与之匹配的协处理器(cp)库。该库在cp侧监听ap侧发来的eq核间处理请求，屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构equalizer运算能力。

###  核间eq使用示例

```c

void main()
{
    /* 初始化核间eq cp侧代码，初始化后即在cp侧监听ap侧的eq请求 */
	return eqicore_cp_init();
}

```

