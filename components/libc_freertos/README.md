@page libc_freertos libc_freertos

[更正文档](https://gitee.com/alios-things/libc_freertos/edit/rel_3.3.0/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
libc_freertos组件是对C库桩函数（stubs）的实现。
对接的功能包括：
- 内存申请释放：malloc/free/realloc/calloc
- 错误码errno记录、异常abort
- 文件系统相关open/read/write/close等
- 输出printf

该组件只对接C库相关接口。

## 版权信息
> Apache license v2.0

## 目录结构
```tree
├── newlib_stub.c # gcc newlibc库的重定向实现
├── compilers
│   ├── armlibc   # armcc库的重定向实现
│   ├── gcc       # gcc对接头文件修改
│   └── iar       # icc库的重定向实现
└── sub.mk        # 编译配置文件
```

# 常用配置
无

# API说明
参考标准C库接口即可。

