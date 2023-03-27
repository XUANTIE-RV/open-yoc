# the bsp for F133


# 注意

64位系统上要运行打包脚本需要先在终端里面执行以下命令开启相应的服务，否则会出现错误。

```bash
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libc6:i386
sudo apt-get install lib32ncurses5
sudo apt-get install lib32z1
sudo apt-get install lib32stdc++6

sudo apt install qemu-user-static
sudo update-binfmts --install i386 /usr/bin/qemu-i386-static --magic '\x7fELF\x01\x01\x01\x03\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x03\x00\x01\x00\x00\x00' --mask '\xff\xff\xff\xff\xff\xff\xff\xfc\xff\xff\xff\xff\xff\xff\xff\xff\xf8\xff\xff\xff\xff\xff\xff\xff'
sudo service binfmt-support start
```

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

固件签名已经在脚本中默认集成，会使用`keystore/key.pem`文件进行签名。
