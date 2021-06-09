## 简介

KV模块测试用例分为接口测试和功能测试，接口测试case在文件kv_api.c中，功能测试case在kv_function.c中。

## 接口测试执行方法

编译程序烧写进板子后，执行test kvapi指令，查看返回结果，执行通过的case不会打印，执行失败的case返回失败的接口信息。

## 功能测试执行方法

功能测试依赖于aos组件，因此如果调用此方法，必须支持aos组件。
打开宏CONFIG_FUNCTIONAL_TEST，程序编译通过后，烧写进板子，在串口执行如下指令：

指令说明如下：

（1）kv写入指令

指令：kv set|serstr|setint|setfloat key vlaue

功能：写入不同类型的kv文件

参数说明：

kv：kv模块；

set|serstr|setint|setfloat：写入value的类型，set表示任何类型的值，setstr表示字符串型值，setint表示写入int型值，setfloat表示写入float型值；

key：写入的key值；

value：写入的vlaue值；

返回值：若是成功，打印success，若是失败，打印failed

（2）kv读取指令

指令：kv get|getstr|getint|getfloat

功能：读取不同类型的kv文件的值

参数说明：

kv：kv模块；

get|getstr|getint|getfloat：读取的value的类型，get表示任何类型的值，getstr表示读取字符串型的值，getint表示读取int型值，getfloat表示读取float型值；

key：读取的key值；

（3）kv删除指令

指令：kv del key

功能：删除某个kv文件

参数说明：

kv：kv模块；

del ：删除动作

key：删除的key值；

返回值：

（4）清除kv文件

指令：kv reset

功能：清除所有的kv信息