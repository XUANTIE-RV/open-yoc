# 概述
IoT小程序各个场景的benchmark程序

# 调试
## 打开持续渲染

需要在 components/iot_miniapp_sdk/resources/cfg.json 配置文件的 debuger 选项中增加一个配置
```
	"debugger": {
		"enable": true,
		"render_continuously": true
	},
```
默认阈值了cfg_render.json,可以使用下面命令切换
```
# 打开持续渲染
cp /resources/cfg.json /resources/cfg.json.bak
cp /resources/cfg_render.json /resources/cfg.json
reboot

# 恢复默认配置
mv /resources/cfg.json.bak /resources/cfg.json
reboot
```

# 使用说明
## 小程序构建环境
https://www.yuque.com/wcye0k/haasui/lytxwk
注：Nodejs可以使用最新版本，

## 使用vscode开发环境
https://www.yuque.com/wcye0k/haasui/sdlgd8

## 模拟器使用
https://www.yuque.com/wcye0k/haasui/uymera

# 命令行
```
#构建-调试模式
aiot-cli -p

#构建-发布模式
aiot-cli -c -q -p

#启动模拟器
aiot-cli simulator ./

#更新到设备
aiot-cli upload -n -h 172.16.0.130 --port 5556 -p index

```
