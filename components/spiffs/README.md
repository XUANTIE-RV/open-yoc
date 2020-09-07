# 概述

SPIFFS 是一个用于 SPI NOR flash 设备的嵌入式文件系统。
目前，SPIFFS 尚不支持目录，但可以生成扁平结构。如果 SPIFFS 挂载在 /spiffs 下，在 /spiffs/tmp/myfile.txt 路径下创建一个文件则会在 SPIFFS 中生成一个名为 /tmp/myfile.txt 的文件，而不是在 /spiffs/tmp 下生成名为 myfile.txt 的文件。

# 使用

包含以下头文件：

```c
#include <vfs.h>
#include <aos_spiffs.h>
```

## 初始化

先调用VFS初始化接口：

```C
vfs_init();
```

再调用SPIFFS的注册接口：

```C
vfs_spiffs_register("spiffs");
```

参数`"spiffs"`为分区的名字，需要在`config.yaml`做好配置。

## 删除

调用SPIFFS的删除接口：

```C
vfs_spiffs_unregister();
```

# 例子

## 初始化

```C
vfs_init();
if (vfs_spiffs_register("spiffs")) {
    printf("spiffs register failed.\n");
    return;
}
```

## 创建、读写文件

```C
static void test_spiffs_create_file_with_text(const char* name, const char* text)
{
    char buf[128] = {0};

    int  fd = aos_open(name, O_CREAT | O_TRUNC | O_RDWR);
    if (fd  < 0) {
        LOGE(TAG, "fopen failed, fd:%d, %s", fd, name);
        return;
    }
    ssize_t len = aos_write(fd, text, strlen(text));
    if (len != strlen(text)) {
        LOGE(TAG, "write text failed, %d", len);
        return;
    }
    aos_lseek(fd, 0, SEEK_SET);
    len = aos_read(fd, buf, 128);
    printf("len=%d\n", len);
    printf("%s\n", buf);
    int ret = aos_close(fd);
    printf("ret=%d\n", ret);
    if (ret == 0) {
        printf("fclose ok\n");
    } else {
        printf("fclose failed\n");
    }
}
```

# 其他

## config.yaml 例子

```yaml
mtb_version: 4
chip: pangu

diff:
  digest_type: SHA1
  signature_type: RSA1024
  fota_version: 0
  ram_buf: 50      #DEC     KB   ( max ram need)
  flash_buf: 16     #DEC     KB   ( buffer size)
  flash_sector: 4096 #DEC     byte ( flash sector)
  diff_mode: 010    #BIN
  double_control: 1
flash:
  base_address: 0x8000000
  run_base_address: 0x18000000 # base address of Storage
  sector: 4096       # Bytes
  size: 8388608      # $(sector count) * sector

partitions:
  - { name: bmtb,   address: 0x8000000, size: 0x001000 }
  - { name: boot,   address: 0x8001000, size: 0x010000 }
  - { name: imtb,   address: 0x8011000, size: 0x002000 }
  - { name: prim,   address: 0x8013000, load_addr: 0x18000000, size: 0x280000, verify: true, update: DIFF}
  - { name: cpu1,   address: 0x8293000, size: 0x080000, update: DIFF }
  - { name: cpu2,   address: 0x8313000, size: 0x300000, update: DIFF }
  - { name: kv,     address: 0x8613000, size: 0x008000 }
  - { name: misc,   address: 0x861B000, size: 0x180000 }
  - { name: spiffs, address: 0x879B000, size: 0x020000 }
```
