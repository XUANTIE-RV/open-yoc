# 概述
IoT miniapp示例小程序，包含首页,下拉,场景及组件使用示例.

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
