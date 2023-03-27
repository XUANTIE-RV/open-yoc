# 简介
蓝牙MESH网络中，Provisioner 是不可缺失的角色，它可以将一个未配网的设备加入到 Mesh 网络中，为该节点分配网络密钥、IV 索引以及节点地址。同时 Provisioner 支持配置节点的各项参数，包括应用密钥，订阅和发布地址，开关 Relay/Friend/Proxy 特性等。
本文将介绍如何使用 Mesh Node 组件和 Mesh Model 组件实现 Provisioner 的功能，并且配置一个 Mesh 灯控节点，实现控制。

# 应用开发
MESH Provisioner示例实现了如下几个功能：
● provisioner scan [1/0] [timeout]:开启/关闭未扫描入网设备。
    timeout:扫描超时时间。
● provisioner add [dev_addr] [addr_type] [uuid] [oob_info] [bearer] [appkey]:添加扫描到的未入网设备。
    dev_addr: 设备MAC地址，格式为xx:xx:xx:xx:xx:xx
    addr_type: 设备地址类型 0:Public / 1:random。
    Uuid: 设备UUID,16个字节。
    oob_info: 0x00: NO OOB / 0x01:static oob
    bearer: 0x01:PB-ADV / 0x02:PB-GATT
    auto_add_appkey: 0(不⾃动添加appkey)/1(⾃动添加appkey),默认为1
● provisioner del [dev_id]:删除已入网设备。
    dev_id:设备编号
● provisioner onoff [dev_id] [0/1]:控制已入网设备 LED 亮/灭。
    dev_id:设备编号

# 示例
/*开启扫描*/
provisioner scan 1
/*显示未入网设备信息*/
[GATEWAY_MESH]<timer_task>Found mesh dev:CC:38:E3:82:BA:C0,addr_type:00,oob_info:00,beraer:01
[MESH_PROVISIONER]<timer_task>Found Mesh Node:cc38e382bac011e886d15f1ce200de02,cc38e382bac011e886d15f1ce200de02
/*
dev_addr:CC:38:E3:82:BA:C0
addr_type:00
uuid:cc38e382bac011e886d15f1ce200de02
oob_info:00
bearer:01
*/
/*添加未入网设备*/
provisioner add CC:38:E3:82:BA:C0 00 cc38e382bac011e886d15f1ce200de02 00 01
/*添加节点log*/
[MESH_PROVISIONER]<mgmt_event_process_task>Subdev[1] status:01
/*
Subdev[1]:1为设备编号
dev_id:1
*/
/*开启子设备 LED 灯，unicat_addr 值从 log 中获取*/
provisioner onoff 1 1
/*关闭子设备 LED 灯，unicat_addr 值从 log 中获取*/
provisioner onoff 1 0
/*将子设备从 mesh 网络中移除*/
provisioner del 1


# 适用说明
本示例适用于
● IoTGW_CB800开发板与d1开发板



