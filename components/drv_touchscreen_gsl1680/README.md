# 概述

屏幕触摸芯片gsl1680驱动对接

# 代码示例

驱动注册
```C
void drv_gsl1680_devops_register(gsl1680_gpio_pin * config);
```

## 调试

具体屏幕触摸驱动的接口调用方式可以参考display_drv_demo