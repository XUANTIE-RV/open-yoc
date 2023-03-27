# the bsp for cv181xh

# 关于KP文件和签名

keystore目录下的key.pem和pubkey.pem是一对公私钥，由用户自己生成并保管。

## RSA公私钥生成

1. PEM私钥格式文件

```bash
openssl genrsa -out key.pem 1024
```

2. PEM公钥格式文件

```bash
openssl rsa -in key.pem -pubout -out pubkey.pem
```

## KP文件生成

执行以下命令生成kp.bin文件，并拷贝到`bootimgs`目录下，重命名为kp。

```bash
product kp kp.bin -t RSA1024 -dt SHA1 -k keystore/pubkey.pem
cp kp.bin bootimgs/kp
```

## 固件签名

固件签名已经在`script/aft_build.sh`脚本中默认集成，会使用`keystore/key.pem`文件进行签名。

## FOTA镜像签名

FOTA镜像签名已经在`script/aft_build.sh`脚本中默认集成，会使用`keystore/key.pem`文件进行签名。

# 支持EMMC

## 分区
替换config.yaml文件
cp configs/config.yaml.emmc configs/config.yaml

## 烧写
chip组件chip_cv181x中的package.yaml使用emmc的烧写算法脚本
```yaml
hw_info:
  arch_name: riscv
  cpu_name: c906fdv
  toolchain_prefix: riscv64-unknown-elf
  vendor_name: cvitek
  ld_script: gcc_flash.ld
  # flash_program: flash.elf
  flash_program: flash_emmc.elf
```

# 切换板级引脚配置

将package.yaml中的CONFIG_BOARD_RUIXIANG_BOARD宏定义关闭即切换到开发板配置。

```shell
...
defconfig:
  #CONFIG_BOARD_RUIXIANG_BOARD
...
```

将package.yaml中的CONFIG_BOARD_RUIXIANG_BOARD宏定义打开即切换到产品板配置。

```shell
...
defconfig:
  CONFIG_BOARD_RUIXIANG_BOARD
...
```

更改后，重新编译即可。