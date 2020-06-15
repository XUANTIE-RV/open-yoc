# 概述

一个c语言实现的按键服务组件，支持单击、双击、长按、松开、自动消抖，可以自由设置组合按键，可用于GPIO及ADC类型按键。

## 接口定义

```C
int button_srv_init(void)
```

按键服务初始化

- 返回值：
  - 成功返回0，失败返回-1



```C
int button_init(const button_config_t b_tbl[])
```

单按键列表初始化

- 参数
  - b_tbl ：单按键列表
- 返回值：
  - 成功返回0，失败返回-1

```c
typedef struct button_config {
    int pin_id;      //按键 pin id
    int evt_flag;    //订阅的按键事件类型
    evt_cb cb;       //按键事件用户回调函数
    void *priv;      //按键事件用户回调函数的自定义参数
    int type;        //按键类型
    char name[MAX_BUTTON_NAME]; //按键名
} button_config_t;
```

```c
#define PRESS_DOWN_FLAG (1<<BUTTON_PRESS_DOWN)            //单击
#define PRESS_UP_FLAG (1<<BUTTON_PRESS_UP)                //松开
#define PRESS_LONG_DOWN_FLAG (1<<BUTTON_PRESS_LONG_DOWN)  //长按
#define DOUBLE_PRESS_FLAG (1<<BUTTON_PRESS_DOUBLE)        //双击
#define EVT_ALL_FLAG (PRESS_DOWN_FLAG | PRESS_UP_FLAG | DOUBLE_PRESS_FLAG | PRESS_LONG_DOWN_FLAG)  //以上所有事件
```

```c

typedef enum {
    BUTTON_TYPE_GPIO,/* GPIO按键 支持中断方式*/
    BUTTON_TYPE_ADC, /* ADC按键 */
} button_type_t;
```

```c
typedef void (*evt_cb)(button_evt_id_t event_id, char *name, void *priv);
```

- 参数
	- event_id ：事件类型
	- name:  按键名
	- priv:  用户自定义参数

```c
typedef enum {
    BUTTON_PRESS_DOWN = 0,       //单击
    BUTTON_PRESS_UP,             //松开
    BUTTON_PRESS_LONG_DOWN,      //长按
    BUTTON_PRESS_DOUBLE,         //双击
    BUTTON_COMBINATION,          //组合按键事件

    BUTTON_EVT_END
} button_evt_id_t;
```



```C
int button_param_cur(char *name, button_param_t *p);
int button_param_set(char *name, button_param_t *p);
```

获取/设置按键参数

- 参数
  - name：按键名
  - p	：按键参数
- 返回值：
  - 成功返回0，失败返回-1



```c
typedef struct button_param {
    int st_tmout;   //min pin active level time(soft elimination buffeting time)
    int ld_tmout;   //min long press time
    int dd_tmout;   //max double press time interval
    int active_level;
    const char *adc_name; //if button is adc_button, it has
    int range; 			//if button is adc_button, it has
    int vref; //if button is adc_button, it has
} button_param_t;
```



```c
int button_combination_init(const button_combinations_t bc_tbl[])
```

组合按键列表初始化

- 参数
  - b_tbl ：组合按键列表
- 返回值：
  - 成功返回0，失败返回-1



**注意：adc类型按键不支持组合键**

```c
typedef struct button_combinations button_combinations_t

struct button_combinations {
    int pin_id[MAX_COMBINATION_NUM]; //按键pin id
    int pin_sum; //组合按键总数
    int evt_flag; //订阅的按键事件类型
    int tmout; //组合按键间隔时长
    evt_cb cb; //按键事件用户回调函数
    void *priv; //按键事件用户回调函数的自定义参数
    char name[MAX_BUTTON_NAME]; //组合按键名
};
```



# 示例代码

```c
const static char *log_buf[] = {
    "down",
    "up",
    "down_l",
    "d_down",
    "bc",
};

static void button_evt(button_evt_id_t event_id, char *name, void *priv)
{
    LOGE(TAG, "key[%s]:%s", name, log_buf[event_id]);
}
/* 按键表 */
const static button_config_t button_table[] = {
    {APP_KEY_MUTE,    EVT_ALL_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO, "mute"},
    {APP_KEY_VOL_INC, EVT_ALL_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO,  "inc"},
    {APP_KEY_VOL_DEC, EVT_ALL_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO,  "dec"},
    {0, 0, NULL, NULL},
};

/* 组合按键表 */
const static button_combinations_t bc_table[] = {
    {
        .pin_name[0] = "mute",
        .pin_name[1] = "inc",
        .evt_flag = PRESS_LONG_DOWN_FLAG,
        .pin_sum = 2,
        .tmout = 500,
        .cb = button_evt,
        .priv = NULL,
        .name = "mute&inc"
    },
    {
        .pin_name[0] = "mute",
        .pin_name[1] = "dec",
        .evt_flag = PRESS_LONG_DOWN_FLAG,
        .pin_sum = 2,
        .tmout = 500,
        .cb = button_evt,
        .priv = NULL,
        .name = "mute&dec"
    },
    {
        .pin_sum = 0,
        .cb = NULL,
    },
};

int main()
{
    board_yoc_init(); //板级初始化
    LOGD(TAG, "%s\n", aos_get_app_version());
    button_srv_init(); // 按键服务初始化
    button_init(button_table); // 初始化按键表
    button_param_t pb;
    button_param_cur("mute", &pb); // 获取“mute”按键当前参数
    pb.ld_tmout = 5000;            // 更改长按时间为5000ms
    button_param_set("mute", &pb); 
    button_param_set("inc", &pb);
    button_param_set("dec", &pb); //设置按键参数
    button_combination_init(bc_table); // 初始化组合按键表

    return 0;
}

```