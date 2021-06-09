# 密钥管理（key_mgr）

## 概述

密钥管理（key_mgr）组件实现密钥管理功能，通过抽象key_mgr组件，为用户提供统一的平台密钥管理及平台安全信息获取接口。
该组件提供以下功能：
• 密钥获取
• 密钥派生
• 密钥导入
• 密钥导出
• 密钥销毁
• 平台安全信息获取




## 组件安装

```bash
yoc init
yoc install key_mgr
```



## 配置

无




## 接口列表

key_mgr接口如下所示, 密钥管理接口如无备注都需要在安全状态下调用，安全状态指系统中有如下配置（如bootloader、tee子系统中）：
```bash
CONFIG_SYSTEM_SECURE: 1
```

| 函数 | 说明 | 备注 |
| :--- | :--- | ---- |
| km_init | 初始化aes |  |
| km_uninit | 去初始化aes |  |
| km_get_key | 密钥获取 | KM_ID_PUBKEY_E 可以在非安全状态下获取。 |
| km_get_pub_key_by_name | 根据密钥名称获取非对称算法公钥 | |
| km_derive_key | 密钥派生 |  |
| km_import_key | 密钥导入 |  |
| km_export_key | 密钥导出 |  |
| km_destory_key | 密钥销毁 |  |



## 接口详细说明

### km_init
```c
/**
  \brief       km initialiez.
  \return      error code
*/
uint32_t km_init(void);
```
### km_uninit
```c
/**
  \brief       km uninitialiez.
*/
void km_uninit(void);
```

### km_get_key
```c
/**
  \brief       Get key from km
  \param[in]   key_type  	key type of km_key_type_e
  \param[out]  key     		key
  \param[out]  key_size     out key size.
  \return      error code
*/
uint32_t km_get_key(km_key_type_e key_type, key_handle *key, uint32_t *key_size);
```
### km_get_pub_key_by_name
```c
/**
  \brief       Get key from km by name
  \param[in]   key_type  	key type of km_key_type_e
  \param[out]  key     		key
  \param[out]  key_size     out key size.
  \return      error code
*/
uint32_t km_get_pub_key_by_name(const char *name, key_handle *key, uint32_t *key_size);
```
### km_derive_key
```c
/**
  \brief       Derive key function.
  \param[in]   key 		derive alg.
  \param[in]   key      Buffer to get key
  \param[in]   key_size Key buffer max size.
  \param[out]  outkey   Buffer to derive key
  \param[out]  size     derive key size.
  \return      error code
*/
uint32_t km_derive_key(km_derive_alg_e alg, uint8_t *key, uint32_t key_size, uint8_t *outkey, uint32_t *size);
```
### km_import_key
```c
/**
  \brief      Get key from km
  \param[in]  key_type   key type,see \ref km_key_type_e
  \param[in]  key     	 Buffer to import key
  \param[in]  key_size   key size.
  \return     error code
*/
uint32_t km_import_key(km_key_type_e key_type, uint8_t *key, uint32_t key_size);
```
### km_export_key
```c
/**
  \brief       Get key from km
  \param[in]   key_type   key type,see \ref km_key_type_e
  \param[out]  keybuf     Buffer to export key
  \param[in]  buf_size    Buffer max size.
  \param[out]  key_size   out key size.
  \return      error code
*/
uint32_t km_export_key(km_key_type_e key_type, uint8_t *keybuf, uint32_t buf_size, uint32_t *key_size);
```
### km_destory_key
```c
/**
  \brief       Get key from km
  \param[in]   key_type  key type of km_key_type_e
  \return      error code
*/
uint32_t km_destory_key(km_key_type_e key_type);
```



## 示例

### 获取RSA算法公钥使用示例

```c
void key_mgr_demo_main()
{
    uint32_t   ret;
    key_handle key_addr;
    uint32_t   key_size;
    ret = km_get_key(KM_ID_PUBKEY_E, &key_addr, &key_size);
    if (ret == KM_ERR_NOT_EXIST || key_size != 256)) {
    	printf("%s, %d, %x\n", __FUNCTION__, __LINE__, ret);
    }
    return;
}
```


## 诊断错误码

| 错误码 | 值 |
| :--- | :--- |
| KM_OK | 0x0 |
| KM_ERR | 0x20000001 |
| KM_ERR_NOT_EXIST | 0x20000002 |
| KM_ERR_VERSION | 0x20000003 |
| KM_ERR_INVALIED_PARAM | 0x20000004 |
| KM_ERR_BUF_TOO_SMALL | 0x20000005 |
| KM_ERR_NOT_SUPPORT | 0x20000006 |
| KM_ERR_DRV | 0x20000007 |
| KM_ERR_TOO_LONG | 0x20000008 |
| KM_ERR_PARTITION | 0x20000009 |



## 运行资源

Flash: 1.6KB
Ram: 3.4KB



## 依赖资源

  - csi



## 组件参考

代码示例见pangu_secure_demo应用示例程序。

