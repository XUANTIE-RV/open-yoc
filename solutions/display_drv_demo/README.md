# 概述
display_drv_demo是display和input驱动的示例代码程序，演示了display和input驱动的方法

# CDK
在CDK的首页，通过搜索关键字lcd，可以找到display_drv_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

# 启动
烧录完成之后系统会自动启动，串口会有打印输出。

```cli
###YoC###[Oct 19 2022,08:30:35]
cpu clock is 1008000000Hz
(cli-uart)# [   0.870]<D>[drv_touch]<app_task>0X39 0X31 0X31 0 0

[   0.880]<D>[drv_touch]<app_task>[TP] Found chip gt911

[   0.890]<D>[app]<app_task>disp info x_res 800, y_res 480, bits_per_pixel: 16, supported_feature: 2, pixel_format: RGB_565
```

# 显示
启动后可以通过串口命令进行显示控制

命令如下
```
display
usage:
	disp brightness [num]
	disp blank [0/1]
	disp write [red color num] [green color num] [blue color num]
	disp write_async [red color num] [green color num] [blue color num]
	disp pan_dispaly [red color num] [green color num] [blue color num]
```

对于 显示格式为 ARGB_8888 的屏幕

同步刷屏，红色
```
display write 255 0 0
```

异步刷屏，绿色
```
display write_async 0 255 0
```

fb刷屏，蓝色
```
display pan_display 0 0 255
```

如果是 RGB_565 的屏幕，则红绿蓝的数值分别是 31，63，31

屏幕亮度调整到最大
```
display brightness 255
```

关闭屏幕
```
display blank 0
```

## 触摸
可以通过触摸屏幕触发如下打印：

```
[11144.930]<D>[app]<touch>touch pressed x: 318 y: 249
[11144.940]<D>[app]<touch>touch move x: 318 y: 249
[11144.940]<D>[app]<touch>touch move x: 318 y: 249
[11144.950]<D>[app]<touch>touch move x: 318 y: 249
[11144.960]<D>[app]<touch>touch move x: 318 y: 249
[11144.970]<D>[app]<touch>touch move x: 318 y: 249
[11144.980]<D>[app]<touch>touch move x: 318 y: 249
[11144.990]<D>[app]<touch>touch unpressed x: 318 y: 249

[11145.290]<D>[app]<touch>touch pressed x: 314 y: 243
[11145.300]<D>[app]<touch>touch move x: 314 y: 243
[11145.300]<D>[app]<touch>touch move x: 314 y: 243
[11145.310]<D>[app]<touch>touch move x: 314 y: 243
[11145.320]<D>[app]<touch>touch move x: 314 y: 243
[11145.330]<D>[app]<touch>touch move x: 314 y: 243
[11145.340]<D>[app]<touch>touch move x: 314 y: 243
[11145.350]<D>[app]<touch>touch unpressed x: 314 y: 243
```