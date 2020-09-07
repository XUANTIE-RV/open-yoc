/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BUTTON__
#define __BUTTON__

#include <drv/gpio.h>

#define LOW_LEVEL (0)
#define HIGH_LEVEL (1)

typedef struct button button_t;
typedef struct button_combinations button_combinations_t;

#define MAX_BUTTON_NAME (20)
typedef enum {
    BUTTON_PRESS_DOWN = 0,
    BUTTON_PRESS_UP,
    BUTTON_PRESS_LONG_DOWN,
    BUTTON_PRESS_DOUBLE,
    BUTTON_COMBINATION,

    BUTTON_EVT_END
} button_evt_id_t;

typedef enum {
    BUTTON_TYPE_GPIO,/* GPIO按键 支持中断方式*/
    BUTTON_TYPE_ADC, /* ADC按键 */
} button_type_t;

typedef void (*evt_cb)(button_evt_id_t event_id, char *name, void *priv);

typedef struct button_param {
    int st_tmout;   //min pin active level time(soft elimination buffeting time)
    int ld_tmout;   //min long press time
    int dd_tmout;   //max double press time interval
    int active_level;
    const char *adc_name; //if button is adc_button, it has
    int range; //if button is adc_button, it has
    int vref; //if button is adc_button, it has
} button_param_t;

typedef struct button_config {
    int pin_id;
    int evt_flag;
    evt_cb cb;
    void *priv;
    int type;
    char name[MAX_BUTTON_NAME];
} button_config_t;

#define MAX_COMBINATION_NUM (2)
struct button_combinations {
    const char *pin_name[MAX_COMBINATION_NUM];
    int pin_sum;
    int evt_flag;
    int tmout;
    evt_cb cb;
    void *priv;
    char name[MAX_BUTTON_NAME];
};


#define PRESS_DOWN_FLAG (1<<BUTTON_PRESS_DOWN)
#define PRESS_UP_FLAG (1<<BUTTON_PRESS_UP)
#define PRESS_LONG_DOWN_FLAG (1<<BUTTON_PRESS_LONG_DOWN)
#define DOUBLE_PRESS_FLAG (1<<BUTTON_PRESS_DOUBLE)
#define EVT_ALL_FLAG (PRESS_DOWN_FLAG | PRESS_UP_FLAG | DOUBLE_PRESS_FLAG | PRESS_LONG_DOWN_FLAG)

/**
 * @brief  button service init
 * @return 0 on success, -1 on failed
 */
int button_srv_init(void);

/**
 * @brief  button table init
 * @param  [in] b_tbl          : button table
 * @return 0 on success, -1 on failed
 */
int button_init(const button_config_t b_tbl[]);

/**
 * @brief  combination button table init
 * @param  [in] bc_tbl          : combination button table
 * @return 0 on success, -1 on failed
 */
int button_combination_init(const button_combinations_t bc_tbl[]);

/**
 * @brief  get button param
 * @param  [in] name       : button name
 * @param  [in] p          : button param
 * @return 0 on success, -1 on failed
 */
int button_param_cur(char *name, button_param_t *p);

/**
 * @brief  set button param
 * @param  [in] name       : button name
 * @param  [in] p          : button param
 * @return 0 on success, -1 on failed
 */
int button_param_set(char *name, button_param_t *p);

#endif