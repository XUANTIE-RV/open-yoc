## Title:  事件类型定义



| BLE Stack事件定义                      |                                                         |
| -------------------------------------- | ------------------------------------------------------- |
| EVENT_STACK_INIT = EVENT_BLE,          | 初始化事件                                              |
| EVENT_GAP_CONN_CHANGE,                 | 连接事件（连接，未连接,Err）                            |
| EVENT_GAP_DEV_FIND,                    | 发现设备事件                                            |
| EVENT_GAP_CONN_PARAM_REQ,              | 连接请求事件(Interval_min,Interval_max,latency,timeout) |
| EVENT_GAP_CONN_PARAM_UPDATE,           | 连接更新事件(Interval,latency,timeout)                  |
| EVENT_GAP_CONN_SECURITY_CHANGE,        | 加密等级变化事件(0,1,2,3)                               |
| EVENT_GATT_NOTIFY,                     | 通知事件                                                |
| EVENT_GATT_INDICATE=EVENT_GATT_NOTIFY, | 指向事件                                                |
| EVENT_GATT_CHAR_READ,                  | 特征读事件                                              |
| EVENT_GATT_CHAR_WRITE,                 | 特征写事件                                              |
| EVENT_GATT_INDICATE_CB,                | 指向返回事件                                            |
| EVENT_GATT_CHAR_READ_CB,               | 特征读返回事件                                          |
| EVENT_GATT_CHAR_WRITE_CB,              | 特征写返回事件                                          |
| EVENT_GATT_CHAR_CCC_CHANGE,            | 特征值变化事件                                          |
| EVENT_GATT_MTU_EXCHANGE,               | 最大传输单元交换事件                                    |
| EVENT_GATT_DISCOVERY_SVC,              | 发现服务事件                                            |
| EVENT_GATT_DISCOVERY_INC_SVC,          | 发现增加服务事件                                        |
| EVENT_GATT_DISCOVERY_CHAR,             | 发现特征事件                                            |
| EVENT_GATT_DISCOVERY_CHAR_DES,         | 特征描述事件                                            |
| EVENT_GATT_DISCOVERY_COMPLETE,         | 发现完成事件                                            |
| EVENT_SMP_PASSKEY_DISPLAY,             | 安全密钥显示事件                                        |
| EVENT_SMP_PASSKEY_CONFIRM,             | 安全密钥确认事件                                        |
| EVENT_SMP_PASSKEY_ENTER,               | 密钥输入事件（对应地址）                                |
| EVENT_SMP_PAIRING_CONFIRM,             | 配对确认事件                                            |
| EVENT_SMP_PAIRING_COMPLETE             | 配对完成事件                                            |
| EVENT_SMP_CANCEL,                      | 配对取消事件                                            |
| EVENT_STACK_UNKNOWN,                   | 无效事件                                                |