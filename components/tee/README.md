# TEE

## 概述

可信执行环境（TEE，Trusted Execution Environment） 最早由Global Platform（GP）提出，是为解决设备的安全问题而设计的解决方案，该方案将运行环境分为安全世界TEE以及相对的非安全世界REE, 其中TEE执行安全应用TA，REE执行非安全应用，REE通过CA（Client Application）接口与TA通信，

YoC TEE组件主要包含三个模块，分别为：

• Client Application (CA)

• Trusted Application (TA)

• TEE Kernel
其中, CA提供了如何调用TA的相关安全服务，向用户提供了安全服务的调用接口，其运行在REE环境里。TEE Kernel模块负责TEE OS的客户端的命令解析和参数处理，以及对内部支持的TA的管理，比如查询、安全服务调用等。TA 实现了各业务场景下的安全应用服务，如安全算法库、安全存储等。



### 功能
- 对称算法
   - AES ECB/CBC
   - AES with 128/192/256 bit keywidth
- 非对称算法
   - RSA1024 
   - RSA2048
- 摘要
  
   - SHA1、SHA2
- 消息验证码

   - ＨＭＡＣ
- 随机数
- 设备信息获取
- 进入低功耗状态
- 

## 组件安装

TEE组件以binary形式随board及chip组件发布，开发者一般不需要安装，当需要修改TEE实现时才需要安装tee组件，组件安装命令如下：

```bash
yoc init
yoc install tee
```



## 配置

TEE组件功能可根据配置灵活裁剪，配置项如下：
- 配置打开设备ID获取功能
```bash
  CONFIG_DEV_CID: 1
```
- 配置打开SHA安全算法
```bash
  CONFIG_ALGO_SHA: 1
```
- 配置打开随机数功能
```bash
  CONFIG_ALGO_TRNG: 1
```
- 配置打开AEA安全算法功能
```bash
  CONFIG_ALGO_AES: 1
```
- 配置打开RSA安全算法功能
```bash
  CONFIG_ALGO_RSA: 1
```
- 配置打开设备运行频率功能
```bash
  CONFIG_SYS_FREQ: 1
```
- 配置打开BASE64算法功能
```bash
  CONFIG_ALGO_BASE64: 1
```
- 配置打开设备HMAC安全算法功能
```bash
  CONFIG_ALGO_HMAC: 1
```
- 配置打开低功耗功能
```bash
  CONFIG_ALGO_LPM: 1
```



## 接口列表

TEE以csi_tee_xxx的形式向开发者提供用户接口，使开发者不需要关心实现细节，接口列表如下所示：

| 函数 | 说明 |
| :--- | :--- |
| csi_tee_dev_info_get | 设备信息获取 |
| csi_tee_set_sys_freq | 设置系统频率 |
| csi_tee_get_sys_freq | 获取系统频率 |
| csi_tee_get_cid | 获取设备ID |
| csi_tee_enter_lpm | 进入低功耗模式 |
| csi_tee_base64 | Base64编解码接口 |
| csi_tee_base64_encode | Base64编码接口 |
| csi_tee_base64_decode | Base64解码接口 |
| csi_tee_hmac | HMAC安全算法 |
| csi_tee_hmac_digest | HMAC安全算法 |
| csi_tee_rand_generate | 随机数生成 |
| csi_tee_sha | SHA摘要算法 |
| csi_tee_sha_start | SHA摘要算法start接口 |
| csi_tee_sha_update | SHA摘要算法update接口 |
| csi_tee_sha_finish | SHA摘要算法finish接口 |
| csi_tee_aes_encrypt | AES加密安全算法 |
| csi_tee_aes_decrypt | AES解密安全算法 |
| csi_tee_aes_encrypt_ecb | AES ECB加密安全算法 |
| csi_tee_aes_decrypt_ecb | AES ECB解密安全算法 |
| csi_tee_aes_encrypt_cbc | AES CBC加密安全算法 |
| csi_tee_aes_decrypt_cbc | AES CBC解密安全算法 |
| csi_tee_rsa_sign | RSA 签名算法 |
| csi_tee_rsa_encrypt | RSA 加密安全算法|
| csi_tee_rsa_decrypt | RSA 解密算法 |



## 接口详细说明

```c
int32_t csi_tee_dev_info_get(const uint8_t *name_encrypted, uint32_t name_encrypted_len, const uint8_t *product_key_encrypted, uint32_t product_key_encrypted_len, const uint8_t *name, uint32_t *name_len,
const uint8_t *product_key, uint32_t *product_key_len);
```

- 参数
   - name_encrypted- 设备名字加密数据指针
   - name_encrypted_len- 设备名字加密数据长度
   - product_key_encrypted- 产品密钥加密数据指针
   - product_key_encrypted_len- 产品密钥加密数据长度
   - name- 设备名字指针
   - name_len- 设备名字长度
   - product_key - 产品密钥指针
   - product_key_len - 产品密钥长度
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_set_sys_freq(uint32_t clk_src, uint32_t clk_val);
```
- 参数
   - clk_src- 时钟源类型
   - clk_val- 时钟值
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_get_sys_freq(uint32_t *clk_val);
```
- 参数
   - clk_val- 时钟值指针
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_get_cid(uint8_t *out, uint32_t *out_len);
```
- 参数
   - out- CID输出数据
   - out_len-  CID输出数据长度
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_enter_lpm(uint32_t gate, uint32_t irqid, tee_lpm_mode_e 
mode);
```
- 参数
   - gate- PMU gate寄存器地址
   - irqid- 中断号ID（reserved）
   - mode - 低功耗模式类型
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举类型

      typedef enum {
            TEE_LPM_MODE_WAIT = 0,   ///< lpm wait
            TEE_LPM_MODE_DOZE = 1,   ///< lpm doze
            TEE_LPM_MODE_STOP = 2,   ///< lpm stop
            TEE_LPM_MODE_STANDBY = 3, ///< lpm standby
            TEE_LPM_MODE_CLOCK = 4,  ///< lpm clock gate
            TEE_LPM_MODE_MAX,
    } tee_lpm_mode_e; |

```c
int32_t csi_tee_base64(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len, uint32_t is_encode,
uint32_t wsafe);
```
- 参数
   - in - 消息数据指针
   - in_len - 消息数据长度
   - out - 编码数据存放指针
   - out_len- 编码数据长度
   - is_encode - 编码解码标志位
   - wsafe - websafe取代特性
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_base64_encode(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len,);
```
- 参数
   - in - 消息数据指针
   - in_len - 消息数据长度
   - out - 编码数据存放指针
   - out_len- 编码数据长度
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_base64_decode(const uint8_t *in, uint32_t in_len, uint8_t *out, uint32_t *out_len,);
```
- 参数
   - in - 消息数据指针
   - in_len - 消息数据长度
   - out - 解码数据存放指针
   - out_len- 解码数据长度
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_hmac(const uint8_t *in, uint32_t  in_len, const uint8_t *key, uint32_t key_len, uint8_t *out,
tee_hmac_type_e type, tee_hash_op_e hash_op, uint32_t *ctx);
```
- 参数
   - in - 消息数据指针
   - in_len - 消息数据长度
   - key - 密钥指针
   - key_len - 密钥长度
   - out - 认证码存放指针
   - type- 认证码类型
   - hash_op- 哈希类型
   - ctx - 哈希上下文
   
- 返回值
   - 0 - 成功
   - !0 - 失败
   
- 枚举类型

      typedef enum {
           TEE_SHA1 = 0,   ///< SHA1
           TEE_SHA256 = 1, ///< SHA256
           TEE_SHA224 = 2, ///< SHA224
           TEE_SHA384 = 3, ///< SHA384
           TEE_SHA512 = 4, ///< SHA512
           TEE_SHA_MAX,    ///< invaild sha type
       } tee_sha_type_t;
      /****** TEE HMAC type *****/
     typedef enum {
           TEE_HMAC_SHA1 = 1,    ///< HMAC with SHA1
      } tee_hmac_type_e; |
  
  

```c
int32_t csi_tee_hmac_digest(const uint8_t *in, uint32_t  in_len, const uint8_t *key, uint32_t key_len, uint8_t *out, tee_hmac_type_e type);
```
- 参数
   - in - 消息数据指针
   - in_len - 消息数据长度
   - key - 密钥指针
   - key_len - 密钥长度
   - out - 认证码存放指针
   - type- 认证码类型
- 返回值
   - 0 - 成功
   - !0 - 失败

```c 
int32_t csi_tee_rand_generate(uint8_t *out, uint32_t out_len);
```
- 参数
   - out - 随机数存放指针
   - out_len- 随机数长度
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_rand_generate(uint8_t *out, uint32_t out_len);
```
- 参数
   - out - 随机数存放指针
   - out_len- 随机数长度
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_sha(const uint8_t *in, uint32_t in_len, uint8_t *out, tee_sha_type_t type, tee_hash_op_e hash_op, void *ctx);
```
- 参数
   - in - 哈希数据指针
   - in_len - 哈希数据长度
   - out - 哈希密钥指针
   - type - 哈希类型
   - hash_op- 哈希操作类型
   - ctx - 哈希上下文
- 返回值
- - 0 - 成功
- !0 - 失败

- 枚举定义
   - typedef enum {

                 TEE_SHA1 = 0,   ///< SHA1
                 TEE_SHA256 = 1, ///< SHA256
                 TEE_SHA224 = 2, ///< SHA224
                 TEE_SHA384 = 3, ///< SHA384
                 TEE_SHA512 = 4, ///< SHA512
                 TEE_SHA_MAX,    ///< invaild sha type
             } tee_sha_type_t;
   - typedef enum {

                 TEE_HASH_OP_NONE = 0,     ///< No operation
                 TEE_HASH_OP_START = 1,    ///< HASH init
                 TEE_HASH_OP_UPDATA = 2,   ///< HASH update
                 TEE_HASH_OP_FINISH = 3,   ///< HASH finish
                 TEE_HASH_OP_MAX,          ///< invailed operation
             } tee_hash_op_e; 

```c
int32_t csi_tee_sha_digest(tee_sha_type_t type, void *ctx);
```
- 参数
   - hash_op- 哈希操作类型
   - ctx - 哈希上下文
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_sha_start(tee_sha_type_t type, void *ctx);
```
- 参数
   - in - 哈希数据指针
   - in_len - 哈希数据长度
   - out - 哈希密钥指针
   - type - 哈希类型
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_sha_update(const uint8_t *in, uint32_t in_len, uint8_t *ctx);
```
- 参数
   - in - 哈希数据指针
   - in_len - 哈希数据长度
   - ctx  - 哈希上下文
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_sha_finish(uint8_t *out, void *ctx);
```
- 参数
   - out - 哈希密钥指针
   - ctx - 哈希上下文
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
nt32_t csi_tee_aes_encrypt(const uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, 
uint8_t iv[16], uint8_t *out, tee_aes_mode_e mode)
```
- 参数
   - in - 加密数据指针
   - in_len - 加密数据长度
   - key - 加密密钥指针
   - key_len - 加密密钥长度
   - iv - IV指针
   - out - 加密数据指针
   - mode - AES操作模式
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举定义
   - typedef enum {

             TEE_AES_MODE_ECB = 0,    ///< TEE AES ECB mode
             TEE_AES_MODE_CBC = 1,    ///< TEE AES CBC mode
             TEE_AES_MODE_MAX,        ///< invaild mode
             } tee_aes_mode_e; 



```c
int32_t csi_tee_aes_decrypt(const uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, 
uint8_t iv[16], uint8_t *out, tee_aes_mode_e mode)
```
- 参数
   - in - 解密数据指针
   - in_len - 解密数据长度
   - key - 解密密钥指针
   - key_len - 解密密钥长度
   - iv - IV指针out - 解密数据指针
   - out - 解密数据指针
   - mode - AES操作模式
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举定义
   - typedef enum {

             TEE_AES_MODE_ECB = 0,    ///< TEE AES ECB mode
             TEE_AES_MODE_CBC = 1,    ///< TEE AES CBC mode
             TEE_AES_MODE_MAX,        ///< invaild mode
             } tee_aes_mode_e;
```c
int32_t csi_tee_aes_encrypt_ecb(const uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, uint8_t *out)
```
- 参数
   - in - 加密数据指针
   - in_len - 加密数据长度
   - key - 加密密钥指针
   - key_len - 加密密钥长度
   - out - 加密数据指针
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_aes_decrypt_ecb(const uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, uint8_t *out)
```
- 参数
   - in - 解密数据指针
   - in_len - 解密数据长度
   - key - 解密密钥指针
   - key_len - 解密密钥长度
   - out - 解密数据指针
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_aes_encrypt_cbcconst uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, uint8_t *iv, uint8_t *out)
```
- 参数
   - in - 加密数据指针
   - in_len - 加密数据长度
   - key - 加密密钥指针
   - key_len - 加密密钥长度
   - iv - iv数据指针
   - out - 加密数据指针
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_aes_decrypt_cbc(const uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, uint8_t *iv, uint8_t *out)
```
- 参数
   - in - 解密数据指针
   - in_len - 解密数据长度
   - key - 解密密钥指针
   - key_len - 解密密钥长度
   - iv - iv数据指针
   - out - 解密数据指针
- 返回值
   - 0 - 成功
   - !0 - 失败

```c
int32_t csi_tee_rsa_sign(const uint8_t *in, uint32_t in_len, const uint8_t *key, uint32_t key_len, uint8_t *sign, uint32_t *sign_len，tee_rsa_sign_type_e type);
```
- 参数
   - in - 消息指针
   - in_len - 消息长度
   - key - 私钥指针
   - key_len - 私钥长度
   - sign - 签名数据指针
   - sign_len - 签名数据长度
   - type - 签名类型
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举类型

      typedef enum {
            TEE_RSA_MD5    = 0,     ///< MD5
            TEE_RSA_SHA1   = 1,     ///< SHA1
            TEE_RSA_SHA256 = 3,     ///< SHA256
            TEE_RSA_SIGN_TYPE_MAX,  ///< invailed type
      } tee_rsa_sign_type_e;

```c
int32_t csi_tee_rsa_verify(const uint8_t *in, uint32_t in_len,const uint8_t *key, uint32_t key_len,uint8_t *sign, uint32_t sign_len,tee_rsa_sign_type_e type);
```
- 参数
   - in - 消息指针
   - in_len - 消息长度
   - key - 公钥指针
   - key_len - 公钥长度
   - sign - 签名数据指针
   - sign_len - 签名数据长度
   - type - 签名类型
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举类型

      typedef enum {
            TEE_RSA_MD5    = 0,     ///< MD5
            TEE_RSA_SHA1   = 1,     ///< SHA1
            TEE_RSA_SHA256 = 3,     ///< SHA256
            TEE_RSA_SIGN_TYPE_MAX,  ///< invailed type
      } tee_rsa_sign_type_e;

```c
int32_t csi_tee_rsa_encrypt(const uint8_t *in, uint32_t in_len,const uint8_t *key, uint32_t key_len,uint8_t *out, uint32_t *out_len, tee_rsa_padding_mode_e padding);
```
- 参数
   - in - 消息指针
   - in_len - 消息长度
   - key - 公钥指针
   - key_len - 公钥长度
   - out- 加密数据指针
   - out_len - 加密数据长度
   - padding- 加密类型
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举类型

      typedef enum {
           TEE_RSA_PKCS1_PADDING = 0x01,     ///< RSA PKCS padding mode
           TEE_RSA_NO_PADDING    = 0x02,     ///< RSA no padding mode
     } tee_rsa_padding_mode_e;

```c
int32_t csi_tee_rsa_decrypt(const uint8_t *in, uint32_t in_len,const uint8_t *key, uint32_t key_len,uint8_t *out, uint32_t *out_len, tee_rsa_padding_mode_e padding);
```
- 参数
   - in - 消息指针
   - in_len - 消息长度
   - key - 私钥指针
   - key_len - 私钥长度
   - out- 加密数据指针
   - out_len - 加密数据长度
   - padding- 加密类型
- 返回值
   - 0 - 成功
   - !0 - 失败
- 枚举类型

      typedef enum {
           TEE_RSA_PKCS1_PADDING = 0x01,     ///< RSA PKCS padding mode
           TEE_RSA_NO_PADDING    = 0x02,     ///< RSA no padding mode
     } tee_rsa_padding_mode_e;



## 示例
int main()
{
	uint8_t plain[16] = {0};
	uint8_t cipher[16] = {0};
	uint8_t key[32] = {0};
    return csi_tee_aes_encrypt_ecb(plain, 16, key, 32, out);
}



## 诊断错误码
| 错误码 | 错误码说明 |
| :--- | :--- |
| TEEC_SUCCESS | 成功 |
| TEEC_ERROR_GENERIC | 通用错误码 |
| TEEC_ERROR_ACCESS_DENIED | 拒绝访问 |
| TEEC_ERROR_CANCEL | 操作取消 |
| TEEC_ERROR_ACCESS_CONFLICT | 访问冲突 |
| TEEC_ERROR_EXCESS_DATA | 数据超大 |
| TEEC_ERROR_BAD_FORMAT | 错误格式 |
| TEEC_ERROR_BAD_PARAMETERS | 错误参数 |
| TEEC_ERROR_BAD_STATE | 错误状态 |
| TEEC_ERROR_ITEM_NOT_FOUND | 资源不存在 |
| TEEC_ERROR_NOT_IMPLEMENTED | 未实现 |
| TEEC_ERROR_NOT_SUPPORTED | 不支持 |
| TEEC_ERROR_NO_DATA | 无此数据 |
| TEEC_ERROR_OUT_OF_MEMORY | 内存耗尽 |
| TEEC_ERROR_BUSY | 遇忙 |
| TEEC_ERROR_COMMUNICATION | 通信失败 |
| TEEC_ERROR_SECURITY | 安全失败 |
| TEEC_ERROR_SHORT_BUFFER | buffer过小 |



## 运行资源
Flash: 32KB
Ram: 8KB



## 依赖资源

  - partition
  - key_mgr



## 组件参考

无。

