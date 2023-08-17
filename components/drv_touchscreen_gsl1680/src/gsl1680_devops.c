
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <aos/kernel.h>
#include <devices/device.h>
#include <devices/input.h>
#include <drv/iic.h>
#include <devices/impl/input_impl.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include "gsl1680_devops.h"
#include "gsl1680_fw.h"
#include <debug/dbg.h>
#include "drv/tick.h"

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))
#define GSL1680_TASK_PRI 38
#define GSL1680_IIC_ADDR 0x40

#define TP_HEIGHT       480
#define EVENT_MAX_NUM 200

typedef struct {
    slist_t             next;
    rvm_hal_input_event event_cb;
    void *              priv;
} touch_event_cb_t;

typedef struct {
    int front;
    int rear;
    rvm_hal_input_event_t event[EVENT_MAX_NUM];
} touch_event_fifo;
static gsl1680_gpio_pin _gsl1680_gpio = {0};
static aos_event_t _gslEvent;
static int _runStatus = 0;
static pthread_t _pthreadId = 0;
static csi_gpio_t _reset_gpio = {0};
static csi_gpio_t _interrupt_gpio = {0};
static touch_event_fifo _event_fifo = {0};
static pthread_mutex_t _event_mutex;
static csi_iic_t _iic_info = {0};
static slist_t g_event_cb_head;
void * _gsl1680_task(void * args);

static int event_push_back(touch_event_fifo * head, rvm_hal_input_event_t * event)
{
    if (head->front  == (head->rear + 1) % EVENT_MAX_NUM ) {
        return 1;
    }
    pthread_mutex_lock(&_event_mutex);
    memcpy(&head->event[head->rear], event, sizeof(rvm_hal_input_event_t));
    head->rear = (head->rear + 1) % EVENT_MAX_NUM;
    pthread_mutex_unlock(&_event_mutex);
    return 0;
}

static int event_pop_front(touch_event_fifo * head, rvm_hal_input_event_t * event)
{
    if (head->front  == head->rear) {
        return 1;
    }
    pthread_mutex_lock(&_event_mutex);
    memcpy(event, &head->event[head->front], sizeof(rvm_hal_input_event_t));
    head->front = (head->front + 1) % EVENT_MAX_NUM;
    pthread_mutex_unlock(&_event_mutex);
    return 0;
}

static int _i2c_init(unsigned char i2c_id)
{
    csi_iic_t *iic; 
    iic = &_iic_info;

    if (csi_iic_init(iic, i2c_id)) {
        printf("iic-%d csi_iic_init error", i2c_id);
        return CSI_ERROR;
    }

    if (csi_iic_mode(iic, IIC_MODE_MASTER)) {
        printf("iic-%d csi_iic_mode error", i2c_id);
        goto INIT_FAIL;
    }

    if (csi_iic_addr_mode(iic, IIC_ADDRESS_7BIT)) {
        printf("iic-%d csi_iic_addr_mode error", i2c_id);
        goto INIT_FAIL;
    }

    if (csi_iic_speed(iic, IIC_BUS_SPEED_FAST)) {
        printf("iic-%d csi_iic_speed error", i2c_id);
        goto INIT_FAIL;
    }
    return 0;

INIT_FAIL:
    csi_iic_uninit(iic);
    return -1;
}

static int _i2c_uninit()
{
    csi_iic_t *iic;
    iic = &_iic_info;
    csi_iic_uninit(iic);
    return 0;
}

static int _i2c_read(unsigned char i2c_addr, unsigned int addr,
             unsigned char * data ,unsigned int data_byte)
{
    int ret = 0;
    unsigned int timeout = 2000;
    csi_iic_t *iic;
    csi_iic_mem_addr_size_t reg_addr_len;
    reg_addr_len = IIC_MEM_ADDR_SIZE_8BIT;

    iic = &_iic_info;
    ret = csi_iic_mem_receive(iic, i2c_addr, addr,
                    reg_addr_len, data, data_byte, timeout);
    if (ret != data_byte) {
        printf("_i2c_read error!\n");
        return -1;
    }
    return ret;
}

static int _i2c_write(unsigned char i2c_addr, unsigned int addr,
             unsigned char * data, unsigned int data_byte)
{
    int ret = 0;
    unsigned int timeout = 4000;
    csi_iic_t *iic;
    csi_iic_mem_addr_size_t reg_addr_len;

    iic = &_iic_info;
    reg_addr_len = IIC_MEM_ADDR_SIZE_8BIT;

    ret = csi_iic_mem_send(iic, i2c_addr, addr, reg_addr_len, data, data_byte, timeout);
    if (ret != data_byte) {
        printf("_i2c_write error!\n");
        return -1;
    }
    // printf("i2c w 0x%x = 0x%2x\n", addr, data);
    return 0;
}

static void _gpio_interrupt_cb()
{
    csi_gpio_irq_enable(&_interrupt_gpio, 1 << _gsl1680_gpio.interrupt_gpio_chn, false);

    aos_event_set(&_gslEvent, 0x01, AOS_EVENT_OR);
}

static int _gpio_init()
{
    csi_gpio_init(&_reset_gpio, _gsl1680_gpio.reset_gpio_group);
    csi_gpio_mode(&_reset_gpio , 1 << _gsl1680_gpio.reset_gpio_group, GPIO_MODE_PULLNONE);
    csi_gpio_dir(&_reset_gpio, 1 << _gsl1680_gpio.reset_gpio_chn ,GPIO_DIRECTION_OUTPUT);

    csi_gpio_init(&_interrupt_gpio, _gsl1680_gpio.interrupt_gpio_group);
    csi_gpio_mode(&_interrupt_gpio , 1 << _gsl1680_gpio.interrupt_gpio_chn, GPIO_MODE_PULLNONE);
    csi_gpio_dir(&_interrupt_gpio , 1 << _gsl1680_gpio.interrupt_gpio_chn, GPIO_DIRECTION_INPUT);
    csi_gpio_irq_mode(&_interrupt_gpio, 1 << _gsl1680_gpio.interrupt_gpio_chn, GPIO_IRQ_MODE_FALLING_EDGE);
    csi_gpio_attach_callback(&_interrupt_gpio, _gpio_interrupt_cb, NULL);
    csi_gpio_irq_enable(&_interrupt_gpio, 1 << _gsl1680_gpio.interrupt_gpio_chn, true);
    return 0;
}

static int _gpio_uninit()
{
    csi_gpio_uninit(&_reset_gpio);
    csi_gpio_uninit(&_interrupt_gpio);
    return 0;
}

static void _gs1680_reset_gpio_set(int value)
{
    csi_gpio_write(&_reset_gpio, 1 << _gsl1680_gpio.reset_gpio_chn, value);
}

static int _event_read(rvm_dev_t *dev, void *data, uint32_t size, unsigned int timeout_ms)
{
    int _ret = size;
    rvm_hal_input_event_t  _event = {0};

    if (dev == NULL) {
        return -1;
    }
    if(event_pop_front(&_event_fifo, &_event) == 0) {
        memcpy(data, &_event, sizeof(_event));
    } else {
        _ret = 0;
    }
    return _ret;
}

static void _gs1680_clear_reg(void)
{
    unsigned char _writebuf = 0x88;
    _i2c_write(GSL1680_IIC_ADDR, 0xe0, &_writebuf, 1);
    usleep(1*1000);
    _writebuf = 0x03;
    _i2c_write(GSL1680_IIC_ADDR, 0x80, &_writebuf, 1);
    usleep(1*1000);
    _writebuf = 0x04;
    _i2c_write(GSL1680_IIC_ADDR, 0xe4, &_writebuf, 1);
    usleep(1*1000);
    _writebuf = 0x00;
    _i2c_write(GSL1680_IIC_ADDR, 0xe0, &_writebuf, 1);
    usleep(1*1000);
}

static void _gs1680_reset_chip(void)
{
    unsigned char data[4] = {0};
    data[0] = 0x88;
    _i2c_write(GSL1680_IIC_ADDR, 0xe0, data, 1);
    usleep(1*1000);
    data[0] = 0x04;
    _i2c_write(GSL1680_IIC_ADDR, 0xe4, data, 1);
    usleep(1*1000);
    data[0] = 0;
    _i2c_write(GSL1680_IIC_ADDR, 0xbc, data, 4);
    usleep(1*1000);
}

static void _gs1680_load_fw()
{
    unsigned char addr;
    unsigned char buf[4] = {0};
    unsigned int source_line = 0;
    unsigned int source_len;
    source_len = ARRAY_SIZE(GSLX680_FW);
    for (source_line = 0; source_line < source_len; source_line++) 
    {
        addr = 0xf0;
        buf[0] = GSLX680_FW[source_line][0];
        _i2c_write(GSL1680_IIC_ADDR, addr, buf, 1);
        _i2c_write(GSL1680_IIC_ADDR, 0x00, GSLX680_DATA[source_line], 128);
    }
}

static void _gs1680_startup_chip(void)
{
    unsigned char data = 0x00;
    _i2c_write(GSL1680_IIC_ADDR, 0xe0, &data, 1);
    usleep(1*1000);
    data = 0x04;
    _i2c_write(GSL1680_IIC_ADDR, 0xe4, &data, 1);
    usleep(1*1000);
}
#if 0
static void _gs1680_check_chip(void)
{

}
#endif
static int _gsl1680_drv_taskinit(rvm_dev_t * dev)
{
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 8192);
    pthread_create(&_pthreadId, &attr, _gsl1680_task, dev);
    pthread_setname_np(_pthreadId, "gsl1680_task");

    return 0;
}

rvm_dev_t *gsl1680_drv_init(driver_t *drv, void *config, int id)
{
    rvm_dev_t *dev = rvm_hal_device_new(drv, sizeof(input_driver_t), id);
    //初始化IIC
    if(_i2c_init(_gsl1680_gpio.iic_id) != 0) {
        if(dev) {
            rvm_hal_device_free(dev);
        }
        return NULL;
    }
    //操作reset引脚
    aos_event_new(&_gslEvent, 0);
    pthread_mutex_init(&_event_mutex, NULL);
    _gpio_init();
    if(_gsl1680_drv_taskinit(dev) != 0) {
        _gpio_uninit();
        _i2c_uninit();
        pthread_mutex_destroy(&_event_mutex);
        aos_event_free(&_gslEvent);
        return NULL;
    }
    return dev;
}

static void gsl1680_drv_uninit(rvm_dev_t *dev)
{
    aos_check_param(dev);
    _runStatus = 0;
    pthread_join(&_pthreadId, NULL);
    _gpio_uninit();
    _i2c_uninit(_gsl1680_gpio.iic_id);
    aos_event_free(&_gslEvent);
    pthread_mutex_destroy(&_event_mutex);
    rvm_hal_device_free(dev);
}

static int gsl1680_drv_open(rvm_dev_t *dev)
{
    slist_init(&g_event_cb_head);

    return 0;
}

static int gsl1680_drv_close(rvm_dev_t *dev)
{
    return 0;
}

static int gsl1680_drv_lpm(rvm_dev_t *dev, int state)
{
    return 0;
}



static int gsl1680_set_event(rvm_dev_t *dev, rvm_hal_input_event event_cb, void *priv)
{
    touch_event_cb_t *node;

    node = malloc(sizeof(touch_event_cb_t));

    if (node == NULL) {
        return -1;
    }

    node->event_cb = event_cb;
    node->priv     = priv;

    slist_add(&node->next, &g_event_cb_head);

    return 0;
}

static int gsl1680_unset_event(rvm_dev_t *dev, rvm_hal_input_event event_cb)
{
    slist_t *   tmp;
    touch_event_cb_t *node;

    slist_for_each_entry_safe(&g_event_cb_head, tmp, node, touch_event_cb_t, next) {
        if (node->event_cb == event_cb) {
            slist_del(&node->next, &g_event_cb_head);
            return 0;
        }
    }

    return -1;
}

static input_driver_t gsl1680_drv = {
    .drv = {
        .name = "input",
        .open = gsl1680_drv_open,
        .close = gsl1680_drv_close,
        .init = gsl1680_drv_init,
        .uninit = gsl1680_drv_uninit,
        .lpm = gsl1680_drv_lpm,
    },
    .set_event = gsl1680_set_event,
    .unset_event = gsl1680_unset_event,
    .read = _event_read,
};

void drv_gsl1680_devops_register(gsl1680_gpio_pin * config)
{
    if (config) {
        _gsl1680_gpio.iic_id = config->iic_id;
        _gsl1680_gpio.reset_gpio_polarity = config->reset_gpio_polarity;
        _gsl1680_gpio.reset_gpio_group = config->reset_gpio_group;
        _gsl1680_gpio.reset_gpio_chn = config->reset_gpio_chn;
        _gsl1680_gpio.interrupt_gpio_group = config->interrupt_gpio_group;
        _gsl1680_gpio.interrupt_gpio_chn = config->interrupt_gpio_chn;
        _gsl1680_gpio.rotation_angle = config->rotation_angle;
        _gsl1680_gpio.mirror_type = config->mirror_type;
    }
    rvm_driver_register(&gsl1680_drv.drv, NULL, 0);
}

void gs1680_report_event(void *args, int id)
{
    slist_t *             tmp;
    touch_event_cb_t *    node;

    slist_for_each_entry_safe(&g_event_cb_head, tmp, node, touch_event_cb_t, next) {
        if (node->event_cb) {
            node->event_cb((rvm_dev_t *)args, id, node->priv);
        }
    }
}

static void pt_move(int *x, int *y, int dx, int dy)
{
    *x += dx;
    *y += dy;
}

static void pt_rotation(int *x, int *y, int angle)
{
    int tmp;
    switch(angle) {
        case 90:
            tmp = *x;
            *x = *y;
            *y = -tmp;
            break;
        case 180:
            *x = -*x;
            *y = -*y;
            break;
        case 270:
            tmp = *x;
            *x = -*y;
            *y = tmp;
            break;
        default:;
    }
}

static void touch_rotation(int *x, int *y, int w, int h, int angle)
{
    if (angle != 90 && angle != 180 && angle != 270) {
        return;
    }
    pt_move(x, y, -w / 2, -h / 2);
    pt_rotation(x, y, angle);
    pt_move(x, y, w / 2, h / 2);
}

static void touch_mirror(int *x, int *y, int w, int h, int type)
{
    switch(type) {
        case 1:
            *x = w - *x;
            break;
        case 2:
            *y = h - *y;
            break;
        default:;
    }
}

#define GSL1680_ROTATION(x, y) touch_rotation(x, y, TP_HEIGHT, TP_HEIGHT, _gsl1680_gpio.rotation_angle)
#define GSL1680_MIRROR(x, y) touch_mirror(x, y, TP_HEIGHT, TP_HEIGHT, _gsl1680_gpio.mirror_type)

static void _gsl1680_event_process(unsigned char * touch_data, void * args)
{
    static unsigned int finger_num = 0;
    static int touch1_down_flag = 0;
    static int touch2_down_flag = 0;
    static int	cur_touch_point = 0;  //当前触摸的点数
    static int	old_touch_point = 0; //上次TP触摸的点数
    static int movex1, movey1, movex2, movey2;
    static __tp_point_t    tp_point   = {0};
    static int SetDown = 0;
    //#define INPUT_EVENT_READ     0
    //#define INPUT_EVENT_OVERFLOW 1
    rvm_hal_input_event_t  _event = {0};
    uint64_t       timestamp_ms = aos_now_ms();

    tp_point.x[0] = (((touch_data[7]&0x0f)<<8) | touch_data[6]);
    tp_point.y[0] = ((touch_data[5]<<8) | touch_data[4]);
    tp_point.id[0] = ((touch_data[7] & 0xf0) >> 4);
    tp_point.x[1] = ((touch_data[11]&0x0f)<<8 )|touch_data[10];
    tp_point.y[1] = (touch_data[9]<<8)|touch_data[8];
    tp_point.id[1] = ((touch_data[11]&0xf0)>>4);
    finger_num = touch_data[0];
    old_touch_point = cur_touch_point;
    cur_touch_point = touch_data[0];
    if(!((cur_touch_point != old_touch_point)&&(touch1_down_flag == 1 || touch2_down_flag == 1))) {
        if(finger_num > 0) {
                touch1_down_flag = 1;
                if (SetDown == 0) {
                    _event.type = EV_KEY;
                    _event.code = BTN_TOUCH;
                    _event.value = 1;
                    _event.timestamp_ms = timestamp_ms;
                    if(event_push_back(&_event_fifo, &_event) != 0) {
                        gs1680_report_event(args,1);
                    }
                }

                int rx = tp_point.x[0];
                int ry = tp_point.y[0];
                GSL1680_ROTATION(&rx, &ry);
                //printf("--> tp (%d,%d) rotation %d -> (%d,%d)\r\n",tp_point.x[0], tp_point.y[0], _gsl1680_gpio.rotation_angle, rx, ry);
                GSL1680_MIRROR(&rx, &ry);
                //printf("--> tp (%d,%d) mirror %d -> (%d,%d)\r\n",tp_point.x[0], tp_point.y[0], _gsl1680_gpio.rotation_angle, rx, ry);

                _event.type = EV_ABS;
                _event.code = ABS_X;
                _event.value = rx;
                _event.timestamp_ms = timestamp_ms;
                if(event_push_back(&_event_fifo, &_event) != 0) {
                    gs1680_report_event(args,1);
                }
                _event.type = EV_ABS;
                _event.code = ABS_Y;
                _event.value = ry;
                _event.timestamp_ms = timestamp_ms;

                if(event_push_back(&_event_fifo, &_event) != 0) {
                    gs1680_report_event(args,1);
                }
                if (SetDown == 0) {
                    SetDown = 1;
                    movex1 =  tp_point.y[0]; 
                    movey1 =  TP_HEIGHT - tp_point.x[0];
                    _event.type = EV_SYN;
                    _event.code = SYN_REPORT;
                    _event.value = 0;
                    _event.timestamp_ms = timestamp_ms;
                    if(event_push_back(&_event_fifo, &_event) != 0) {
                        gs1680_report_event(args,1);
                    }
                } else {
                    movex2 =  tp_point.y[0]; 
                    movey2 =  TP_HEIGHT - tp_point.x[0];
                    if((movex1 > movex2 && (movex1 - movex2 > 15))
                        || (movex2 > movex1 && (movex2 -movex1 > 15))
                        || (movey1 >movey2 && (movey1 -movey2 > 15))
                        || (movey2 > movey1 && (movey2 -movey1 > 15))) {
                            _event.type = EV_SYN;
                            _event.code = SYN_REPORT;
                            _event.value = 0;
                            _event.timestamp_ms = timestamp_ms;
                            if(event_push_back(&_event_fifo, &_event) != 0) {
                                gs1680_report_event(args,1);
                            }
                    } else {
                        _event.type = EV_KEY;
                        _event.code = BTN_TOUCH;
                        _event.value = 1;
                        _event.timestamp_ms = timestamp_ms;
                        if(event_push_back(&_event_fifo, &_event) != 0) {
                            gs1680_report_event(args,1);
                        }
                    }
                }
        }
        gs1680_report_event(args,0);
    } else {
        if(touch1_down_flag && cur_touch_point == 0) {
            touch1_down_flag = 0;
            //printf("-- event = %d -- \n", (touch_data[3]>>6)&0x3);
            
            int rx = tp_point.x[0];
            int ry = tp_point.y[0];
            GSL1680_ROTATION(&rx, &ry);
            GSL1680_MIRROR(&rx, &ry);

            _event.type = EV_ABS;
            _event.code = ABS_X;
            _event.value = rx;
            _event.timestamp_ms = timestamp_ms;
            if(event_push_back(&_event_fifo, &_event) != 0) {
                gs1680_report_event(args,1);
            }
            _event.type = EV_ABS;
            _event.code = ABS_Y;
            _event.value = ry;
            _event.timestamp_ms = timestamp_ms;
            if(event_push_back(&_event_fifo, &_event) != 0) {
                gs1680_report_event(args,1);
            }
            SetDown = 0;
            movex1=movey1=movex2=movey2 = 0;
            _event.type = EV_KEY;
            _event.code = BTN_TOUCH;
            _event.value = 0;
            _event.timestamp_ms = timestamp_ms;
            if(event_push_back(&_event_fifo, &_event) != 0) {
                gs1680_report_event(args,1);
            }
        }
        gs1680_report_event(args,0);
    }
}

void _gls1680_drv_fwinit()
{
    if (_gsl1680_gpio.reset_gpio_polarity == 1) {
        _gs1680_reset_gpio_set(0);
        usleep(1*1000);
        _gs1680_reset_gpio_set(1);
    } else {
        _gs1680_reset_gpio_set(1);
        usleep(1*1000);
        _gs1680_reset_gpio_set(0);
    }
    usleep(1*1000);
    //清除芯片寄存器
    _gs1680_clear_reg();
    //复位芯片
    _gs1680_reset_chip();
    //加载固件
    _gs1680_load_fw();
    //启动芯片
    _gs1680_startup_chip();
    //_gs1680_reset_chip();
    //_gs1680_startup_chip();
}

void * _gsl1680_task(void * args)
{
    int _ret = 0;
    unsigned char touch_data[24] = {0};
    unsigned int actl_flags = 0;
    _gls1680_drv_fwinit();
    _runStatus = 1;

    pthread_setschedprio(_pthreadId, 36);
    while (_runStatus) {
        if (aos_event_get(&_gslEvent , 0x01, AOS_EVENT_OR_CLEAR,
            &actl_flags, AOS_WAIT_FOREVER) == 0) {
                // if (_pinput_event) {
                    _ret = _i2c_read(GSL1680_IIC_ADDR, 0x80, touch_data, sizeof(touch_data));
                    if(_ret > 0) {
                        _gsl1680_event_process(touch_data, args);
                    }
                    csi_gpio_irq_enable(&_interrupt_gpio, 1 << _gsl1680_gpio.interrupt_gpio_chn, true);
                // }
            }
    }
    _runStatus = 0;
    return 0;
}
