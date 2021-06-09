# 简介

`test_kv` 是一个针对平头哥kv组件的测试应用。目的是验证对接是否成功。

# 操作手册

具体请参考:doc/kv测试操作手册.md

# 测试用例

具体请参考:kv测试用例集.xlsx

# 测试报告

具体请参考:kv测试报告.xlsx

# CDK IDE编译
由于test_kv工程支持不同的CHIP SDK切换， 所以必须使用CDK v2.7.x以上的版本进行编译和下载。否则，可能会有下载失败的现象。

# 命令行编译

```bash
make clean;make
```

## 指定SDK编译
比如选择sdk_chip_ch2201进行编译。

```bash
make SDK=sdk_chip_ch2201
```

# 烧录

```bash
make flashall
```

## 指定芯片SDK进行烧录
```bash
make flashall SDK=sdk_chip_ch2201

