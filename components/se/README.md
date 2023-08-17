# SE安全组件（se）

## 概述

se组件为安全组件，提供安全平台的安全算法抽象开发接口，实现常见的密码学算法，包含对称算法、非对称/公钥算法、摘要算法、消息验证码算法。并提供密钥管理、密钥存储等功能。

### 功能
- 对称算法
   - AES ECB/CBC
   - AES with 128 bit keywidth
- 非对称算法
   - RSA1024 
   - RSA2048
   - ECC256
- 摘要
   - SHA1、SHA224、SHA256
- 消息验证码
- 随机数


## 组件安装

```bash
yoc init
yoc install se
```


## 配置

无。