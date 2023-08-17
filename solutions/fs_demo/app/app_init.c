#include <stdbool.h>
#include <aos/kv.h>
#include <uservice/uservice.h>
#include <yoc/partition.h>
#include <yoc/init.h>
#include <vfs.h>
#include <vfs_cli.h>
#include <board.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#endif
#ifdef CONFIG_USB_MSC
#include <usbh_core.h>
#include <usbh_cdc_acm.h>
#include <usbh_hid.h>
#include <usbh_msc.h>
#include <usbh_video.h>
#include <drv_usbh_class.h>
#endif

#define TAG "init"

static void stduart_init(void)
{
    extern void console_init(int idx, uint32_t baud, uint16_t buf_size);
    console_init(CONSOLE_UART_IDX, CONFIG_CLI_USART_BAUD, CONFIG_CONSOLE_UART_BUFSIZE);
}

static void fs_init(void)
{
    int ret;

    aos_vfs_init();
#ifdef CONFIG_FS_EXT4
    extern int vfs_ext4_register(void);
    ret = vfs_ext4_register();
    if (ret != 0) {
        LOGE(TAG, "ext4 register failed(%d).", ret);
        aos_assert(false);
    } else {
        LOGD(TAG, "ext4 register ok.");
    }
#endif
#ifdef CONFIG_FS_FAT
    extern int vfs_fatfs_register(void);
    ret = vfs_fatfs_register();
    if (ret != 0) {
        LOGE(TAG, "fatfs register failed(%d).may be you can input \"fs mountfatfs\" in cli console.", ret);
    } else {
        LOGD(TAG, "fatfs register ok.");
    }
#endif
#ifdef CONFIG_FS_LFS
    extern int32_t vfs_lfs_register(char *partition_desc);
    ret = vfs_lfs_register("lfs");
    if (ret != 0) {
        LOGE(TAG, "littlefs register failed(%d)", ret);
        aos_assert(false);
    } else {
        LOGD(TAG, "littlefs register ok.");
    }
#endif
    LOGI(TAG, "filesystem init ok.");
}

extern void board_cli_init();
void board_yoc_init(void)
{
    board_init();
    stduart_init();
    printf("###YoC###[%s,%s]\n", __DATE__, __TIME__);
    printf("cpu clock is %dHz\n", soc_get_cpu_freq(0));
    board_cli_init();
    event_service_init(NULL);
    ulog_init();
    aos_set_log_level(AOS_LL_DEBUG);
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
    aos_debug_init();
#endif
#ifdef CONFIG_USB_MSC
    rvm_usb_msc_drv_register(0);
    USBH_REGISTER_MSC_CLASS();
    USBH_REGISTER_CDCACM_CLASS();
    USBH_REGISTER_HID_MOUSE_CLASS();
    usbh_initialize();
#endif
    int ret = partition_init();
    if (ret <= 0) {
        LOGE(TAG, "partition init failed");
        aos_assert(false);
    }
    ret = aos_vfs_init();
    if (ret < 0) {
        LOGE(TAG, "vfs init failed");
        aos_assert(false);
    }
    fs_init();
}
