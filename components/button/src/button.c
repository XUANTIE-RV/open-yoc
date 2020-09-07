/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <yoc/button.h>

#include <pinmux.h>

#include "internal.h"

#define MIN_ST_TMOUT (60)
#define MIN_LD_TMOUT (2000)
#define MAX_DD_TMOUT (1000)


#define BUTTON_OPS(b, fn) \
        if (b->ops->fn) \
            b->ops->fn(b)

#define TAG "button"

typedef enum {
    NONE,
    START,
    CNT,
    HIGH,
    END,
} button_state;

typedef struct button_srv {
    aos_timer_t tmr;
    int         adc_flag;
    int         inited;
    slist_t     head;
} button_srv_t;

#define b_param(b) (b->param)

button_srv_t g_button_srv;
#define BUTTON_EVT(button) \
    do {\
        if(button->evt_flag & (1 << button->evt_id)) {\
            button->cb(button->evt_id, button->name, button->priv);\
        }\
    } while(0)

static button_t *button_find(int pin_id);
static button_t *button_find_by_name(const char *name);

static void irq_disable(button_t *button)
{
    BUTTON_OPS(button, irq_disable);
}

static void irq_enable(button_t *button)
{
    BUTTON_OPS(button, irq_enable);
}

void button_irq(button_t *button)
{
    irq_disable(button);
    button->irq_flag = 1;
    button->st_ms = aos_now_ms();

    if (aos_timer_is_valid(&g_button_srv.tmr)) {
        aos_timer_start(&g_button_srv.tmr);
    } else {
        irq_enable(button);
    }
}

static int reset(button_t *button)
{
    button->state = NONE;
    button->active = 0;

    return 0;
}

static int none_hdl(button_t *button, int level)
{
    if (level == b_param(button).active_level) {
        button->state = START;
        aos_timer_start(&g_button_srv.tmr);
    } else {
        irq_enable(button);
    }

    return 0;
}

static int start_hdl(button_t *button, int level)
{
    if (level == b_param(button).active_level) {
        if ((aos_now_ms() - button->st_ms) >= b_param(button).st_tmout) {
            // button->active = 1;
            button->state = CNT;
        }

        aos_timer_start(&g_button_srv.tmr);
    } else {
        reset(button);
        irq_enable(button);
    }

    return 0;
}

static int cnt_hdl(button_t *button, int level)
{
    if (level == b_param(button).active_level) {
        if ((aos_now_ms() - button->st_ms) >= b_param(button).ld_tmout) {
            button->state = HIGH;
            button->active = 1;
            button->evt_id = BUTTON_PRESS_LONG_DOWN;
        }

        aos_timer_start(&g_button_srv.tmr);
    } else {
        button->state = HIGH;
        button->active = 1;
        button->evt_id = BUTTON_PRESS_DOWN;
        aos_timer_start(&g_button_srv.tmr);
        button->old_evt_id = BUTTON_PRESS_DOWN;
    }

    return 0;
}

static int high_hdl(button_t *button, int level)
{
    if (level == HIGH_LEVEL) {
        button->state = END;
        button->active = 1;

        if (button->evt_id == BUTTON_PRESS_LONG_DOWN) {
            button->evt_id = BUTTON_EVT_END;
        } else {
            button->evt_id = BUTTON_PRESS_UP;
        }

        if (button->old_evt_id == BUTTON_PRESS_DOWN) {
            button->t_ms = aos_now_ms();
        }

        irq_enable(button);
    } else {
        aos_timer_start(&g_button_srv.tmr);
    }

    return 0;
}

static int update_repeat(button_t *button)
{
    if (button->state == NONE && button->old_evt_id == BUTTON_PRESS_DOWN \
        && (button->evt_flag & DOUBLE_PRESS_FLAG)) {
        long long past = aos_now_ms() - button->t_ms;

        if (past < b_param(button).dd_tmout) {
            button->repeat = 1;
        } else {
            button->repeat = 0;
        }
    }

    return 0;
}

static int event_call(button_t *button)
{
    if (button->active >= 1) {
        if (button->repeat == 0) {
            BUTTON_EVT(button);
            button->active = 0;

            if (button->state == END) {
                reset(button);
            }

        } else {
            if (button->evt_id == BUTTON_PRESS_DOWN || button->evt_id == BUTTON_PRESS_LONG_DOWN) {
                button->evt_id = BUTTON_PRESS_DOUBLE;
                button->old_evt_id = BUTTON_PRESS_DOUBLE;
            }

            button->repeat = 0;
            BUTTON_EVT(button);

            if (button->state == END) {
                reset(button);
            }
        }
    }

    return 0;
}

static int read_level(button_t *button)
{
    if (button->ops->read) {
        return (button->ops->read(button));
    }
    return -1;
}

#define BC_SRV_QUEUE_MAX (4)
typedef struct _bc {
    int count;
    button_combinations_t b;
} bc_t;

typedef struct {
    const char *b_name;
    int evt_id;
} bc_msg_t;
typedef struct _bc_srv {
    aos_queue_t queue;
    int bc_num;
    int bc_max_bit;
    bc_msg_t queue_buf[BC_SRV_QUEUE_MAX];
    bc_t *bc;
} bc_srv_t;

static bc_srv_t g_srv_bc;

static int bc_do_time(void)
{
    int tmout = AOS_WAIT_FOREVER;

    for (int i = 0; i < g_srv_bc.bc_num; i++) {
        bc_t *bc = g_srv_bc.bc + i;

        if (bc->count != 0) {
            if (tmout != AOS_WAIT_FOREVER) {
                tmout = tmout < bc->b.tmout ? tmout : bc->b.tmout;
            } else {
                tmout = bc->b.tmout;
            }
        }
    }

    return tmout;
}

static int bc_trigger(const char *name, int evt_id)
{
    bc_msg_t msg;

    msg.b_name = name;
    msg.evt_id = evt_id;

    int ret = aos_queue_send(&g_srv_bc.queue, &msg, sizeof(bc_msg_t));

    return ret;
}

static int bc_wait(int tmout, bc_msg_t *msg)
{
    unsigned int count;
    return aos_queue_recv(&g_srv_bc.queue, tmout, msg, &count);
}

static void bc_button_disable(bc_t *bc)
{
    for (int j = 0; j < MAX_COMBINATION_NUM; j ++) {
        button_t *b = button_find_by_name(bc->b.pin_name[j]);
        reset(b);
        irq_enable(b);
    }
}

static bc_t *bc_do_event(bc_msg_t *msg)
{
    const char *name = msg->b_name;
    int evt_id = msg->evt_id;

    for (int i = 0; i < g_srv_bc.bc_num; i++) {
        bc_t *bc = g_srv_bc.bc + i;

        for (int j = 0; j < MAX_COMBINATION_NUM; j ++) {
            if (strcmp(bc->b.pin_name[j], name) == 0) {
                bc->count |= 1 << j;

                if ((bc->count == g_srv_bc.bc_max_bit) && ((1 << evt_id) & bc->b.evt_flag)) {
                    if (bc->b.cb) {
                        bc->b.cb(BUTTON_COMBINATION, bc->b.name, bc->b.priv);
                    }
                    bc->count = 0;
                    return bc;
                }
            }
        }
    }

    return NULL;
}
static void bc_event_hdl(bc_msg_t *msg)
{
    bc_t *bc = bc_do_event(msg);

    if (bc)
        bc_button_disable(bc);
}

static bc_t *bc_find(const char *name)
{
    bc_t *bc;

    for (int i = 0; i < g_srv_bc.bc_num; i++) {
        bc = g_srv_bc.bc + i;

        for (int j = 0; j < MAX_COMBINATION_NUM; j ++) {
            if (strcmp(bc->b.pin_name[j], name) == 0) {
                return bc;
            }
        }
    }

    return NULL;
}

static void bc_button_hdl(void)
{
    for (int i = 0; i < g_srv_bc.bc_num; i++) {
        bc_t *bc = g_srv_bc.bc + i;

        if (bc->count != 0) {
            for (int j = 0; j < MAX_COMBINATION_NUM; j ++) {
                if (bc->count & (1 << j)) {
                    button_t *button = button_find_by_name(bc->b.pin_name[j]);
                    event_call(button);
                }
            }

            bc->count = 0;
        }
    }
}

static void _bc_hdl(void *priv)
{
    int tmout;
    int ret   = -1;
    bc_msg_t msg;

    while (1) {
        tmout = bc_do_time();
        ret = bc_wait(tmout, &msg);

        if (ret == 0) {
            bc_event_hdl(&msg);
        } else {
            bc_button_hdl();
        }
    }
}

static int bc_name_is_duplicate(const button_combinations_t bc_tbl[])
{
    char *name;
    int ret = 0;
    int i = 0;

    while (!ret) {

        if (bc_tbl[i].cb == NULL && bc_tbl[i].pin_sum == 0) {
            ret = 0;
            break;
        }

        name = (char *)bc_tbl[i].name;
        for (int j = i+1; bc_tbl[j].cb != NULL && bc_tbl[j].pin_sum != 0; j++) {
            if (strcmp(name, bc_tbl[j].name) == 0) {
                ret = 1;
                break;
            }
        }

        i ++;
    }

    return ret;
}

static int bc_tbl_check(const button_combinations_t *bc_tbl)
{
    if (bc_tbl->cb == NULL) {
        return -1;
    }

    if (((bc_tbl->evt_flag & EVT_ALL_FLAG) == 0) || ((bc_tbl->evt_flag & ~EVT_ALL_FLAG) != 0)) {
        return -1;
    }

    if (bc_tbl->name[0] == 0) {
        return -1;
    }

    if (bc_tbl->pin_sum > MAX_COMBINATION_NUM || bc_tbl->pin_sum < 2) {
        return -1;
    }

    for (int j = 0; j < bc_tbl->pin_sum; j++) {
        if (bc_tbl->pin_name[j] == NULL) {
            return -1;
        }
    }

    return 0;
}

int button_combination_init(const button_combinations_t bc_tbl[])
{
    int i = 0;
    int j = 0;
    int ret = -1;

    if (g_srv_bc.bc_num || (bc_tbl[0].cb == NULL && bc_tbl[0].pin_sum == 0)) {
        goto bc_err0;
    }

    if (bc_name_is_duplicate(bc_tbl)) {
        goto bc_err0;
    }

    for (j = 0; j < MAX_COMBINATION_NUM; j++) {
        g_srv_bc.bc_max_bit |= 1 << j;
    }

    button_t *b[MAX_COMBINATION_NUM];
    button_t *button;

    while (1) {
        if (bc_tbl[i].cb == NULL && bc_tbl[i].pin_sum == 0) {
            ret = 0;
            break;
        }

        if (bc_tbl_check(&bc_tbl[i])) {
            ret = -1;
            goto bc_err0;
        }

        button = button_find_by_name(bc_tbl[i].name);
        if (button != NULL) {
            ret = -1;
            goto bc_err0;
        }

        for (j = 0; j < bc_tbl[i].pin_sum; j++) {
            b[j] = button_find_by_name(bc_tbl[i].pin_name[j]);

            if (b[j] == NULL || ((b[j]->evt_flag & bc_tbl[i].evt_flag) == 0)) {
                ret = -1;
                goto bc_err0;
            }
        }

        for (j = 0; j < bc_tbl[i].pin_sum; j++) {
            b[j]->bc_flag = 1;
        }

        i ++;
    }

    if (ret == 0) {
        g_srv_bc.bc_num = i;
        g_srv_bc.bc     = aos_zalloc_check(sizeof(bc_t) * i);
    } else {
        goto bc_err0;
    }

    for (i = 0; i < g_srv_bc.bc_num; i++) {
        bc_t *bc = g_srv_bc.bc + i;
        memcpy(&bc->b, bc_tbl + i, sizeof(button_combinations_t));
    }

    ret = aos_queue_new(&g_srv_bc.queue, g_srv_bc.queue_buf, BC_SRV_QUEUE_MAX * sizeof(bc_msg_t), sizeof(bc_msg_t));

    if (ret < 0) {
        ret = -1;
        goto bc_err1;
    }

    aos_task_t task;
    ret = aos_task_new_ext(&task, "button_bc", _bc_hdl, NULL, 2 * 1024, AOS_DEFAULT_APP_PRI - 8);

    if (ret < 0) {
        ret = -1;
        goto bc_err2;
    }

    return 0;
bc_err2:
    aos_queue_free(&g_srv_bc.queue);
bc_err1:
    aos_free(g_srv_bc.bc);
bc_err0:

    return ret;
}
/*
    ----|
*/
static int _button_hdl(button_t *button)
{
    int level;

    update_repeat(button);

    level = read_level(button);

    // LOGD(TAG, "level:%d state:%d", level, button->state);

    aos_timer_stop(&g_button_srv.tmr);

    switch (button->state) {
        case NONE:
            none_hdl(button, level);
            break;

        case START:
            start_hdl(button, level);
            break;

        case CNT:
            cnt_hdl(button, level);
            break;

        case HIGH:
            high_hdl(button, level);
            break;

        default:
            reset(button);
            break;
    }

    // LOGD(TAG, "state:%d", button->state);

    if (button->bc_flag && button->active == 1) {
        bc_t *bc = bc_find(button->name);

        if (bc && (bc->b.evt_flag & (1 << button->evt_id))) {
            int ret = bc_trigger(button->name, button->evt_id);

            if (ret >= 0) {
                button->active ++;
            }

            return 0;
        }
    }

    if (button->active == 1) {
        event_call(button);
    }

    return 0;
}

void button_adc_sttime_check(void)
{
    button_t *b;

    slist_for_each_entry(&g_button_srv.head, b, button_t, next) {
        if (b->param.adc_name != NULL && b->irq_flag == 0 && b->st_ms == 0) {
            int level = b->ops->read(b);

            if (level == b_param(b).active_level) {
                b->st_ms = aos_now_ms();
                b->irq_flag = 1;
            }
        }
    }
}

static void button_timer_entry(void *timer, void *arg)
{
    button_t *b;
    int cnt = 0;

    button_adc_sttime_check();

    slist_for_each_entry(&g_button_srv.head, b, button_t, next) {
        if (b->irq_flag == 1 || b->state > 0) {
            _button_hdl(b);
            b->irq_flag = 0;
            cnt ++;
        }
    }

    if (cnt == 0) {
        aos_timer_stop(&g_button_srv.tmr);
    }

    if (g_button_srv.adc_flag) {
        aos_timer_start(&g_button_srv.tmr);
    }
}

static button_t *button_find_by_name(const char *name)
{
    button_t *b;

    slist_for_each_entry(&g_button_srv.head, b, button_t, next) {
        if (strcmp(name, b->name) == 0) {
            return b;
        }
    }

    return NULL;
}

static button_t *button_find(int pin_id)
{
    button_t *b;

    slist_for_each_entry(&g_button_srv.head, b, button_t, next) {
        if (b->pin_id == pin_id) {
            return b;
        }
    }

    return NULL;
}

static int button_new(button_t **button)
{
    *button = aos_zalloc(sizeof(button_t));

    return *button == NULL ? -1 : 0;
}

static int button_add(button_t *button)
{
    slist_add_tail(&button->next, &g_button_srv.head);

    return 0;
}

static int button_set_ops(button_t *button, button_ops_t *ops)
{
    button->ops = ops;

    return 0;
}

static int button_param_init(button_t *button)
{
    button_param_t *p = &button->param;

    p->active_level = LOW_LEVEL;
    p->st_tmout = MIN_ST_TMOUT;
    p->ld_tmout = MIN_LD_TMOUT;
    p->dd_tmout = MAX_DD_TMOUT;

    return 0;
}

// csi pin start
static void pin_event(int32_t idx)
{
    button_t *button = NULL;

    button = button_find(idx);

    if (button != NULL) {
        button_irq(button);
    }
}

static gpio_pin_handle_t *csi_gpio_init(int pin_id, button_t *button)
{
    gpio_pin_handle_t *pin_hdl;

    drv_pinmux_config(pin_id, PIN_FUNC_GPIO);

    pin_hdl = csi_gpio_pin_initialize(pin_id, pin_event);
    // csi_gpio_pin_set_evt_priv(pin_hdl, button);
    csi_gpio_pin_config_direction(pin_hdl, GPIO_DIRECTION_INPUT);
    csi_gpio_pin_set_irq(pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 0);

    return pin_hdl;
}

static int csi_irq_disable(button_t *button)
{
    csi_gpio_pin_set_irq(button->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 0);

    return 0;
}

static int csi_irq_enable(button_t *button)
{
    csi_gpio_pin_set_irq(button->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 1);

    return 0;
}

static int csi_pin_read(button_t *button)
{
    bool val;

    csi_gpio_pin_read(button->pin_hdl, &val);

    return (val == false) ? LOW_LEVEL : HIGH_LEVEL;
}

static int csi_pin_init(button_t *button)
{
    button->pin_hdl = csi_gpio_init(button->pin_id, button);
    csi_gpio_pin_set_irq(button->pin_hdl, GPIO_IRQ_MODE_FALLING_EDGE, 1);

    return 0;
}

static button_ops_t gpio_ops = {
    .init = csi_pin_init,
    .read = csi_pin_read,
    .irq_disable = csi_irq_disable,
    .irq_enable = csi_irq_enable,
};
// csi pin end

#include <drv/adc.h>
#include <devices/adc.h>

int button_adc_check(button_t *b, int vol)
{
    int range = b_param(b).range;
    int vref  = b_param(b).vref;

    if (vol < (vref + range) && vol > (vref - range)) {
        return 0;
    } else {
        return 1;
    }
}

static int button_adc_read(button_t *b)
{
    int ret, vol;
    uint32_t ch = 0;
    hal_adc_config_t config;

    aos_dev_t *dev = adc_open(b_param(b).adc_name);
    ch = adc_pin2channel(dev, b->pin_id);
    adc_config_default(&config);
    config.channel = &ch;
    ret = adc_config(dev, &config);

    if(ret == 0) {
        ret = adc_read(dev, &vol, 0);
    }

    adc_close(dev);

    return button_adc_check(b, vol);
}

static int button_adc_enable(button_t *button)
{
    button->st_ms = 0;

    return 0;
}

static int button_adc_disable(button_t *button)
{
    button->st_ms = -1;

    return 0;
}

static button_ops_t adc_ops = {
    .read = button_adc_read,
    .irq_enable = button_adc_enable,
    .irq_disable = button_adc_disable,
};

static int button_param_check(button_t *button, button_param_t *p)
{
    if (p ->active_level != 0 && p ->active_level != 1) {
        return -1;
    }

    if (p->dd_tmout < 0) {
        return -1;
    }

    if (p->ld_tmout < 0) {
        return -1;
    }

    if (button->ops == &adc_ops) {
        if (p->adc_name == NULL || p->vref <= 0 || p->range < 0) {
            return -1;
        }
    }
    return 0;
}

static int button_table_check(const button_config_t *b_tbl)
{
    if (b_tbl == NULL) {
        return -1;
    }

    if (b_tbl->cb == NULL) {
        return -1;
    }

    if (((b_tbl->evt_flag & EVT_ALL_FLAG) == 0) || ((b_tbl->evt_flag & ~EVT_ALL_FLAG) != 0)) {
        return -1;
    }

    if (b_tbl->name[0] == 0) {
        return -1;
    }

    if (b_tbl->pin_id < 0) {
        return -1;
    }

    if (b_tbl->type != BUTTON_TYPE_ADC && b_tbl->type != BUTTON_TYPE_GPIO) {
        return -1;
    }

    return 0;
}

int button_param_cur(char *name, button_param_t *p)
{
    aos_check_return_einval(name && p);

    button_t *button = button_find_by_name(name);

    if (button) {
        memcpy(p, &button->param, sizeof(button_param_t));
        return 0;
    } else {
        return -1;
    }
}

int button_param_set(char *name, button_param_t *p)
{
    aos_check_return_einval(name && p);

    button_t *button = button_find_by_name(name);

    if (button) {
        if (button_param_check(button, p) == 0) {
            memcpy(&button->param, p, sizeof(button_param_t));
            return 0;            
        }
    }

    return -1;
}

int button_init(const button_config_t b_tbl[])
{
    button_t *button;
    int ret;

    if (b_tbl == NULL || (b_tbl[0].evt_flag == 0 && b_tbl[0].cb == NULL)) {
        return -1;
    }

    int i = 0;

    while (1) {
        if (b_tbl[i].evt_flag == 0 && b_tbl[i].cb == NULL) {
            ret = 0;
            break;
        }

        if (button_table_check(&b_tbl[i]) < 0) {
            ret = -1;
            break;
        }

        button = button_find_by_name(b_tbl[i].name);
        if (button != NULL) {
            ret = -1;
            break;
        }

        ret = button_new(&button);

        if (ret == 0) {
            button->pin_id = b_tbl[i].pin_id;
            button->evt_flag = b_tbl[i].evt_flag;
            button->cb = b_tbl[i].cb;
            button->priv = b_tbl[i].priv;
            button_add(button);
            button->evt_id = BUTTON_EVT_END;
            button->old_evt_id = BUTTON_EVT_END;
            strlcpy(button->name, b_tbl[i].name, MAX_BUTTON_NAME);
            if (b_tbl[i].type == BUTTON_TYPE_ADC) {
                g_button_srv.adc_flag = 1;
                button_set_ops(button, &adc_ops);
            } else {
                button_set_ops(button, &gpio_ops);
            }
            button_param_init(button);
            BUTTON_OPS(button, init);
            i ++;
        } else {
            ret = -1;
            break;
        }
    }

    if (g_button_srv.adc_flag && ret == 0) {
        aos_timer_start(&g_button_srv.tmr);
    }
    return ret;
}

int button_srv_init(void)
{
    if (g_button_srv.inited) {
        return -1;
    }
    aos_timer_new(&g_button_srv.tmr, button_timer_entry, NULL, 20, 0);
    aos_timer_stop(&g_button_srv.tmr);
    slist_init(&g_button_srv.head);
    g_button_srv.inited = 1;

    return 0;
}
