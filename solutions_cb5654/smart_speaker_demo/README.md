# 1. 开发环境
## 1.1 Linux环境
**工具链安装**  
该步骤可以忽略，开始编译也会自动下载安装
```
wget "http://cop-image-prod.oss-cn-hangzhou.aliyuncs.com/resource/420257228264570880/1574927493455/csky-elfabiv2-tools-x86_64-minilibc-20191122.tar.gz" -O csky-elfabiv2-tools-x86_64-minilibc-20191122.tar.gz

mkdir -p ~/gcc-csky-abiv2
tar xf csky-elfabiv2-tools-x86_64-minilibc-20191122.tar.gz -C ~/gcc-csky-abiv2
echo "export PATH=$HOME/gcc-csky-abiv2/bin:$PATH" >> ~/.bashrc
```
**编译环境安装**

```
#安装python3
sudo apt-get update
sudo apt-get install python3 python3-pip python-pip
```

```
#切换到python3
sudo update-alternatives --install /usr/bin/python python /usr/bin/python2 90
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 100

#确认是否切换到python3，如果默认不是python3，请选择对应的配置id
sudo update-alternatives --config python
```

```
#安装编译工具
sudo pip install yoctools==1.0.46 -i https://mirrors.aliyun.com/pypi/simple/
sudo pip install yoctools==1.0.46 -i https://pypi.tuna.tsinghua.edu.cn/simple/

#工具的卸载，安装过程不需要执行
sudo pip uninstall yoctools

#其他问题
如果卸载过scons，请执行
sudo mkdir /usr/local/share/man/man1
然后再走安装过程
```

## 1.2 CDK基础开发环境
参考OCC上开发板的快速上手

# 2. 应用基础功能

## 2.1 连接路由器
**WiFi配网**
默认为手机热点配网方式，可通过命令行切换其他配网方式。
命令行输入： app wifi_prov softap/dev_ap/smartconfig
- softap：手机热点网页配网
- dev_ap：阿里智能生活设备热点配网
- smartconfig：阿里智能生活一键配网

***手机热点网页配网***
默认手机热点网页配网模式，长按S7按钮进入配网模式，用手机查看YoC开头的热点进行连接。连接后自动弹出授权界面，选择无线名称，并输入密码

***阿里智能生活配网***
打开手机云智能APP，扫描产品二维码后，进入配网界面，根据提示进行操作。

**命令行配置**

```
#重启生效
kv set wifi_ssid test
kv set wifi_psk 12345678
```
## 2.2 语音交互
对接阿里云(MIT版本)
- 支持天气、十万个为什么等语音交互
- 支持播放器控制等语义解析
- 支持自定义字符串本地匹配解析
- 不支持音乐播放

语音示例，需要“宝拉宝拉”进行唤醒再说命令词  
|功能|语音命令词|说明|
|-|-|-|
|增大音量|声音大一点|云端NLP|
|降低音量|声音小一点|云端NLP|
|最大音量|声音调到最大|云端NLP|
|最大音量|声音调到最小|云端NLP|
|播放测试音|播放http协议MP3音频|本地字符匹配|
|播放标准音|播放http协议的1KHz MP3音频|本地字符匹配|
|播放低频音|播放http协议的300Hz MP3音频|本地字符匹配|
|闲聊|你叫什么名字||
|闲聊|一加一等于几||
|闲聊|地球有多大||

## 2.3 音频播放命令
```
#播放1K正弦波10s
ai sin 10

#停止播放
ai stop 255

#播放内置通知音
ai notify 0~12

#音量调整
ai vol +
ai vol -
ai vol 60

#播放网络音频
ai play http://192.168.1.100/1.mp3

#播放SD卡音频,注意是三个杠
ai play file:///fatfs0/1.mp3

#播放CENC加密MP4资源（Major_Brand信息对应密钥）
kv set <major brand> <decrypt key>

#关闭重采样
ai res 0
```

## 2.4 LED控制指令（MIT V2开发板适用）

- 呼吸灯

  ```
  app led_rgb <led_id> <mode> <dime>
  <led_id>: 0~2
  <mode>: 0/1/2/3, off/on/flash/breath
  <dime>: 0~255
  ```

- 排灯

  ```
  app led_array <led_id> <dime>
  <led_id>: 0~11/all
  <dime>: 0~255
  ```

## 2.5 接口测试

### 开启关闭PA功放
app pa 0/1

### GPIO输出测试
```
gpio set id 0/1
gpio get id
    Name  ID
    PA0,	0
    PA1,	1
    PA2,	2
    PA3,	3
    PA4,	4
    PA5,	5
    PA6,	6
    PA7,	7
    PB0,	8
    PB1,	9
    PB2,	10
    PB3,	11
    PB4,	12
    PB5,	13
    PB6,	14
    PB7,	15
    PB8,	16
    PC0,	17
    PC1,	18
    PC2,	19
    PC3,	20
    PC4,	21
    PC5,	22
    PC6,	23
    PC7,	24
    PC8,	25
    PC9,	26
    PC10,	27
    PC11,	28
    PC12,	29
    PC13,	30
    PD0,	31
    PD1,	32
    PD2,	33
    PD3,	34
    PD4,	35
    PD5,	36
    PD6,	37
    PD7,	38
    PD8,	39
    PD9,	40
    PD10,	41
```
### ADC 测试
读取ADC采样值，PA1~PA6对应ADC 1~6 六个通道
```
gpio adc 1~6
```
### SD卡
```
#加载SD卡，重启生效
kv setint fatfs_en 1

#显示卡内文件
ls /fatfs0/
```

## 2.6 闹铃功能命令

闹铃命令：
```shell
app clock func_id clodk_id period hh mm ss
```
新建一个闹铃
func_id为2， clock_id为0
如：新增一次性22:05:00的闹铃，则命令为
```shell
app clock 2 0 0 22 05 00
```
修改一个闹铃
func_id为2，clock_id为待修改的id
如修改id=1的闹铃，则命令为
```shell
app clock 2 1 2 10 30 00
```
删除一个闹铃
func_id为1，clock_id为待删的闹铃
如：删除id=2的闹铃，则命令为
```shell
app clock 1 2
```

## 2.7 文件上传下载

**下载网络文件至SD卡**

```
app http_download <save direcotry> <download url>
<save direcotry>: 保存文件目录
<download url>: 下载URL，仅支持HTTP协议
```

**SD卡文件上传至主机**

```
#启动主机Websocket服务器(python版本3.5及以上)
python ./tools/record_server/server.py <ip:port> <save dir>
<ip:port>: 本机IP地址及websocket监听端口
<save dir>: 文件保存路径

#发送SD卡文件至服务器
app ws_file_send <host ip> <port> <file path>
<host ip>: 服务器IP地址
<poort>: 服务器Websocket服务端口
<file path>: 待发送文件绝对路径
```

# 3. 应用功能配置

## 3.1 修改提示音
   集成的提示音在app/sr/audio目录中  
   支持wav和mp3，唤醒响应的提示音可用wav来提高响应速度  
   把音频文件放到audio目录总，在linux shell中执行  
   sh audio_bin2c.sh  
   输出音频的.c文件和audio_res.h

## 3.2 EQ 功能
默认启动EQ，但未配置参数  
若要关闭查看文件app/include/app_config.h
```
#define APP_EQ_EN 1 /* 软件EQ使能配置 */
```
命令行配置示例
```
#配置
ai eqparam "{100,-20.0,1.0}{200,-20.0,1.0}{400,-20.0,1.0}{1000,3.0,1.0}"

#删除配置
ai eqparam null
```
启动默认配置
```
参考 app/src/app_player.c中的函数app_player_init
```

## 3.3 语音DSP固件切换(CB5654开发板)
方案支持多种固件，默认使用的是mit固件

### 切换到T-HEAD固件
- 切换组件
组件voice_mit修改为voice_thead

- 编译烧录
命令行模式
```
make clean
make
make flash dsp
make flash
```
CDK方式  
工程节点rebuil编译完成后，再工程节点右键，菜单"Open Flash Programmer"->"YoC-CB5654-DSP"
弹出烧写界面，点击"Start"按钮烧写，然后再用工具栏烧写按钮烧写应用。

## 3.4 休眠唤醒算法切换
用户可切换两种休眠唤醒算法，只需修改app/include/app_config.h中对应宏开关
- 简单唤醒算法：占用SRAM内存资源少，无法区分人声和噪音
```
#define CONFIG_TEST_LPMVAD 1
```
- 复杂唤醒算法：占用SRAM内存资源多，可以区分人声和噪音
```
#define CONFIG_MIT_LPMVA 1
```

## 3.5 云切换
方案支持两种云端对接，aliyun和demo方案，默认使用的是aliyun方案
### Makefile
- 修改package.yaml  

切换组件 aui_aliyunnls 修改为 aui_cloud

- 重新编译

### CDK
- 删除组件

Packages中选择aui_aliyunnls右键，选择Remove
- 添加组件

右键点击Packages节点，选择Packages Runtime Management,Avalible选项卡，Search栏输入aui_cloud回车，点击组件的install按钮

# 4. 低功耗
系统默认没有开启低功耗

- 手动进入低功耗
```shell
#安静环境下输入下面命令，进入低功耗
app lpm 1
```

- 开启低功耗  
```shell
#配置好重启生效
kv setint lpm_en 1
```
若外部安静，会倒计时30秒进入低功耗,外界有声音者唤醒  
检测时间修改app_config.h的APP_LPM_CHECK_TIME宏

app lpm 1进入vad模式，adc任意按键均可vad模式唤醒

- 模拟电源键开关机
默认采用PA1的ADC S9键来作为电源键，系统正常运行时，长按ADC S9键进入standby深度睡眠
再次长按ADC S9系统唤醒并进入正常运行模式；ADC S9短按可唤醒重启但是会立刻再次进入深度睡眠
其他按键可唤醒重启但是立即进入深度睡眠

# 5. 升级
系统默认60秒检查一次升级，升级需要进行如下配置  
```
#设置设备ID
kv set device_id 3a1ad548044000006230e778e3b3ec26

#设置产品类型
kv set model Model2

#使能FOTA检测
kv setint fota_en 0/1

#若调整ota服务器，参考如下命令，默认就是如下URL
kv set otaurl http://occ.t-head.cn/api/image/ota/pull

```

# 6. 音频算法相关
## DSP调试命令

|命令|说明|
|-|-|
|yv debug 1| 开启DSP调试模式，应用无响应 |
|yv debug 0| 关闭DSP调试模式 |
|yv asr -1 | 关闭算法，可以录制数据 |
|yv asr 0 | 关闭唤醒事件，执行算法，有vad事件|
|yv asr 1 | 正常流程，有所有事件|
|yv asr 257 |正常流程+rtf显示|

## 音频录制说明
首先使能SD卡并重启  
kv setint fatfs_en 1

- 无应用交互录制
```
  yv debug 1
  yv sdrec 1
  ....
  yv sdrec 0
```

## 录制低功耗唤醒时刻数据
320K的sdram缓冲，双通道16K，16bit音频大概保存5s  
```
  yv debug 3
  ...
  app lpm 1
  ...

  #唤醒后等待 [DSP]Mem record full
  yv memrec 0
  #等待Record End
```

## websocket录制
```
#服务端
  sudo python server.py 172.16.33.208:8090 filesavedir

#设备端
  yv debug 1
  yv micrec start ws://172.16.33.208:8090 micdata.pcm
  ...
  yv micrec stop
```

# 7. 其他
## WiFi高级命令
```
#iwpriv wifi_on
#修改mac地址
iwpriv wifi_debug set_mac 00E04C87F004

#低功耗配置
iwpriv wifi_debug ps_on/ps_off

#驱动层获取mac
iwpriv wifi_debug get_mac
```

## 切换开发板(MIT-EVD-V2)
如需从CB5654开发板切换到MIT-V2开发板，需执行命令sh ./tools/change_mit_board.sh

