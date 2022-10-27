# 概述
实现ADC按键，同时扩展支持GPIO  
- 支持多个ADC引脚
- 支持ADC和GPIO的混合按键
- 支持不同引脚的组合键
- 支持GPIO电平变化检测

# 示例代码

```c
#include <yoc/adc_key_srv.h>

#define KEY_ID1 (1<<0)
#define KEY_ID2 (1<<1)
#define KEY_ID3 (1<<2)
#define KEY_ID4 (1<<3)
#define KEY_ID5 (1<<4)
#define KEY_ID_DBG (1<<5)

static int key_event_process(key_event_type_t event_type, int key_id, int rt_value)
{
    LOGD(TAG, "event_type=%d key_id=%x value=%d", event_type, key_id, rt_value);

    /* 状态可能会和按键同时发生 */
    if (event_type == KEY_SWITCH_CHANGE) {
    
    }

    /* 按键处理 */
    if (event_type == KEY_COMBO_CLICK) {
        /* 组合键处理 */
    } else {
        /* 单键处理 */
        switch (key_id) {
            case KEY_ID1:
                if (event_type == KEY_S_CLICK) {
                    app_volume_inc(0);
                }
                break;
            case KEY_ID2:
                if (event_type == KEY_S_CLICK) {
                    app_player_reverse();
                } else if (event_type == KEY_LONG_PRESS) {
                    app_player_stop();
                    wifi_pair_start();
                }
                break;
            case KEY_ID3:
                if (event_type == KEY_S_CLICK) {
                    app_volume_dec(0);
                }
                break;
            case KEY_ID4:
                app_volume_mute();
                break;
            case KEY_ID5:
                break;
            default:
                ;
        }
    }

    return 0;
}

void app_button_init(void)
{
    /* app_adc_value_read ADC读取接口需要应用实现 */
    key_srv_init(app_adc_value_read, key_event_process);

    key_srv_config(0, 0, 0, KEY_AD_VAL_OFFSET);
    /* 一个ADC引脚 5个按键 */
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL1, KEY_ID1);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL2, KEY_ID2);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL3, KEY_ID3);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL4, KEY_ID4);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL5, KEY_ID5);

    /* GPIO按键的支持 */
    //key_srv_add_keyid(PA2, KEY_TYPE_GPIO, 1, KEY_ID_DBG);

    //key_srv_add_keyid(PA2, KEY_TYPE_ADC, 10, KEY_ID_DBG);

    /* GPIO电平变化检测 */
    //key_srv_add_keyid(PA2, KEY_TYPE_GPIO_SWITCH, 0, KEY_ID_DBG);

    key_srv_start();
}
```