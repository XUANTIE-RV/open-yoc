# 安全算法（sec_crypto）

## 概述

sec_crypto组件为安全算法组件，提供安全平台的安全算法抽象开发接口，实现常见的密码学算法，包含对称算法、非对称/公钥算法、摘要算法、消息验证码算法。

### 功能
- 对称算法
   - AES ECB/CBC
   - AES with 128/192/256 bit keywidth
- 非对称算法
   - RSA1024 
   - RSA2048
   - ECC256
- 摘要
   - SHA1、SHA2
- 消息验证码
- 随机数


## 组件安装

```bash
yoc init
yoc install sec_crypto
```


## 配置

无。


## 接口列表

sec_crypto接口如下所示：

| 函数 | 说明 |
| :--- | :--- |
| sc_aes_init | 初始化aes |
| sc_aes_uninit | 去初始化aes |
| sc_aes_set_encrypt_key | 设置加密密钥 |
| sc_aes_set_decrypt_key | 设置解密密钥 |
| sc_aes_ecb_encrypt | 加密计算（ecb模式） |
| sc_aes_ecb_decrypt | 解密计算（ecb模式） |
| sc_aes_cbc_encrypt | 加密计算（cbc模式） |
| sc_aes_cbc_decrypt | 解密计算（cbc模式） |
| sc_aes_cfb1_encrypt | 加密计算（cfb1模式） |
| sc_aes_cfb1_decrypt | 解密计算（cfb1模式） |
| sc_aes_cfb8_encrypt | 加密计算（cfb8模式） |
| sc_aes_cfb8_decrypt | 解密计算（cfb8模式） |
| sc_aes_cfb128_decrypt | 解密计算（cfb128模式） |
| sc_aes_cfb128_encrypt | 加密计算（cfb128模式） |
| sc_aes_ofb_encrypt | 加密计算（ofb模式） |
| sc_aes_ofb_decrypt | 解密计算（ofb模式） |
| sc_aes_ctr_encrypt | 加密计算（ctr模式） |
| sc_aes_ctr_decrypt | 解密计算（ctr模式）|
| sc_srv_authenticate | 安全服务认证(暂未实现) |
| sc_srv_memprot | 安全服务保护 |
| sc_rng_get_multi_word | 获取指定字节的随机数 |
| sc_rng_get_single_word | 获取一个字的随机数 |
| sc_rsa_init | 初始化rsa |
| sc_rsa_uninit | 去初始化rsa |
| sc_rsa_attach_callback | 注册回调 |
| sc_rsa_detach_callback | 注销回调 |
| sc_rsa_gen_key | 生成rsa密钥对 |
| sc_rsa_encrypt | rsa加密 |
| sc_rsa_decrypt | rsa解密 |
| sc_rsa_sign | rsa签名 |
| sc_rsa_verify | rsa验签 |
| sc_rsa_encrypt_async | 异步模式下的rsa加密 |
| sc_rsa_decrypt_async | 异步模式下的rsa解密 |
| sc_rsa_sign_async | 异步模式下的rsa签名 |
| sc_rsa_verify_async | 异步模式下的rsa验签 |
| sc_rsa_get_state | 获取rsa状态 |
| sc_rsa_get_prime | 获取最大素数 |
| sc_rsa_enable_pm | 使能rsa电源管理 |
| sc_rsa_disable_pm | 禁用rsa电源管理 |
| sc_sha_init | 初始化sha |
| sc_sha_uninit | 反初始化sha |
| sc_sha_attach_callback | 注册回调 |
| sc_sha_detach_callback | 注销回调 |
| sc_sha_start | 开始计算 |
| sc_sha_update | 积累计算 |
| sc_sha_update_async | 异步模式下的积累计算 |
| sc_sha_finish | 结束计算 |


## 接口详细说明


### sc_aes_init

`uint32_t sc_aes_init(sc_aes_t *aes, uint32_t idx);`

- 功能描述:
   - 通过索引号初始化对应的aes实例，返回结果值。

- 参数:
   - `aes`: 实例句柄（需要用户申请句柄空间）。
   - `idx`: 控制器号。

- 返回值:
   - SC_OK: 初始化成功。
   - SC_PARAM_INV: 参数无效。

#### sc_aes_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| handle | aes_handle_t | 句柄 |
| key | unsigned char | 密钥 |
| key_len | unsigned int | 密钥长度 |
| csi_aes | csi_aes_t | csi_aes结构体|
| aes_ctx | sc_mbedtls_aes_context | 上下文 |


### sc_aes_uninit

`void sc_aes_uninit(sc_aes_t *aes);`

- 功能描述:
   - aes实例反初始化，该接口会停止aes实例正在进行的工作（如果有），并且释放相关的软硬件资源。

- 参数:
   - `aes`: 实例句柄。

- 返回值:
   - 无。


### sc_aes_set_encrypt_key

`uint32_t sc_aes_set_encrypt_key(sc_aes_t *aes, void *key, sc_aes_key_bits_t key_len);`

- 功能描述:
   - 设置aes算法加密密钥。

- 参数:
   - `aes`: 实例句柄。
   - `key`: 密钥指针。
   - `key_len`: 密钥长度。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。

#### sc_aes_key_bits_t

| 类型 | 说明 |
| :--- | :--- |
| SC_AES_KEY_LEN_BITS_128 | 密钥长度128位 |
| SC_AES_KEY_LEN_BITS_192 | 密钥长度192位 |
| SC_AES_KEY_LEN_BITS_256 | 密钥长度256位 |


### sc_aes_set_decrypt_key

`uint32_t sc_aes_set_decrypt_key(sc_aes_t *aes, void *key, sc_aes_key_bits_t key_len);`

- 功能描述:
   - 设置aes算法解密密钥。

- 参数:
   - `aes`: 实例句柄。
   - `key`: 密钥指针。
   - `key_len`: 密钥长度。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。


### sc_aes_ecb_encrypt

`uint32_t sc_aes_ecb_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size);`

- 功能描述:
   - ecb模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。
   - SC_DRV_FAILED: 驱动失败。


### sc_aes_ecb_decrypt

`uint32_t sc_aes_ecb_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size);`

- 功能描述:
   - ecb模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。
   - SC_DRV_FAILED: 驱动失败。


### sc_aes_cbc_encrypt

`uint32_t sc_aes_cbc_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv);`

- 功能描述:
   - cbc模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cbc_decrypt

`uint32_t sc_aes_cbc_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv);`

- 功能描述:
   - cbc模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cfb1_encrypt

`uint32_t sc_aes_cfb1_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv);`

- 功能描述:
   - cfb1模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cfb1_decrypt

`uint32_t sc_aes_cfb1_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv);`

- 功能描述:
   - cfb1模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cfb8_encrypt

`uint32_t sc_aes_cfb8_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv);`

- 功能描述:
   - cfb8模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cfb8_decrypt

`uint32_t sc_aes_cfb8_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv);`

- 功能描述:
   - cfb8模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cfb128_decrypt

`uint32_t sc_aes_cfb128_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);`

- 功能描述:
   - cfb128模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。
   - `num`: 输出参数，计算了多少个cfb数据块。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_cfb128_encrypt

`uint32_t sc_aes_cfb128_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);`

- 功能描述:
   - cfb128模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。
   - `num`: 输出参数，计算了多少个cfb数据块。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_ofb_encrypt

`uint32_t sc_aes_ofb_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);`

- 功能描述:
   - ofb模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。
   - `num`: 输出参数，计算了多少个ofb数据块。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_ofb_decrypt

`uint32_t sc_aes_ofb_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, void *iv, uint32_t *num);`

- 功能描述:
   - ofb模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `iv`: 初始化向量。
   - `num`: 输出参数，计算了多少个ofb数据块。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_ctr_encrypt

`uint32_t sc_aes_ctr_encrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, uint8_t nonce_counter[16], uint8_t stream_block[16], void *iv, uint32_t *num);`

- 功能描述:
   - ctr模式加密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `nonce_counter`: 随机计数器128位。
   - `stream_block`: 流块。
   - `iv`: 初始化向量。
   - `num`: 输出参数，计算了多少个ctr数据块。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_aes_ctr_decrypt

`uint32_t sc_aes_ctr_decrypt(sc_aes_t *aes, void *in, void *out, uint32_t size, uint8_t nonce_counter[16], uint8_t stream_block[16], void *iv, uint32_t *num);`

- 功能描述:
   - ctr模式解密接口。

- 参数:
   - `aes`: 实例句柄。
   - `in`: 输入数据指针。
   - `out`: 输出数据指针。
   - `size`: 输入数据长度（16字节对齐）。
   - `nonce_counter`: 随机计数器128位。
   - `stream_block`: 流块。
   - `iv`: 初始化向量。
   - `num`: 输出参数，计算了多少个ctr数据块。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_srv_authenticate

`int sc_srv_authenticate(uint8_t *authinfo, int authinfo_size);`

- 功能描述:
   - 证书认证。

- 参数:
   - `authinfo`: 认证信息。
   - `authinfo_size`: 认证信息大小。

- 返回值:
   - 0: 成功 。
   - -1: 失败。


### sc_srv_memprot

`int sc_srv_memprot(uint32_t region_addr, int region_size, enum region_access_perm rap);`

- 功能描述:
   - 服务内存保护。

- 参数:
   - `region_addr`: 地址区域。
   - `region_size`: 区域大小。
   - `rap`: 访问权限。

- 返回值:
   - 0: 成功 。
   - -1: 失败。

#### region_access_perm

| 类型 | 说明 |
| :---: | :---: |
| RAP_ONLY_X | 0 |
| RAP_X | 1 |
| RAP_R | 2 |
| RAP_W | 3 |


### sc_rng_get_multi_word

`uint32_t sc_rng_get_multi_word(uint32_t *data, uint32_t num);`

- 功能描述:
   - 获取指定字节数的随机数。

- 参数:
   - `data`: 指向接收数据的缓存。
   - `num`: 需要接收的字节数。

- 返回值:
   - SC_OK: 成功。
   - SC_FAIL: 失败。


### sc_rng_get_single_word

`uint32_t sc_rng_get_single_word(uint32_t *data);`

- 功能描述:
   - 获取一个字的随机数。

- 参数:
   - `data`: 指向接收数据的缓存。

- 返回值:
   - SC_OK: 成功。
   - SC_FAIL: 失败。


### sc_rsa_init

`uint32_t sc_rsa_init(sc_rsa_t *rsa, uint32_t idx, sc_rsa_key_bits_t data_bits);`

- 功能描述:
   - 通过索引号初始化对应的rsa实例，返回结果值。

- 参数:
   - `rsa`: 实例句柄（需要用户申请句柄空间）。
   - `idx`: 控制器号。
   - `data_bits`: 密钥长度。

- 返回值:
   - SC_OK: 成功。
   - SC_FAIL: 失败。

#### sc_rsa_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| handle | rsa_handle_t | 句柄 |
| bits | sc_rsa_key_bits_t | 密钥 |
| csi_rsa | csi_rsa_t | csi_rsa结构体|
| rsa_ctx | sc_mbedtls_rsa_context | 上下文 |

#### sc_rsa_key_bits_t

| 类型 | 说明 |
| :--- | :--- |
| SC_RSA_KEY_BITS_192 | 密钥长度192位 |
| SC_RSA_KEY_BITS_256 | 密钥长度256位 |
| SC_RSA_KEY_BITS_512 | 密钥长度512位 |
| SC_RSA_KEY_BITS_1024 | 密钥长度1024位 |
| SC_RSA_KEY_BITS_2048 | 密钥长度2048位 |
| SC_RSA_KEY_BITS_3072 | 密钥长度3072位 |
| SC_RSA_KEY_BITS_4096 | 密钥长度4096位 |


### sc_rsa_uninit

`void sc_rsa_uninit(sc_rsa_t *rsa);`

- 功能描述:
   - rsa实例反初始化，该接口会停止rsa实例正在进行的工作（如果有），并且释放相关的软硬件资源。

- 参数:
   - `rsa`: 实例句柄。

- 返回值:
   - 无。


### sc_rsa_attach_callback

`uint32_t sc_rsa_attach_callback(sc_rsa_t *rsa, sc_rsa_callback_t cb, void *arg);`

- 功能描述:
   - 注册回调函数。

- 参数:
   - `rsa`: 实例句柄。
   - `cb`: 密钥指针。
   - `arg`: 用户定义的回调参数。

- 返回值:
   - 0: 成功 。
   - -1: 失败。


### sc_rsa_detach_callback

`void sc_rsa_detach_callback(sc_rsa_t *rsa);`

- 功能描述:
   - 注销回调函数。

- 参数:
   - `rsa`: 实例句柄。

- 返回值:
   - 无。


### sc_rsa_gen_key

`uint32_t sc_rsa_gen_key(sc_rsa_t *rsa, sc_rsa_context_t *context);`

- 功能描述:
   - 产生密码对。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 输入数据指针。

- 返回值:
   - SC_OK: 成功 。
   - SC_NOT_SUPPORT: 不支持此操作。

#### sc_rsa_context_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| n | void* | 公共模数指针 |
| e | void* | 公共指数指针 |
| d | void* | 私有指数指针|
| key_bits | uint32_t | 密钥长度 |
| padding_type | sc_rsa_padding_type_t | 填充类型 |
| hash_type | sc_rsa_hash_type_t | 哈希类型 |

#### sc_rsa_padding_type_t

| 类型 | 说明 |
| :--- | :--- |
| SC_RSA_PADDING_MODE_NO | 无填充模式 |
| SC_RSA_PADDING_MODE_PKCS1 | PKCS1填充模式 |
| SC_RSA_PADDING_MODE_PKCS1_OAEP | PKCS1 OAEP填充模式 |
| SC_RSA_PADDING_MODE_SSLV23 | SSLV23填充模式 |
| SC_RSA_PADDING_MODE_X931 | X931填充模式 |
| SC_RSA_PADDING_MODE_PSS | PSS填充模式 |

#### sc_rsa_hash_type_t

| 类型 | 说明 |
| :--- | :--- |
| SC_RSA_HASH_TYPE_MD5 | MD5 |
| SC_RSA_HASH_TYPE_SHA1 | SHA1 |
| SC_RSA_HASH_TYPE_SHA224 | SHA224 |
| SC_RSA_HASH_TYPE_SHA256 | SHA256 |
| SC_RSA_HASH_TYPE_SHA384 | SHA384 |
| SC_RSA_HASH_TYPE_SHA512 | SHA512 |


### sc_rsa_encrypt

`uint32_t sc_rsa_encrypt(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *out);`

- 功能描述:
   - 加密接口。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度（16字节对齐）。
   - `out`: 输出数据指针。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 失败。


### sc_rsa_decrypt

`uint32_t sc_rsa_decrypt(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *out, uint32_t *out_size);`

- 功能描述:
   - 解密接口。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度。
   - `out`: 输出数据指针。
   - `out_size`: 结果大小。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 失败。


### sc_rsa_sign

`uint32_t sc_rsa_sign(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *signature, sc_rsa_hash_type_t hash_type);`

- 功能描述:
   - 输出签名功能。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度。
   - `signature`: 输出签名指针。
   - `hash_type`: 哈希类型。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 失败。


### sc_rsa_verify

`bool sc_rsa_verify(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *signature, uint32_t sig_size, sc_rsa_hash_type_t hash_type);`

- 功能描述:
   - 验签功能。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 。
   - `src`: 输入数据指针。
   - `src_size`: 。
   - `signature`: 。
   - `sig_size`: 。
   - `hash_type`: 。

- 返回值:
   - SC_OK: 成功。
   - SC_PARAM_INV: 参数无效。


### sc_rsa_encrypt_async

`uint32_t sc_rsa_encrypt_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *out);`

- 功能描述:
   - 异步模式加密接口。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度。
   - `out`: 输出数据指针。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_rsa_decrypt_async

`uint32_t sc_rsa_decrypt_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *out, uint32_t *out_size);`

- 功能描述:
   - 异步模式解密接口。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度。
   - `out`: 输出数据指针。
   - `out_size`: 输出数据长度指针。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_rsa_sign_async

`uint32_t sc_rsa_sign_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *signature, sc_rsa_hash_type_t hash_type);`

- 功能描述:
   - 异步模式生成签名。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度。
   - `signature`: 输出签名指针。
   - `hash_type`: 哈希类型。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_rsa_verify_async

`uint32_t sc_rsa_verify_async(sc_rsa_t *rsa, sc_rsa_context_t *context, void *src, uint32_t src_size, void *signature, uint32_t sig_size, sc_rsa_hash_type_t hash_type);`

- 功能描述:
   - 异步模式验签功能。

- 参数:
   - `rsa`: 实例句柄。
   - `context`: 上下文。
   - `src`: 输入数据指针。
   - `src_size`: 输入数据长度。
   - `signature`: 输出数据长度指针。
   - `sig_size`: 签名长度。
   - `hash_type`: 哈希类型。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_rsa_get_state

`uint32_t sc_rsa_get_state(sc_rsa_t *rsa, sc_rsa_state_t *state);`

- 功能描述:
   - 获取rsa模块状态。

- 参数:
   - `rsa`: 实例句柄。
   - `state`: rsa 状态指针。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。。

#### sc_rsa_state_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| busy : 1 | uint8_t | Calculate busy flag |
| error : 1 | uint8_t | Calculate error flag |


### sc_rsa_get_prime

`uint32_t sc_rsa_get_prime(sc_rsa_t *rsa, void *p, uint32_t bit_length);`

- 功能描述:
   - 获取最大素数。

- 参数:
   - `rsa`: 实例句柄。
   - `p`: 素数指针。
   - `bit_length`: 素数长度。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_rsa_enable_pm

`uint32_t sc_rsa_enable_pm(sc_rsa_t *rsa);`

- 功能描述:
   - 使能rsa电源管理。

- 参数:
   - `rsa`: 实例句柄。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_rsa_disable_pm

`void sc_rsa_disable_pm(sc_rsa_t *rsa);`

- 功能描述:
   - 禁用rsa电源管理。

- 参数:
   - `rsa`: 实例句柄。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_sha_init

`uint32_t sc_sha_init(sc_sha_t *sha, uint32_t idx);`

- 功能描述:
   - 初始化sha。

- 参数:
   - `sha`: 实例句柄。
   - `idx`: sha索引号。

- 返回值:
   - SC_OK: 成功
   - SC_PARAM_INV: 参数无效。

#### sc_sha_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| handle | sha_handle_t | 句柄 |
| ctx | sc_sha_context_t | 上下文 |
| mode | sc_sha_mode_t | 模式|
| csi_sha | csi_sha_t | 句柄 |


### sc_sha_uninit

`void sc_sha_uninit(sc_sha_t *sha);`

- 功能描述:
   - sha实例反初始化，该接口会停止sha实例正在进行的工作（如果有），并且释放相关的软硬件资源。

- 参数:
   - `sha`: 实例句柄。

- 返回值:
   - 无。


### sc_sha_attach_callback

`uint32_t sc_sha_attach_callback(sc_sha_t *sha, void *callback, void *arg);`

- 功能描述:
   - 注册回调函数。

- 参数:
   - `sha`: 实例句柄。
   - `callback`: sha实例的事件回调函数（一般在中断上下文执行）。
   - `arg`: 回调函数参数（可选，由用户定义）。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作 。


### sc_sha_detach_callback

`void sc_sha_detach_callback(sc_sha_t *sha);`

- 功能描述:
   - 注销回调函数。

- 参数:
   - `sha`: 实例句柄。

- 返回值:
   - 无。


### sc_sha_start

`uint32_t sc_sha_start(sc_sha_t *sha, sc_sha_context_t *context, sc_sha_mode_t mode);`

- 功能描述:
   - 配置sha工作模式，设置数据源寄存器和数据目的寄存器。

- 参数:
   - `sha`: 实例句柄。
   - `context`: 上下文。
   - `mode`: 模式。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。

#### sc_sha_context_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| ctx | uint8_t | 上下文/使能宏CONFIG_CSI_V1有效 |
| ctx | csi_sha_context_t | 上下文/使能宏CONFIG_CSI_V2有效 |
| sha1_ctx | sc_mbedtls_sha1_context | 上下文|
| sha2_ctx | sc_mbedtls_sha256_context | 上下文 |
| mode | sc_sha_mode_t | 模式 |

#### sc_sha_mode_t

| 类型 | 说明 |
| :--- | :--- |
| SC_SHA_MODE_1 | SHA_1 |
| SC_SHA_MODE_256 | SHA_256 |
| SC_SHA_MODE_224 | SHA_224 |
| SC_SHA_MODE_512 | SHA_512 |
| SC_SHA_MODE_384 | SHA_384 |
| SC_SHA_MODE_512_256 | SHA_512_256 |
| SC_SHA_MODE_512_224 | SHA_512_224 |


### sc_sha_update

`uint32_t sc_sha_update(sc_sha_t *sha, sc_sha_context_t *context, const void *input, uint32_t size);`

- 功能描述:
   - 根据明文信息持续计算整数倍的明文包，并将最后一组（不足一包明文）缓存到context等待最后finish操作。

- 参数:
   - `sha`: 实例句柄。
   - `context`: 上下文。
   - `input`: 输入明文。
   - `size`: 明文长度。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。


### sc_sha_update_async

`uint32_t sc_sha_update_async(sc_sha_t *sha, sc_sha_context_t *context, const void *input, uint32_t size);`

- 功能描述:
   - （异步方式）根据明文信息持续计算整数倍的明文包，并将最后一组（不足一包明文）缓存到context等待最后finish操作。

- 参数:
   - `sha`: 实例句柄。
   - `context`: 上下文。
   - `input`: 输入明文。
   - `size`: 明文长度。

- 返回值:
   - SC_NOT_SUPPORT: 不支持此操作。


### sc_sha_finish

`uint32_t sc_sha_finish(sc_sha_t *sha, sc_sha_context_t *context, void *output, uint32_t *out_size);`

- 功能描述:
   - 配合sc_sha_update接口进行长明文操作后，最后填充数据包并计算。

- 参数:
   - `sha`: 实例句柄。
   - `context`: 上下文。
   - `output`: 输出结果。
   - `out_size`: 输出结果大小（单位：字节）。

- 返回值:
   - SC_OK: 成功 。
   - SC_PARAM_INV: 参数无效。


## 示例


### AES 同步模式使用示例

#### ECB-256模式

```c
#include <stdio.h>
#include "include/sec_crypto_aes.h"

#define CHECK_RETURN(ret)                           \
        do {                                        \
            if (ret != 0) {                         \
                return -1;                          \
            }                                       \
        } while(0);

static sc_aes_t g_aes;

int main(void)
{
    int ret = 0;
    const uint8_t in[16] = "Hello, World!";
    const uint8_t key[32] = "Demo-Key";
    const uint8_t expect[16] =
        {0x7b, 0xab, 0x3a, 0xb2, 0xf3, 0x3c, 0xcf, 0xca, 
         0x20, 0x7a, 0xc9, 0x63, 0x7f, 0x02, 0x49, 0x55};
    uint8_t out[16];

    /* STEP 1: init aes */
    ret = sc_aes_init(&g_aes, 0);
    CHECK_RETURN(ret);

    /* STEP 2: config encrypt key */
    ret = sc_aes_set_encrypt_key(&g_aes, (void *)key, SC_AES_KEY_LEN_BITS_256);
    CHECK_RETURN(ret);

    /* STEP 3: do encryption process */
    ret = sc_aes_ecb_encrypt(&g_aes, (void *)in, (void *)out, 16);
    CHECK_RETURN(ret);

    /* STEP 4: compare output result */
    ret = memcmp(out, expect, 16);
    CHECK_RETURN(ret);

    /* STEP 5: config decrypt key */
    ret = sc_aes_set_decrypt_key(&g_aes, (void *)key, SC_AES_KEY_LEN_BITS_256);
    CHECK_RETURN(ret);

    /* STEP 6: do decryption process */
    ret = sc_aes_ecb_decrypt(&g_aes, (void *)expect, (void *)out, 16);
    CHECK_RETURN(ret);

    /* STEP 7: compare output result */
    ret = memcmp(out, in, 16);
    CHECK_RETURN(ret);

    return ret;
}
```


#### CBC-256模式

```c
#include <stdio.h>
#include "include/sec_crypto_aes.h"

#define CHECK_RETURN(ret)                           \
        do {                                        \
            if (ret != 0) {                         \
                return -1;                          \
            }                                       \
        } while(0);

static sc_aes_t g_aes;

int main(void)
{
    int ret = 0;
    const uint8_t in[16] = "Hello, World!";
    const uint8_t key[32] = "Demo-Key";
    const uint8_t iv[16] = {0};
    const uint8_t expect[16] =
        {0x7b, 0xab, 0x3a, 0xb2, 0xf3, 0x3c, 0xcf, 0xca, 
         0x20, 0x7a, 0xc9, 0x63, 0x7f, 0x02, 0x49, 0x55};
    uint8_t out[16];

    /* STEP 1: init aes */
    ret = sc_aes_init(&g_aes, 0);
    CHECK_RETURN(ret);

    /* STEP 2: config encrypt key */
    ret = sc_aes_set_encrypt_key(&g_aes, (void *)key, SC_AES_KEY_LEN_BITS_256);
    CHECK_RETURN(ret);

    /* STEP 3: do encryption process */
    ret = sc_aes_cbc_encrypt(&g_aes, (void *)in, (void *)out, 16, (void *)iv);
    CHECK_RETURN(ret);

    /* STEP 4: compare output result */
    ret = memcmp(out, expect, 16);
    CHECK_RETURN(ret);

    /* STEP 5: clear init vector (!!!if not clear will cache last iv value) */
    memset((void *)iv, 0, 16);

    /* STEP 6: config decrypt key */
    ret = sc_aes_set_decrypt_key(&g_aes, (void *)key, SC_AES_KEY_LEN_BITS_256);
    CHECK_RETURN(ret);

    /* STEP 7: do decryption process */
    ret = sc_aes_cbc_decrypt(&g_aes, (void *)expect, (void *)out, 16, (void *)iv);
    CHECK_RETURN(ret);

    /* STEP 8: compare output result */
    ret = memcmp(out, in, 16);
    CHECK_RETURN(ret);

    return ret;
}

```


### RSA 同步模式使用示例

#### 加解密及验签
```c
#include <stdio.h>
#include <string.h>
#include "include/sec_crypto_rsa.h"

static sc_rsa_t rsa = {0};

uint8_t modulus[] = {
    0xdf, 0x83, 0xe4, 0x76, 0x2d, 0x00, 0x61, 0xf6, 0xd0, 0x8d, 0x4a, 0x04, 0x66, 0xb1, 0xd5, 0x55,
    0xef, 0x71, 0xb5, 0xa5, 0x4e, 0x69, 0x44, 0xd3, 0x4f, 0xb8, 0x3d, 0xec, 0xb1, 0x1d, 0x5f, 0x82,
    0x6a, 0x48, 0x21, 0x00, 0x7f, 0xd7, 0xd5, 0xf6, 0x82, 0x35, 0xc2, 0xa6, 0x67, 0xa3, 0x53, 0x2d,
    0x3a, 0x83, 0x9a, 0xba, 0x60, 0xc2, 0x11, 0x22, 0xc2, 0x35, 0x83, 0xe9, 0x10, 0xa1, 0xb4, 0xa6,
    0x74, 0x57, 0x99, 0xd3, 0xa8, 0x6a, 0x21, 0x83, 0x76, 0xc1, 0x67, 0xde, 0xd8, 0xec, 0xdf, 0xf7,
    0xc0, 0x1b, 0xf6, 0xfa, 0x14, 0xa4, 0x0a, 0xec, 0xd1, 0xee, 0xc0, 0x76, 0x4c, 0xcd, 0x4a, 0x0a,
    0x5c, 0x96, 0xf2, 0xc9, 0xa4, 0x67, 0x03, 0x97, 0x2e, 0x17, 0xcd, 0xa9, 0x27, 0x9d, 0xa6, 0x35,
    0x5f, 0x7d, 0xb1, 0x6b, 0x68, 0x0e, 0x99, 0xc7, 0xdd, 0x5d, 0x6f, 0x15, 0xce, 0x8e, 0x85, 0x33
};
static const uint8_t publicExponent[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01
};
static const uint8_t privateExponent[] = {
    0xc6, 0x15, 0x3d, 0x02, 0xfe, 0x1e, 0xb8, 0xb2, 0xe3, 0x60, 0x53, 0x98, 0x52, 0xea, 0x87, 0x06,
    0x01, 0x8d, 0xe4, 0x4c, 0xfb, 0x90, 0x8f, 0x4e, 0x35, 0xf8, 0x31, 0xe8, 0xf1, 0x8d, 0xf6, 0x76,
    0xbd, 0x79, 0xee, 0xc5, 0x62, 0x87, 0x05, 0x37, 0xd1, 0x6d, 0x93, 0x73, 0xa5, 0xa5, 0x38, 0xb1,
    0x7c, 0x89, 0xe5, 0x36, 0x07, 0x49, 0xf5, 0xa5, 0xb8, 0x37, 0x75, 0x0f, 0xb7, 0x8d, 0x97, 0x69,
    0xc4, 0xd4, 0x8a, 0xb7, 0xfe, 0x74, 0x48, 0x45, 0x58, 0x47, 0x29, 0xa3, 0x0b, 0xa7, 0xdc, 0x55,
    0x98, 0x18, 0x8c, 0xd4, 0x52, 0xf5, 0xc9, 0xe8, 0x40, 0xce, 0x97, 0x46, 0x14, 0x1f, 0x62, 0x94,
    0xc3, 0x21, 0x1e, 0x5d, 0x49, 0x59, 0x31, 0xeb, 0xc4, 0x95, 0xf9, 0x33, 0x70, 0xa7, 0x90, 0xc3,
    0x9e, 0x98, 0x58, 0xa4, 0x00, 0xa4, 0x0f, 0xf3, 0x51, 0x80, 0xc6, 0x14, 0xfb, 0xd5, 0x5b, 0x01
};

uint8_t Digest_signature_pkcs1_padding_out[] = {
    0x07, 0x2d, 0x25, 0xde, 0xa5, 0xfd, 0x7c, 0xb0, 0x92, 0xb4, 0xee, 0x57, 0xe8, 0xd3, 0x79, 0x74,
    0x59, 0x25, 0x34, 0xef, 0xfd, 0x2b, 0xda, 0x8b, 0xa4, 0x40, 0x4e, 0xd8, 0x92, 0x6e, 0xee, 0x84,
    0x52, 0xb0, 0xe1, 0x0e, 0xa8, 0xa9, 0x68, 0x62, 0x1b, 0x51, 0xed, 0x50, 0x84, 0x98, 0x6a, 0x97,
    0x98, 0xe8, 0xcf, 0x3f, 0x85, 0xd3, 0x28, 0x26, 0xf3, 0x7a, 0x52, 0x4b, 0x04, 0x95, 0xe6, 0xfd,
    0xfa, 0x41, 0xf3, 0xac, 0x8a, 0x6d, 0x74, 0x91, 0x8c, 0x87, 0x52, 0x38, 0x08, 0x49, 0xf4, 0x60,
    0xcd, 0x4b, 0x1a, 0x9e, 0x52, 0x60, 0xf2, 0x73, 0x60, 0x31, 0x78, 0x37, 0xd9, 0x42, 0xc4, 0x61,
    0x43, 0xcf, 0x6d, 0x55, 0xee, 0x05, 0x19, 0xb7, 0xc3, 0x37, 0xa7, 0xa8, 0xa4, 0xbd, 0xf1, 0xac,
    0x8e, 0x39, 0x20, 0x59, 0xcd, 0xfc, 0x50, 0x16, 0x81, 0x2d, 0xeb, 0xba, 0x95, 0xe9, 0x38, 0xa5,
};

static const uint8_t Digest[] = {
    0xe4, 0x2c, 0x9f, 0x12, 0xf7, 0xd2, 0x67, 0x3a, 0x23, 0xea, 0x85, 0x61, 0xeb, 0xb2, 0xc2, 0x19,
    0xdc, 0xd9, 0xf1, 0xaa
};

uint8_t dst[1024 >> 3] = {0};
uint32_t dst_size = 0;
uint32_t async_verify_flag;

static void rsa_encrypt_decrypt()
{
    int ret;
    uint8_t i;
    int index;
    sc_rsa_context_t context;

    ///<sc_rsa_hash_type_t hash_type = SC_RSA_HASH_TYPE_SHA1;
    sc_rsa_state_t status;

    index = 1;

    for (i = 0; i < index; i++) {
        sc_rsa_init(&rsa, 0);
        context.padding_type = SC_RSA_PADDING_MODE_PKCS1;
        context.key_bits = SC_RSA_KEY_BITS_1024;
        context.n = (void *)modulus;
        context.e = (void *)publicExponent;

        ret = sc_rsa_encrypt(&rsa, &context, (void *)Digest, 20, (void *)dst);
        dst_size = 128;

        while (1) {
            sc_rsa_get_state(&rsa, &status);

            if (status.busy == 0) {
                break;
            }
        }

        context.n = (void *)modulus;
        context.d = (void *)privateExponent;

        ret = sc_rsa_decrypt(&rsa, &context, (void *)dst, dst_size, (void *)dst, (uint32_t *)&dst_size);

        while (1) {
            sc_rsa_get_state(&rsa, &status);

            if (status.busy == 0) {
                break;
            }
        }

        if (ret == 0) {
            printf("&rsa encrypt and decrypt: %s\n", memcmp(dst, Digest, 20) ? "Fail" : "Pass");
        }

        sc_rsa_uninit(&rsa);
    }

    printf("test &rsa successfully\n");
}


static void rsa_sign_verify()
{
    uint8_t i;
    uint8_t result;
    int index;
    sc_rsa_context_t context;
    sc_rsa_hash_type_t hash_type = SC_RSA_HASH_TYPE_SHA1;
    sc_rsa_state_t status;

    index = 1;

    for (i = 0; i < index; i++) {
        //&rsa = csi_rsa_initialize(i, rsa_crypto);
        sc_rsa_init(&rsa, 0);
        sc_rsa_detach_callback(&rsa);
        // csi_rsa_attach_callback(&rsa,rsa_example_cb,NULL);
        context.padding_type = RSA_PADDING_MODE_PKCS1;
        context.key_bits = RSA_KEY_BITS_1024;

        context.n = (void *)modulus;
        context.d = (void *)privateExponent;
        sc_rsa_sign(&rsa, &context, (void *)Digest, 20, (void *)dst, hash_type);
        dst_size = 128;

        while (1) {
            csc_rsa_get_state(&rsa, &status);

            if (status.busy == 0) {
                break;
            }
        }

        printf("&rsa sign: %s\n", memcmp(dst, Digest_signature_pkcs1_padding_out, dst_size) ? "Fail" : "Pass");
        context.n = (void *)modulus;
        context.e = (void *)publicExponent;
        result = sc_rsa_verify(&rsa, &context, (void *)Digest, 20, (void *)dst, dst_size,  hash_type);

        while (1) {
            sc_rsa_get_state(&rsa, &status);

            if (status.busy == 0) {
                break;
            }
        }

        printf("&rsa verify: %s\n", result ? "Pass" : "Fail");

        sc_rsa_uninit(&rsa);
    }
}

/*****************************************************************************
test_rsa: main function of the &rsa test

INPUT: NULL

RETURN: NULL

*****************************************************************************/
int example_rsa(void)
{
    rsa_sign_verify();
    rsa_encrypt_decrypt();
    return 0;
}

int main(void)
{
    ......
    printf("hello\n");
    return example_rsa();
}
```


### SHA 同步模式使用示例

#### SHA1-单次加密示例
```c
#include <stdio.h>
#include "include/sec_crypto_sha.h"

#define SHA_MESSAGE          ("abc")

#define CHECK_RETURN(ret)                           \
        do {                                        \
            if (ret != 0) {                         \
                return -1;                          \
            }                                       \
        } while(0);

static sc_sha_t g_sha;
static sc_sha_context_t   g_sha_context;

int main(void)
{
    int ret = 0;
    uint8_t out[64];            ///< calculation result of sha
    uint8_t in[1024];           ///< message of sha prepare calculation
    uint32_t out_size = 0;        ///< bytes of calculation result
    const uint8_t expect[20] = {0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d};

    /* STEP 1: init sha */
    ret = sc_sha_init(&g_sha, 0);
    CHECK_RETURN(ret);

    /* STEP 2: clean context cache */
    memset((uint8_t *)g_sha_context.total, 0, sizeof(csi_sha_context_t));

    /* STEP 3: config sha-1*/
    ret = sc_sha_start(&g_sha, &g_sha_context, SHA_MODE_1);
    CHECK_RETURN(ret);

    /* STEP 4: clean output buff */
    memset(out, 0, sizeof(out));

    /* STEP 5: first encrypt characters(abc) */
    memcpy(in, SHA_MESSAGE, strlen(SHA_MESSAGE));
    ret = sc_sha_update(&g_sha, &g_sha_context, in, strlen(SHA_MESSAGE));
    CHECK_RETURN(ret);

    /* STEP 6: finish encrypt and output result */
    ret = sc_sha_finish(&g_sha, &g_sha_context, out, &out_size);
    CHECK_RETURN(ret);

    /* STEP 7: verification output results */
    ret = memcmp(out, expect, out_size);
    CHECK_RETURN(ret);

    return ret;
}
```
#### SHA1-累计加密示例
```c
#include <stdio.h>
#include <soc.h>
#include <drv/sha.h>

#define SHA_MESSAGE_1          ("a")
#define SHA_MESSAGE_2          ("b")
#define SHA_MESSAGE_3          ("c")

#define CHECK_RETURN(ret)                           \
        do {                                        \
            if (ret != 0) {                         \
                return -1;                          \
            }                                       \
        } while(0);

static sc_sha_t g_sha;
static sc_sha_context_t   g_sha_context;

int main(void)
{
    int ret = 0;
    uint8_t out[64];            ///< calculation result of sha
    uint8_t in[1024];           ///< message of sha prepare calculation
    uint32_t out_size = 0;        ///< bytes of calculation result
    const uint8_t expect[20] = {0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d};

    /* STEP 1: init sha */
    ret = sc_sha_init(&g_sha, 0);
    CHECK_RETURN(ret);

    /* STEP 2: clean context cache */
    memset((uint8_t *)g_sha_context.total, 0, sizeof(csi_sha_context_t));

    /* STEP 3: config sha-1*/
    ret = sc_sha_start(&g_sha, &g_sha_context, SHA_MODE_1);
    CHECK_RETURN(ret);

    /* STEP 4: clean output buff */
    memset(out, 0, sizeof(out));

    /* STEP 5: first encrypt characters(abc) */
    memcpy(in, SHA_MESSAGE_1, strlen(SHA_MESSAGE_1));
    ret = sc_sha_update(&g_sha, &g_sha_context, in, strlen(SHA_MESSAGE_1));
    CHECK_RETURN(ret);

    /* STEP 6: first encrypt characters(abc) */
    memcpy(in, SHA_MESSAGE_2, strlen(SHA_MESSAGE_2));
    ret = sc_sha_update(&g_sha, &g_sha_context, in, strlen(SHA_MESSAGE_2));
    CHECK_RETURN(ret);

    /* STEP 7: first encrypt characters(abc) */
    memcpy(in, SHA_MESSAGE_3, strlen(SHA_MESSAGE_3));
    ret = sc_sha_update(&g_sha, &g_sha_context, in, strlen(SHA_MESSAGE_3));
    CHECK_RETURN(ret);

    /* STEP 8: finish encrypt and output result */
    ret = sc_sha_finish(&g_sha, &g_sha_context, out, &out_size);
    CHECK_RETURN(ret);

    /* STEP 9: verification output results */
    ret = memcmp(out, expect, out_size);
    CHECK_RETURN(ret);

    return ret;
}
```

```
#### ECDSA-示例
```c
#include <sec_crypto_ecdsa.h>
#include <stdio.h>
#include <string.h>
uint32_t sc_ecdsa_test(void)
{
    static const uint8_t private[] = {
        0xce,0x35,0x97,0xca,0x50,0x73,0x6c,0x68,0x57,0xe6,0xbd,0xe1,0x3f,0x41,0x68,0xbf,
        0xa1,0x0c,0xf1,0x37,0xa6,0x9e,0xfd,0xab,0x2d,0x12,0x39,0x8b,0xe4,0x43,0x69,0x8f,
    };

    static const uint8_t public[] = {
        0x68,0x27,0x99,0xce,0x79,0x4c,0x48,0x5a,0xc0,0x9e,0x6e,0x92,0x0a,0x8b,0xd7,0xc4,
        0x50,0x65,0x8f,0x22,0x2c,0x4f,0x35,0xb4,0xbe,0x98,0x9c,0x7c,0xe8,0xd2,0x25,0x2c,
        0xc7,0xc7,0x41,0xf0,0x91,0x28,0xe7,0x48,0x97,0xc5,0x1f,0x12,0x62,0xc2,0x29,0x99,
        0xba,0x66,0x32,0x83,0x88,0xe0,0x06,0x19,0xf2,0x34,0x62,0xc0,0xa3,0xd9,0xb4,0x30
    };
    uint32_t ret = 0;
    sc_ecdsa_t ecdsa;
    uint8_t hash[32] = {0};
    uint8_t sig[64] = {0};
    uint32_t sig_len = 0;

    /* STEP1：ECDSA handle initialize */
    sc_ecdsa_init(&ecdsa, 0);

    /* STEP2：Load curve param to ECDSA handle */
    sc_ecdsa_load_curve(&ecdsa, SC_CURVES_SECP521R1);

    memset(hash, 0x55, sizeof(hash));

    /* STEP3：ECDSA sign */
    ret = sc_ecdsa_sign(&ecdsa, private, sizeof(private), hash, sizeof(hash), sig, &sig_len);
    if (ret != 0) {
        printf("sc_ecdsa_sign failed, ret:0x%x line:%d \n", ret, __LINE__);
    } else {
        printf("sc_ecdsa_sign success, ret:0x%x line:%d \n", ret, __LINE__);
    }

    /* STEP4：ECDSA verify */
    ret = sc_ecdsa_verify(&ecdsa, public, sizeof(public), hash, sizeof(hash), sig, sig_len);
    if (ret != 0) {
        printf("sc_ecdsa_verify failed, ret:0x%x line:%d \n", ret, __LINE__);
    } else {
        printf("sc_ecdsa_verify success, ret:0x%x line:%d \n", ret, __LINE__);
    }

    /* STEP5：ECDSA handle De-initialize */
    sc_ecdsa_uninit(&ecdsa);

    return ret;
}
```

```
#### ECDH-示例
```c

#include <sec_crypto_ecdh.h>
#include <stdio.h>
#include <string.h>

uint32_t sc_ecdh_test(void)
{
    uint32_t ret = 0;
    sc_ecdh_t ecdh;
    uint8_t private_key_1[32] = {0};
    uint8_t public_key_1[64] = {0};
    uint8_t private_key_2[32] = {0};
    uint8_t public_key_2[64] = {0};
    uint8_t sk1[32] = {0};
    uint8_t sk2[32] = {0};
    uint32_t sk1_len = 0;
    uint32_t sk2_len = 0;

    /* STEP1：ECDH handle initialize */
    sc_ecdh_init(&ecdh, 0);

    /* STEP2：Load curve param to ECDH handle */
    sc_ecdh_load_curve(&ecdh, SC_CURVES_SECP521R1);

    /* STEP3：ECDH generate key pair 1 */
    ret = sc_ecdh_gen_keypair(&ecdh, private_key_1, public_key_1);
    if (ret != 0) {
        printf("sc_ecdh_gen_keypair failed, ret:0x%x line:%d \n", ret, __LINE__);
    } else {
        printf("sc_ecdh_gen_keypair success, ret:0x%x line:%d \n", ret, __LINE__);
    }

    /* STEP4：ECDH generate key pair 2 */
    ret = sc_ecdh_gen_keypair(&ecdh, private_key_2, public_key_2);
    if (ret != 0) {
        printf("sc_ecdh_gen_keypair failed, ret:0x%x line:%d \n", ret, __LINE__);
    } else {
        printf("sc_ecdh_gen_keypair success, ret:0x%x line:%d \n", ret, __LINE__);
    }

    /* STEP5：ECDH generate share secret 1 */
    ret = sc_ecdh_calc_secret(&ecdh, private_key_1, public_key_2, sk1, &sk1_len);
    if (ret != 0) {
        printf("sc_ecdh_calc_secret 1 failed, ret:0x%x line:%d \n", ret, __LINE__);
    } else {
        printf("sc_ecdh_calc_secret 1 success, ret:0x%x line:%d \n", ret, __LINE__);
    }

    /* STEP6：ECDH generate share secret 2 */
    ret = sc_ecdh_calc_secret(&ecdh, private_key_2, public_key_1, sk2, &sk2_len);
    if (ret != 0) {
        printf("sc_ecdh_calc_secret 2 failed, ret:0x%x line:%d \n", ret, __LINE__);
    } else {
        printf("sc_ecdh_calc_secret 2 success, ret:0x%x line:%d \n", ret, __LINE__);
    }

    /* STEP7：ECDH handle De-initialize */
    sc_ecdh_uninit(&ecdh);

    /* STEP8：Check whether sk1 and sk2 are the same */
    if (sk1_len != sk2_len) {
        printf("sc_ecdh_calc_secret failed, line:%d \n", __LINE__);
    } else {
        if (memcmp(sk1, sk2, sk1_len) != 0) {
            printf("sc_ecdh_calc_secret failed, line:%d \n", __LINE__);
        } else {
            printf("sc_ecdh_calc_secret success, sk1 = sk2, line:%d \n", __LINE__);
        }
    }

    return ret;
}
```



## 诊断错误码
| 错误码 | 错误码说明 |
| :--- | :--- |
| SC_FAIL | 失败 |
| SC_MEM_OVERFLOW | 内存溢出 |
| SC_PARAM_INV | 参数无效 |
| SC_OPERATION_BUSY | 操作忙 |
| SC_AUTH_FAIL | 认证失败 |
| SC_CRYPT_FAIL | 加密失败 |
| SC_NOT_SUPPORT | 不支持此操作 |
| SC_INVALID_PADDING | 无效填充 |
| SC_BAD_INPUT_DATA | 输入数据错误 |
| SC_INVALID_KEY_LENGTH | 密钥长度无效 |
| SC_INVALID_INPUT_LENGTH | 输入长度无效 |
| SC_FEATURE_UNAVAILABLE | 功能不可用 |
| SC_HW_ACCEL_FAILED | 硬件加速失败 |
| SC_CCM_AUTH_FAILED | ccm认证失败 |
| SC_KEY_GEN_FAILED | 密钥生成失败 |
| SC_KEY_CHECK_FAILED | 密钥检测失败 |
| SC_PUBLIC_FAILED | 公钥失败 |
| SC_PRIVATE_FAILED | 私钥失败 |
| SC_VERIFY_FAILED | 验证失败 |
| SC_OUTPUT_TOO_LARGE | 输出太大 |
| SC_RNG_FAILED | 随机数失败 |
| SC_BUFFER_TOO_SMALL | buffer太小 |
| SC_INVALID_FORMAT | 格式无效 |
| SC_ALLOC_FAILED | 内存分配失败 |
| SC_DRV_FAILED | 驱动失败 |


## 运行资源
无。


## 依赖资源

  - csi


## 组件参考
无。

