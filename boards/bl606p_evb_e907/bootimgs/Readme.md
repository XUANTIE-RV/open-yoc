## 文件说明
| 文件 | 说明 |
| - | - |
|boot2.bin| 博流一级boot，跳转到YoC boot，有引导产测程序功能|
|boot.bin| YoC boot 验证及Fota功能都在这里实现|
|bootmini.bin| 来源与boot2，用于支持jtag烧写|
|kp| 安全引导默认安全信息，需要写入kp分区才能正常引导|

## 其他说明
### bootmini.bin
由于博流烧写会附加一些格式信息所以boot2.bin直接烧写是无法启动的，
所以需要重新制作bootmini.bin。  

### 制作方法
- 使用博流的图形工具烧写, 无需真实连接设备，按下烧写按钮后会创建烧写文件`chips\bl606p\img_create_iot\whole_flash_data.bin`
- 裁剪出上述文件的前64K
```
dd if=whole_flash_data.bin of=bootmini.bin bs=1024 count=64
```
