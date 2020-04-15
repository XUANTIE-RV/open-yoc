# 概述
DHT传感驱动

# 使用示例
```c
#include <devices/dht_sensor.h>

{
    dev_t *sensor_dev;
    sensor_dht11_t sval;

    /* 驱动初始化 */
    static sensor_pin_config_t dht11_config = {PA4};
    sensor_dht11_register(&dht11_config, 0);

    /* 打开设备 */
    sensor_dev = sensor_open_id(DHT11_DEV_NAME, 0);

    /* 读取数据 */
    sensor_fetch(sensor_dev);
    sensor_getvalue(sensor_dev, (void *)&sval, sizeof(sensor_dht11_t));

    /* 关闭驱动 */
    sensor_close(sensor_dev);

}
```
# 注意
受DHT11传感器的初始化时间限制，两次采集间隔必须大于2秒以上
