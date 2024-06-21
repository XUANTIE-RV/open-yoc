# 概述
secure_demo是sec_crypto组件的测试例程，其中测试将包括基础接口、算法接口、密钥存储接口、密钥导入接口的功能测试。
rng          test pass
sha256       test pass
sha1         test pass
hmacsha256   test pass

# 使用
## CDK
在CDK的首页，通过搜索secure_demo，可以找到secure_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook) 章节

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install secure_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

#### bl606P平台

1. 编译

```bash
make clean
make SDK=sdk_chip_bl606p_e907
```

2. 烧写

```bash
make flashall SDK=sdk_chip_bl606p_e907
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后系统会自动启动，串口会有打印输出。
