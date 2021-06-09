## 概述
`genie_console`是一个基于aos hal uart的console组件,能完成基本的cli功能，并将数据传到genie service对接genie AT命令或二进制串口命令或用户自定义串口命令

## 组件安装

```bash
yoc init
yoc install genie_console
```

## 运行资源

不同方案下资源消耗参考

| Solution              | Code size/Bytes | Ro data/Bytes | RW Data/Bytes | BSS/Bytes |
| --------------------- | --------------- | ------------- | ------------- | --------- |
| genie_mesh_light_ctl  | 3564            | 1134          | 0             | 201       |
| genie_mesh_node_ctl   | 3568            | 1134          | 0             | 201       |
| genie_mesh_switches   | 3944            | 1439          | 0             | 201       |

## 依赖资源

hal_csi

## 组件参考

无。

