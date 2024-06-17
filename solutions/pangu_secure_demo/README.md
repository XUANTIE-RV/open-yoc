# 修改

version 7.4  pangu_secure_demo该版本提供sec_crypto、mbedtls、key_mgr组件使用 演示，介绍以上各组件的基本用法。

# 参考

阅读以下章节前请先熟悉YoCBook以下内容：

- [CSI2接口](https://www.xrvm.cn/document?temp=csi&slug=csi-chn)

- [CDK](https://www.xrvm.cn/document?temp=use-cdk-to-get-started-quickly&slug=yocbook)

- [安全组件](https://www.xrvm.cn/document?temp=key_mgr&slug=yocbook)

- [芯片对接](https://www.xrvm.cn/document?temp=overview-4&slug=yocbook)


# 概述

`pangu_secure_demo` 提供基于pangu开发板的secure应用程序demo，用于介绍如何使用安全组件如sec_crypto、mbedtls、tee、key_mgr等。

# 代码下载
通过CDK下载pangu_secure_demo 应用程序。详细方法请参考yocbook CDK开发快速上手章节。

# 目录结构

```c
  app
├── include  				//应用程序头文件
└── src		 				//应用程序源代码
    ├── app_main.c			//应用入口文件
    ├── demo_key_mgr.c		//key_mgr demo程序
    ├── demo_mbedtls.c  	//mbedtls自测试程序
    ├── demo_sec_crypto.c   //sec_crypto demo程序。
└── package.yaml		 	//应用程序配置脚本

```

# 配置

该应用程序需要在package.yaml里添加以下配置：


  pangu_secure_demo配置：
-   打开自测试。
```bash
  MBEDTLS_SELF_TEST: 1 

```
- 打开mbedtls演示功能。
```bash
  CONFIG_SECURITY_DEMO_MBEDTLS: 1
```
- 打开sec_crypto演示功能
```bash
  CONFIG_SECURITY_DEMO_SEC_CRYPTO: 1
```
- 打开key_mgr演示功能
```bash
  CONFIG_SECURITY_DEMO_KEY_MGR: 1
```
key_mgr组件相关配置：

- KP格式密钥存储

```bash
  CONFIG_TB_KP: 1
```

# 编译

1. CLI方式：
```bash
make
```


2. CDK方式：

```bash
CDK: 菜单中选择并点击Project->Build Active Project
```

# 烧录
1. CLI方式：
```bash
make flashall
```

2. CDK方式：
```bash
CDK：菜单中选择并点击 Flash->Download
```

# 启动

烧录完成之后按复位键或者重新上电单板，串口会有命令行提示符提示输入演示命令，支持的演示命令如下：

- mbedtls演示命令，执行该命令后会调用mbedtls各算法自测试程序。
```bash
sdemo mbedtls
```
- sec_crypto演示命令，执行该命令后会调用sec_crypto组件SHA、AES、RSA、RNG等算法。
```bash
sdemo sec_crypto
```
- key_mgr演示命令
```bash
sdemo key_mgr
```

以上各命令执行成功后会有”passed“log，如果运行错误会有”failed“关键字log，
如sec_crypto rsa算法执行成功后会有如下log：

```bash
“secure_demo sec_crypto_rsa_demo, passed!”
```