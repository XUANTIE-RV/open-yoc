## Title:  Profile定义说明

# 概述

该部分描述了特性值的定义并列举出标准UUID值

## **特征值定义**

### **GATT_PRIMARY_SERVICE_DEFINE**

- 函数原型

```c
uuid_t  GATT_PRIMARY_SERVICE_DEFINE(_uuid)
```

- 功能描述

定义服务类型

- 参数描述

| IN/OUT | NAME  | DESC                        |
| ------ | ----- | --------------------------- |
| [in]   | _uuid | uuid里的 服务类型结构体uuid |

- 返回值

  无

- 注意事项

  一种应用一般只有一个

### **GATT_CHAR_DEFINE**

- 函数原型

```c
#define GATT_CHAR_DEFINE(_uuid, _props)
```

- 功能描述

特征属性定义

- 参数描述

| IN/OUT | NAME   | DESC         |
| ------ | ------ | ------------ |
| [in]   | _uuid  | 特征类型UUID |
| [in]   | _props | 属性         |

| _props（枚举）定义                           |                                    |
| -------------------------------------------- | ---------------------------------- |
| GATT_CHRC_PROP_BROADCAST=BLE_BIT(0)          | 广播属性特征                       |
| GATT_CHRC_PROP_READ=BLE_BIT(1)               | 读属性                             |
| GATT_CHRC_PROP_WRITE_WITHOUT_RESP=BLE_BIT(2) | 无需回复的写入属性                 |
| GATT_CHRC_PROP_WRITE=BLE_BIT(3)              | 需要回复的写属性                   |
| GATT_CHRC_PROP_NOTIFY=BLE_BIT(4)             | 通知属性（允许未经接受的通知特性） |
| GATT_CHRC_PROP_INDICATE=BLE_BIT(5)           | 指向属性(允许未经接受的指向特性)   |
| GATT_CHRC_PROP_AUTH=BLE_BIT(6)               | 签名认证写属性（允许签名的写特性） |
| GATT_CHRC_PROP_EXT_PROP=BLE_BIT(7)           | 扩展属性特性(附加特征属性)         |

- 返回值

  无

- 注意事项

  无

### **GATT_CHAR_VAL_DEFINE**

- 函数原型

```c
#define GATT_CHAR_VAL_DEFINE(_uuid, _perm)
```

- 功能描述

特征值定义

- 参数描述

| IN/OUT | 4NAME | DESC             |
| ------ | ----- | ---------------- |
| [in]   | _uuid | 特征描述类UUID   |
| [in]   | _perm | GATT操作许可类型 |

| _perm（枚举）定义                  |                |
| ---------------------------------- | -------------- |
| GATT_PERM_NONE=0                   | 无(仅用于通知) |
| GATT_PERM_READ=BLE_BIT(0)          | 读属性许可     |
| GATT_PERM_WRITE=BLE_BIT(1)         | 写属性许可     |
| GATT_PERM_READ_ENCRYPT=BLE_BIT(2)  | 读加密许可     |
| GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3) | 写加密许可     |
| GATT_PERM_READ_AUTHEN=BLE_BIT(4)   | 读验证许可     |
| GATT_PERM_WRITE_AUTHEN=BLE_BIT(5)  | 写验证许可     |
| GATT_PERM_PREPARE_WRITE=BLE_BIT(6) | 准备写入许可   |

- 返回值

  无

- 注意事项

  无

### **GATT_CHAR_CCC_DEFINE**

- 函数原型

```c
GATT_CHAR_CCC_DEFINE(_perm)
```

- 功能描述

客户端特征配置,可用于客户端配置触发的条件

- 参数描述

  | _perm（枚举）定义                  |                |
  | ---------------------------------- | -------------- |
  | GATT_PERM_NONE=0                   | 无(仅用于通知) |
  | GATT_PERM_READ=BLE_BIT(0)          | 读属性许可     |
  | GATT_PERM_WRITE=BLE_BIT(1)         | 写属性许可     |
  | GATT_PERM_READ_ENCRYPT=BLE_BIT(2)  | 读加密许可     |
  | GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3) | 写加密许可     |
  | GATT_PERM_READ_AUTHEN=BLE_BIT(4)   | 读验证许可     |
  | GATT_PERM_WRITE_AUTHEN=BLE_BIT(5)  | 写验证许可     |
  | GATT_PERM_PREPARE_WRITE=BLE_BIT(6) | 准备写入许可   |

- 返回值

  无

- 注意事项

  用户可定义触发的类型

### **GATT_CHAR_CUD_DEFINE**

- 函数原型

```c
GATT_CHAR_CUD_DEFINE(_value, _perm)
```

- 功能描述

用户描述GATT特征

- 参数描述

| IN/OUT | NAME   | DESC               |
| ------ | ------ | ------------------ |
| [in]   | _value | 用户描述的特征数据 |
| [in]   | _perm  | GATT操作许可类型   |

| _perm（枚举）定义                  |                |
| ---------------------------------- | -------------- |
| GATT_PERM_NONE=0                   | 无(仅用于通知) |
| GATT_PERM_READ=BLE_BIT(0)          | 读属性许可     |
| GATT_PERM_WRITE=BLE_BIT(1)         | 写属性许可     |
| GATT_PERM_READ_ENCRYPT=BLE_BIT(2)  | 读加密许可     |
| GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3) | 写加密许可     |
| GATT_PERM_READ_AUTHEN=BLE_BIT(4)   | 读验证许可     |
| GATT_PERM_WRITE_AUTHEN=BLE_BIT(5)  | 写验证许可     |
| GATT_PERM_PREPARE_WRITE=BLE_BIT(6) | 准备写入许可   |

- 返回值

  无

- 注意事项

  无

# 标准UUID定义

| 服务类型UUID |        |                                   |
| ------------ | ------ | --------------------------------- |
| GAP          | 0x1800 | Generic Access                    |
| GATT         | 0x1801 | Generic Attribute                 |
| CTS          | 0x1805 | Current Time Service              |
| DIS          | 0x180a | Device Information Service        |
| HRS          | 0x180d | Heart Rate Service                |
| BAS          | 0x180f | Battery Service                   |
| HIDS         | 0x1812 | HID Service                       |
| CSC          | 0x1816 | Cycling Speed and Cadence Service |
| ESS          | 0x181a | Environmental Sensing Service     |
| IPSS         | 0x1820 | IP Support Service                |
| MESH_PROV    | 0x1827 | Mesh Provisioning Service         |
| MESH_PROXY   | 0x1828 | Mesh Proxy Service                |

| 属性类型UUID   |        |                        |
| -------------- | ------ | ---------------------- |
| GATT_PRIMARY   | 0x2800 | GATT Primary Service   |
| GATT_SECONDARY | 0x2801 | GATT Secondary Service |
| GATT_SECONDARY | 0x2801 | GATT Secondary Service |
| GATT_INCLUDE   | 0x2802 | GATT Include Service   |
| GATT_CHRC      | 0x2803 | GATT Characteristic    |

| 特性描述UUID       |        |                                                  |
| ------------------ | ------ | ------------------------------------------------ |
| GATT_CEP           | 0x2900 | GATT Characteristic Extended Properties          |
| GATT_CUD           | 0x2901 | GATT Characteristic User Description             |
| GATT_CCC           | 0x2902 | GATT Client Characteristic Configuration         |
| GATT_SCC           | 0x2903 | GATT Server Characteristic Configuration         |
| GATT_CPF           | 0x2904 | GATT Characteristic Presentation Format          |
| VALID_RANGE        | 0x2906 | Valid Range Descriptor                           |
| HIDS_EXT_REPORT    | 0x2907 | HID External Report Descriptor                   |
| HIDS_REPORT_REF    | 0x2908 | HID Report Reference Descriptor                  |
| ES_CONFIGURATION   | 0x290b | Environmental Sensing Configuration Descriptor   |
| ES_MEASUREMENT     | 0x290c | Environmental Sensing Measurement Descriptor     |
| ES_TRIGGER_SETTING | 0x290d | Environmental Sensing Trigger Setting Descriptor |

| 特征类型UUID                            |        |                                                              |
| --------------------------------------- | ------ | ------------------------------------------------------------ |
| GAP_DEVICE_NAME                         | 0x2a00 | GAP Characteristic Device Name                               |
| GAP_APPEARANCE                          | 0x2a01 | GAP Characteristic Appearance                                |
| GAP_PPCP                                | 0x2a04 | GAP Characteristic Peripheral Preferred Connection Parameter |
| GATT_SC                                 | 0x2a05 | GATT Characteristic Service Changed                          |
| BAS_BATTERY_LEVEL                       | 0x2a19 | BAS Characteristic Battery Level                             |
| HIDS_BOOT_KB_IN_REPORT                  | 0x2a22 | HID Characteristic Boot Keyboard Input Report                |
| DIS_SYSTEM_ID                           | 0x2a23 | DIS Characteristic System ID                                 |
| DIS_MODEL_NUMBER                        | 0x2a24 | IS Characteristic Model Number String                        |
| DIS_SERIAL_NUMBER                       | 0x2a25 | DIS Characteristic Serial Number String                      |
| DIS_FIRMWARE_REVISION                   | 0x2a26 | DIS Characteristic Firmware Revision String                  |
| DIS_HARDWARE_REVISION                   | 0x2a27 | DIS Characteristic Hardware Revision String                  |
| DIS_SOFTWARE_REVISION                   | 0x2a28 | IS Characteristic Software Revision String                   |
| DIS_MANUFACTURER_NAME                   | 0x2a29 | DIS Characteristic Manufacturer Name String                  |
| IEEE_REGULATORY_CERTIFICATION_DATA_LIST | 0x2a2a | IEEE Regulatory Certification Data List characteristic UUID  |
| DIS_PNP_ID                              | 0x2a50 | DIS Characteristic PnP ID                                    |
| CTS_CURRENT_TIME                        | 0x2a2b | CTS Characteristic Current Time                              |
| MAGN_DECLINATION                        | 0x2a2c | Magnetic Declination Characteristic                          |
| HIDS_BOOT_KB_OUT_REPORT                 | 0x2a32 | HID Boot Keyboard Output Report Characteristic               |
| HIDS_BOOT_MOUSE_IN_REPORT               | 0x2a33 | HID Boot Mouse Input Report Characteristic                   |
| HRS_MEASUREMENT                         | 0x2a37 | HRS Characteristic Measurement Interval                      |
| HRS_BODY_SENSOR                         | 0x2a38 | HRS Characteristic Body Sensor Location                      |
| HRS_CONTROL_POINT                       | 0x2a39 | HRS Characteristic Control Point                             |
| HIDS_INFO                               | 0x2a4a | HID Information Characteristic                               |
| HIDS_REPORT_MAP                         | 0x2a4b | HID Report Map Characteristic                                |
| HIDS_CTRL_POINT                         | 0x2a4c | HID Control Point Characteristic                             |
| HIDS_REPORT                             | 0x2a4d | HID Report Characteristic                                    |
| HIDS_PROTOCOL_MODE                      | 0x2a4e | HID Protocol Mode Characteristic                             |
| CSC_MEASUREMENT                         | 0x2a5b | CSC Measurement Characteristic                               |
| CSC_FEATURE                             | 0x2a5c | CSC Feature Characteristic                                   |
| SENSOR_LOCATION                         | 0x2a5d | Sensor Location Characteristic                               |
| SC_CONTROL_POINT                        | 0x2a55 | SC Control Point Characteristic                              |
| ELEVATION                               | 0x2a6c | Elevation Characteristic                                     |
| PRESSURE                                | 0x2a6d | Pressure Characteristic                                      |
| TEMPERATURE                             | 0x2a6e | Temperature Characteristic                                   |
| HUMIDITY                                | 0x2a6f | Humidity Characteristic                                      |
| TRUE_WIND_SPEED                         | 0x2a70 | True Wind Speed Characteristic                               |
| TRUE_WIND_DIR                           | 0x2a71 | True Wind Direction Characteristic                           |
| APPARENT_WIND_SPEED                     | 0x2a72 | Apparent Wind Speed Characteristic                           |
| APPARENT_WIND_DIR                       | 0x2a73 | Apparent Wind Direction Characteristic                       |
| GUST_FACTOR                             | 0x2a74 | Gust Factor Characteristic                                   |
| POLLEN_CONCENTRATION                    | 0x2a75 | Pollen Concentration Characteristic                          |
| UV_INDEX                                | 0x2a76 | UV Index Characteristic                                      |
| IRRADIANCE                              | 0x2a77 | Irradiance Characteristic                                    |
| RAINFALL                                | 0x2a78 | Rainfall Characteristic                                      |
| WIND_CHILL                              | 0x2a79 | Wind Chill Characteristic                                    |
| HEAT_INDEX                              | 0x2a7a | Heat Index Characteristic                                    |
| DEW_POINT                               | 0x2a7b | Dew Point Characteristic                                     |
| DESC_VALUE_CHANGED                      | 0x2a7d | Descriptor Value Changed Characteristic                      |
| MAGN_FLUX_DENSITY_2D                    | 0x2aa0 | Magnetic Flux Density - 2D Characteristic                    |
| MAGN_FLUX_DENSITY_3D                    | 0x2aa1 | Magnetic Flux Density - 3D Characteristic                    |
| BAR_PRESSURE_TREND                      | 0x2aa3 | Barometric Pressure Trend Characteristic                     |
| CENTRAL_ADDR_RES                        | 0x2aa6 | Central Address Resolution Characteristic                    |
| MESH_PROV_DATA_IN                       | 0x2adb | Mesh Provisioning Data In                                    |
| MESH_PROV_DATA_OUT                      | 0x2adc | Mesh Provisioning Data Out                                   |
| MESH_PROXY_DATA_IN                      | 0x2add | Mesh Proxy Data In                                           |
| MESH_PROXY_DATA_OUT                     | 0x2ade | Mesh Proxy Data Out                                          |

## 



