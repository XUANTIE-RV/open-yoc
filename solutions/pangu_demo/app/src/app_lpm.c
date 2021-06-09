#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <aos/aos.h>
#include <aos/cli.h>
#include <drv/codec.h>
#include <aos/debug.h>
#include <aos/hal/flash.h>
#include <soc.h>
#include <yoc/yoc.h>
#include <yoc/partition.h>
#include <devices/hal/uart_impl.h>
#include <yoc/netmgr_service.h>
#include <devices/wifi.h>
#ifdef CONFIG_CSI_V2
#include <sys_clk.h>
#include <drv/gpio_pin.h>
#include <drv/pin.h>
#else
#include <sys_freq.h>
#endif
#include <ipc.h>
#include <voice.h>
#include <app_main.h>
#include <yoc/mic.h>
#include <yoc/lpm.h>
#include <ipc.h>
#include <board.h>
#include <drv/tee.h>
#ifdef CONFIG_CSI_V2
#include <drv/gpio_pin.h>
#include <drv/pin.h>
#else
#include "pinmux.h"
#include "pin_name.h"
#endif

#include "app_main.h"
#define TAG "lpm"

#ifndef CONFIG_SYSTICK_HZ
    #define CONFIG_SYSTICK_HZ 100
#endif

#define LPM_WAIT_CMD (0X01)
#define LPM_SERVER_ID (0X02)
#define LPM_TIMEOUT (10*1000)

#define reg_wr(reg_addr) *((volatile uint32_t *)reg_addr)

typedef struct {
#ifdef CONFIG_CSI_V2
    csi_codec_input_t codec;
#else
    codec_input_t    codec;
#endif
    int              recv_size;
    int              ticks;
    uint8_t         *mic;
    volatile int     vad_flag;
} app_lpm_t;

typedef struct {
    int vad_th;
    int policy;
    int standby_cb;
} app_param_t;

typedef app_lpm_t *(*lpm_enter_t)(void *p);

static long long g_lpm_tag = 0;
static app_param_t g_lpm_param;
static int g_lpm_cnt;
static ipc_t *g_lpm_ipc[2];

static int app_ipc_lpm(int cmd);
#ifdef CONFIG_CSI_V2
csi_gpio_t *pgpio_pin_handle;
#endif
void lpm_update(void)
{
    g_lpm_tag = aos_now_ms();
}

int lpm_check(void)
{
    if (pm_get_policy() == LPM_POLICY_NO_POWER_SAVE) {
        LOGD(TAG, "LPM_POLICY_NO_POWER_SAVE\n");
        return 0;
    }

    //播放状态
    if (app_player_is_busy()) {
        LOGD(TAG, "app_player_is_busy\n");
        return 0;
    } else {
        app_player_lpm(1);
    }

    //MIC状态
    if (app_mic_is_busy()) {
        LOGD(TAG, "app_mic_is_busy\n");
        app_player_lpm(0);
        return 0;
    }

    //VAD
    if (aos_now_ms() - g_lpm_tag < LPM_TIMEOUT) {
        LOGD(TAG, "lpm is not timeout(%d s)\n", LPM_TIMEOUT/1000);
        app_player_lpm(0);
        return 0;
    }

    // wifi state
    if (wifi_is_pairing() || wifi_connecting()) {
        LOGD(TAG, "wifi_is_pairing\n");
        app_player_lpm(0);
        return 0;
    }

    // fota
    if (app_fota_is_downloading()) {
        LOGD(TAG, "app_fota_is_downloading\n");
        app_player_lpm(0);
        return 0;
    }

    lpm_update();
    return 1;
}

static void app_consle_recover(void)
{
    console_init(CONSOLE_UART_IDX, 115200, 512);
}

static void app_time_recover(int ticks)
{
    aos_kernel_resume(ticks);
#ifdef CONFIG_CSI_V2
    csi_coret_config(soc_get_cur_cpu_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
#else
    csi_coret_config(drv_get_cur_cpu_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
#endif
}

static void card_lpm(int state)
{
    aos_dev_t *dev;

    if (state) {
        speaker_uninit();
    } else {
        speaker_init();
    }

    if (state == 0) {
        dev = device_find("card", 0);
        device_lpm(dev, state);
    }

    dev = device_find("pcmC", 2);
    device_lpm(dev, state);

    dev = device_find("pcmC", 0);
    device_lpm(dev, state);

    // dev = device_find("pcmP", 0);
    // if (dev)
    //     device_lpm(dev, state);

    if (state != LPM_POLICY_NO_POWER_SAVE) {
        dev = device_find("card", 0);

        if (state == LPM_POLICY_LOW_POWER) {
            device_lpm(dev, 1);
        } else {
            driver_unregister("card0");
        }
    }
}

extern netmgr_hdl_t app_netmgr_hdl;

static void wifi_lpm(int lpm_on)
{
    if (!lpm_on) {
        drv_clk_enable(SDIO_CLK);
    }

    if (app_netmgr_hdl) {
        aos_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
        hal_wifi_set_lpm(dev, lpm_on ? WIFI_LPM_KEEP_LINK : WIFI_LPM_NONE);
    }

    if (lpm_on) {
        drv_clk_disable(SDIO_CLK);
    }
}

static void flash_lpm(int state)
{
    aos_dev_t *dev;

    dev = device_find("eflash", 0);
    device_lpm(dev, state);
}

static lpm_enter_t lpm_load_fw(void)
{
#define LPM_RUN_ADDR   (0X0FFE0000)

    partition_t p = partition_open("lpm");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "lpm->start_addr=0x%x", info->start_addr);
    LOGD(TAG, "lpm->length=0x%x", info->length);
    partition_read(p, 0, (void *)LPM_RUN_ADDR, info->length);
    partition_close(p);

    return (lpm_enter_t)(LPM_RUN_ADDR);
}

static void cpu1_cpu2_lpm(int state)
{
    if (state == 0) {
        ipc_lpm(1, state);
        ipc_lpm(2, state);
    }

    if (state) {
        app_ipc_lpm(LPM_WAIT_CMD);
        while ((reg_wr(0x30000024) & 0x3c) != 0x14);
    } else {
        app_ipc_lpm(2);
        while ((reg_wr(0x30000024) & 0x3c) != 0x3c);
    }

    if (state == 1) {
        ipc_lpm(1, state);
        ipc_lpm(2, state);
    }
}

app_lpm_t g_lpm_ret;

static void lpm_check_event(uint32_t event_id, const void *data, void *context)
{
    if (event_id == EVENT_LPM_CHECK) {
        if (lpm_check()) {
            LOGE(TAG, "lpm check done");
            pm_agree_halt(0);
        }
    }

    event_publish_delay(EVENT_LPM_CHECK, NULL, 7000);
}

static void app_wifi_standby_mode(void)
{
#if 1
    wifi_lpm(1);
#else
    if (app_netmgr_hdl) {
        netmgr_stop(app_netmgr_hdl);
        netmgr_dev_wifi_deinit(app_netmgr_hdl);
        netmgr_service_deinit();
    }
#endif
}

static int get_bootloader_addr(void)
{
    partition_t p = partition_open("boot");
    partition_info_t *info = hal_flash_get_info(p);
    LOGD(TAG, "boot->start_addr=0x%x", info->start_addr + info->base_addr);

    return info->start_addr + info->base_addr;
}

static void flash_gpio_config(int state)
{
    int i = PA11;

    if (state == 0 || state == LPM_POLICY_LOW_POWER) {
        i = PA7;
    }

    for (; i < PA13; i++) {
        if (state) {
#ifdef CONFIG_CSI_V2
            csi_gpio_pin_t pin;

            memset(&pin, 0, sizeof(pin));
            csi_pin_set_mux(i, PIN_FUNC_GPIO);

            csi_gpio_pin_init(&pin, i);
            csi_gpio_pin_mode(&pin, GPIO_MODE_PULLNONE);
            csi_gpio_pin_dir(&pin, GPIO_DIRECTION_INPUT);
#else
            drv_pinmux_config(i, PIN_FUNC_GPIO);

            gpio_pin_handle_t pin = NULL;
            pin = csi_gpio_pin_initialize(i, NULL);

            csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
            csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_INPUT);
#endif
        } else {
#ifdef CONFIG_CSI_V2
            csi_pin_set_mux(i, 0);
#else
            drv_pinmux_config(i, 0);
#endif
        }
    }
}

static void sdio_gpio_config(int state)
{
    for (int i = PA13; i < PA19; i++) {
#ifdef CONFIG_CSI_V2
        if (state) {
            csi_pin_set_mux(i, PIN_FUNC_GPIO);
        } else {
            csi_pin_set_mux(i, 0);
        }
        csi_gpio_pin_t pin;
        memset(&pin, 0, sizeof(pin));
        csi_gpio_pin_init(&pin, i);
        csi_gpio_pin_mode(&pin, GPIO_MODE_PULLNONE);
        csi_gpio_pin_dir(&pin, GPIO_DIRECTION_INPUT);
#else
        if (state) {
            drv_pinmux_config(i, PIN_FUNC_GPIO);
        } else {
            drv_pinmux_config(i, 0);
        }
        gpio_pin_handle_t pin = NULL;
        pin = csi_gpio_pin_initialize(i, NULL);

        csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
        csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_INPUT);
#endif
    }
}

static void lpm_enter(int policy)
{
    int boot_addr;
    uint32_t irq_flag = 0;

    card_lpm(policy);

    if (policy == LPM_POLICY_LOW_POWER) {
        wifi_lpm(1);
    } else {
        app_wifi_standby_mode();
    }
    cpu1_cpu2_lpm(1);
    irq_flag = csi_irq_save();
    aos_kernel_sched_suspend();

    int ret;
    int vad_th;

    ret = aos_kv_getint("vad_th", &vad_th);

    if (ret >= 0) {
        g_lpm_param.vad_th = vad_th;
    } else {
        g_lpm_param.vad_th = 0x9;
    }

    g_lpm_param.policy = policy;
    LOGE(TAG, "app lpm enter(%s)(%d)(%d)", __TIME__, g_lpm_cnt++, g_lpm_param.vad_th);
    lpm_enter_t func = lpm_load_fw();
    boot_addr = get_bootloader_addr();
    flash_lpm(1);
    sdio_gpio_config(1);

    if (policy == LPM_POLICY_LOW_POWER) {
        LOGE(TAG, "LPM_POLICY_LOW_POWER");
        flash_gpio_config(policy);
        console_deinit();
        app_lpm_t *ret = (*func)(&g_lpm_param);
        memcpy(&g_lpm_ret, ret, sizeof(app_lpm_t));
    } else {
        LOGE(TAG, "LPM_POLICY_DEEP_SLEEP");
#ifdef CONFIG_TEE_CA
        csi_tee_write_reg(0x3b800004, 0x100);
#else
        // TODO:
#endif
        console_deinit();
        g_lpm_param.standby_cb = boot_addr;
        (*func)(&g_lpm_param);
    }

    flash_gpio_config(0);
    sdio_gpio_config(0);

    aos_kernel_sched_resume();
    csi_irq_restore(irq_flag);
}

static void lpm_leave(int state)
{
    uint32_t irq_flag = 0;
    app_player_lpm(0);
    irq_flag = csi_irq_save();
    aos_kernel_sched_suspend();

    app_consle_recover();
    app_time_recover(g_lpm_ret.ticks);
    flash_lpm(0);
    aos_kernel_sched_resume();
    csi_irq_restore(irq_flag);
    cpu1_cpu2_lpm(0);
    card_lpm(0);
    wifi_lpm(0);

    LOGE(TAG, "app lpm leave");
}

static void lpm_event_cb(pm_policy_t policy, lpm_event_e event)
{
    if (LPM_EVENT_SUSPEND == event) {
        lpm_enter(policy);
    } else if (event == LPM_EVENT_WAKUP) {
        lpm_leave(1);
    }
}

int sys_soc_init(pm_ctx_t *pm_ctx)
{

    return 0;
}

int sys_soc_suspend(pm_ctx_t *pm_ctx)
{

    return 0;
}

int sys_soc_resume(pm_ctx_t *pm_ctx, int pm_state)
{
    return 0;
}

static int app_ipc_lpm(int cmd)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.command = cmd;
    if (cmd == 1) {
        msg.flag   = MESSAGE_SYNC;
    } else {
        msg.flag   = MESSAGE_ASYNC;
    }

    msg.service_id = 2;

    ipc_message_send(g_lpm_ipc[0], &msg, AOS_WAIT_FOREVER);
    ipc_message_send(g_lpm_ipc[1], &msg, AOS_WAIT_FOREVER);


    LOGE(TAG, "ipc lpm cpu1&2 done\n");

    return 0;
}

static void ipc_init(void)
{
    g_lpm_ipc[0] = ipc_get(1);
    ipc_add_service(g_lpm_ipc[0], LPM_SERVER_ID, NULL, NULL);

    g_lpm_ipc[1] = ipc_get(2);
    ipc_add_service(g_lpm_ipc[1], LPM_SERVER_ID, NULL, NULL);
}

void app_lpm_init(void)
{
    int lpm_mode = 0;
    aos_kv_getint("lpm_en", &lpm_mode);
    LOGD(TAG, lpm_mode ? "lpm enable" : "lpm disable");

    if (!lpm_mode) {
        return;
    }
    pm_init(lpm_event_cb);
    ipc_init();

    pm_config_policy(LPM_POLICY_LOW_POWER);

    event_subscribe(EVENT_LPM_CHECK, lpm_check_event, NULL);

    event_publish_delay(EVENT_LPM_CHECK, NULL, 12000);
}
