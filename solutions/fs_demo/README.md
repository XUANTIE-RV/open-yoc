# 概述
本示例通过vfs 文件系统的功能。
可以通过cli来操作文件系统内的文件。
示例支持 littlefs，ext4fs，fatfs

# 使用

## CDK

在CDK的首页，通过搜索fs_demo，可以找到fs_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行

需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install fs_demo
```

### 切换系统内核

#### 切换到RHINO

默认支持RHINO 无需切换,如果需要从RT-Thread修改到RHINO，则根据下一节内容`切换到RT-Thread`下面的内容修改回去。

#### 切换到RT-Thread

##### D1平台

比如在develop分支上面，需要修改`component/sdk_chip_d1/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`ble_host`,`freertos` 最终如下所示:
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
  #- ble_host: develop
```

##### bl606P平台

比如在develop分支上面，需要修改`component/sdk_chip_bl606p_e907/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`bl606p_bthost`,`bl606p_blimpls`,`freertos`如下所示
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
  #- bl606p_bthost: develop
  #- bl606p_blimpls: develop
  ......
```

##### ch2601平台

比如在develop分支上面，需要修改`component/sdk_chip_ch2601/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`freertos`如下所示
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
```

##### f133平台

比如在develop分支上面，需要修改`component/sdk_chip_f133/package.ymal`文件中的`depends`部分：
将`rtthread`这个注释打开，需要注释掉这几个组件 `rhino`,`rhino_arch`,`rhino_pwrmgmt`,`ble_host` ,`freertos`最终如下所示:
```yaml
depends:
  ......
  #- rhino: develop
  #- rhino_arch: develop
  #- rhino_pwrmgmt: develop
  #- freertos: develop
  - rtthread: develop
  ......
  #- ble_host: develop
  ......
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

#### D1平台

1. 编译

```bash
make clean
make SDK=sdk_chip_d1
```

2. 烧写

```bash
make flashall SDK=sdk_chip_d1
```

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

#### ch2601平台

1. 编译

```bash
make clean
make SDK=sdk_chip_ch2601
```

2. 烧写

```bash
make flashall SDK=sdk_chip_ch2601
```

#### f133平台

1. 编译

```bash
make clean
make SDK=sdk_chip_f133
```

2. 烧写

```bash
make flashall SDK=sdk_chip_f133
```


#### cv181xh_huashanpi_evb 平台

1. 编译

```bash
make clean
make SDK=sdk_chip_cv181xh_bga
```

2. 烧写

```bash
make flashall SDK=sdk_chip_cv181xh_bga
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 配置

## cv181xh_huashanpi_evb 平台

编译之前需要在 config.yaml 中增加分区
```
mtb_version: 4
chip: cv181x

diff:
  fota_version: 0
  ram_buf: 50           #DEC     KB   ( max ram need)
  flash_buf: 16         #DEC     KB   ( buffer size)
  flash_sector: 4096    #DEC     byte ( flash sector)
  diff_mode: 010        #BIN
  double_control: 1

storages:
  - name: emmc_boot1        # eflash/spinor/spinand/emmc_boot1/emmc_boot2/emmc_user/emmc_rpmb/sd/usb
    base_address: 0         # 存储基地址，比如FLASH的基地址
    run_base_address: 0x80000000 # base address of Storage
    block: 512              # Bytes
    size: 4194304           # Total Bytes
  - name: emmc_boot2        # eflash/spinor/spinand/emmc_boot1/emmc_boot2/emmc_user/emmc_rpmb/sd/usb
    base_address: 0         # 存储基地址，比如FLASH的基地址
    run_base_address: 0x8000C000 # base address of Storage
    block: 512              # Bytes
    size: 4194304           # Total Bytes
  - name: emmc_user         # eflash/spinor/spinand/emmc_boot1/emmc_boot2/emmc_user/emmc_rpmb/sd/usb
    base_address: 0         # 存储基地址，比如FLASH的基地址
    run_base_address: 0x80040000 # base address of Storage
    block: 512              # Bytes
    size: 536870912         # Total Bytes
  - name: sd                # eflash/spinor/spinand/emmc_boot1/emmc_boot2/emmc_user/emmc_rpmb/sd/usb
    base_address: 0         # 存储基地址，比如FLASH的基地址
    run_base_address: 0x8000C000 # base address of Storage
    block: 512              # Bytes
    size: 0                 # Total Bytes
    hot_plug: true
  - name: usb               # eflash/spinor/spinand/emmc_boot1/emmc_boot2/emmc_user/emmc_rpmb/sd/usb
    base_address: 0         # 存储基地址，比如FLASH的基地址
    run_base_address: 0x8000C000 # base address of Storage
    block: 512              # Bytes
    size: 0                 # Total Bytes
    hot_plug: true

# must erase_size(512 * 1024) align
partitions:
  - { name: boot0,  address: 0x0000000, size: 0x080000, index: 0, file: boot0.emmc }
  - { name: boot,   address: 0x0000000, size: 0x080000, index: 1, file: boot }
  - { name: imtb,   address: 0x0000000, size: 0x100000, index: 2 }               # imtb must be in use area, two erase_size
  - { name: envab,  address: 0x0100000, size: 0x180000, index: 2 }               # for ab switch, size must be 2*erase_size+1024B at least
  - { name: prima,  address: 0x0280000, size: 0x800000, index: 2, file: prim, load_addr: 0x80040000, verify: false, update: AB }
  - { name: primb,  address: 0x0A80000, size: 0x800000, index: 2, load_addr: 0x80040000, verify: false }
  - { name: weighta,address: 0x1A80000, size: 0x900000, index: 2, file: weight, update: AB }
  - { name: weightb,address: 0x2380000, size: 0x900000, index: 2 }
  - { name: ext4,   address: 0x2C80000, size: 0x800000, index: 2, file: ext4.img }
  - { name: kv,     address: 0x3880000, size: 0x200000, index: 2 }
  - { name: afrot,  address: 0x3A80000, size: 0x080000, index: 2 }
  - { name: kp,     address: 0x3B00000, size: 0x080000, index: 2 }                                      # size = 0 let device decide size
  - { name: fatfs,  address: 0x0000000, size: 0, index: 4 }                                        # size = 0 let device decide size

如上 config.yaml 中，增加了一个 fatfs 分区，增加了一个 usb 的 storages，并需要通过分区的的 index 参数吧，fatfs 和usb 关联起来，这样，分区操作的时候就会操作对应的设备了。
```

# 启动
```
设备如果有littlfs ext4 fatfs将会在启动的时候挂在这些文件系统

如果是sd 或者是 usb，则需要输入cli mount命令来挂载
```

## 调试

查看USB设备
```
>lsusb -t
/: Hub 01, ports=1, is roothub
    |__Port 1,Port addr:0x02, ID 058f:6254, If 0,ClassDriver=hub
/: Hub 02, ports=4, mounted on Hub 01:Port 1
    |__Port 2,Port addr:0x03, ID 05e3:0610, If 0,ClassDriver=hub
/: Hub 03, ports=4, mounted on Hub 02:Port 2
    |__Port 2,Port addr:0x04, ID 0bda:8153, If 0,ClassDriver=NULL
    |__Port 4,Port addr:0x05, ID 23a9:ef18, If 0,ClassDriver=msc
```

可以看到有 msc 的usb 设备，说明 u盘插入成功

挂在文件系统
```
>fs mountfatfs
[mtb][D] [part_blockdev_info_get, 33]info->base_addr:0x0
[mtb][D] [part_blockdev_info_get, 34]info->sector_size:0x0
[mtb][D] [part_blockdev_info_get, 35]info->block_size:0x200
[mtb][D] [part_blockdev_info_get, 36]info->erase_size:0x200
[mtb][D] [part_blockdev_info_get, 37]info->device_size:0x753000000
[mtb][D] [part_blockdev_info_get, 38]info->boot_area_size:0x0
```

以上打印说明挂在成功

可以通过 ls命令查看u盘里面的内容
```
>ls -l /mnt/sd
d--------- root root 0B System Volume Information
---------- root root 0B rwspeed.tmp
```

还有很多其他文件系统相关命令如下
```
help
================ AliOS Things Command List ==============
help           : print this
cat            : read file
cd             : change current working directory
cp             : copy file
df             : show fs usage info
echo           : echo strings
ll             : the same as 'ls -kl'
ls             : list file or directory
lsfs           : list the registered filesystems
mkdir          : make directory
mv             : move file
rm             : remove file
touch          : create empty file
pwd            : print name of current working directory
hd             : the same as 'hexdump -C'
hexdump        : dump binary data in decimal
rwspeed        : get seq read/write speed
fs_task        : fs multi-task test
****************** Commands Num : 56 *******************

================ AliOS Things Command end ===============
```


