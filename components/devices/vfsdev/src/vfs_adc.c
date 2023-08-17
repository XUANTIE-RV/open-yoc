/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/adc.h>
#include <string.h>

static aos_status_t _devfs_adc_ioctl(devfs_file_t *file, int cmd, uintptr_t arg)
{
    int ret;
    rvm_dev_t *dev = devfs_file2dev(file);

    if (!dev) {
        return -EINVAL;
    }

    ret = 0;
    switch (cmd) {
    case ADC_IOC_GET_DEFAULT_CONFIG:
    {
        if (!(void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_adc_config_t config;
        rvm_hal_adc_config_default(&config);
        memcpy((void *)arg, &config, sizeof(rvm_hal_adc_config_t));
        break;
    }
    case ADC_IOC_SET_CONFIG:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        rvm_hal_adc_config_t config;
        memcpy(&config, (void *)arg, sizeof(rvm_hal_adc_config_t));
        if (rvm_hal_adc_config(dev, &config)) {
            return -EIO;
        }
        break;
    }
    case ADC_IOC_GET_PIN2CHANNEL:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }

        rvm_adc_dev_msg_t msg;
        memcpy(&msg, (const void *)arg, sizeof(rvm_adc_dev_msg_t));

        msg.ch = rvm_hal_adc_pin2channel(dev, msg.pin);

        memcpy((void *)arg, &msg, sizeof(rvm_adc_dev_msg_t));
        break;
    }
    case ADC_IOC_DMA_ENABLE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        bool enable;
        memcpy(&enable, (void *)arg, sizeof(bool));
        if (rvm_hal_adc_trans_dma_enable(dev, enable)) {
            return -EIO;
        }
        break;
    }
    case ADC_IOC_GET_VALUE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_adc_dev_msg_t msg;

        memcpy(&msg, (const void *)arg, sizeof(rvm_adc_dev_msg_t));

#ifdef CONFIG_CSI_V2
        if (rvm_hal_adc_read(dev, msg.ch, msg.output, msg.timeout)) {
            return -EIO;
        }

#else
        if (rvm_hal_adc_read(dev, msg.output, msg.timeout)) {
            return -EIO;
        }

#endif
        break;
    }
    case ADC_IOC_GET_MULTIPLE_VALUE:
    {
        if (!(const void *)arg) {
            ret = -EFAULT;
            break;
        }
        if (!devfs_file_is_readable(file)) {
            ret = -EPERM;
            break;
        }
        rvm_adc_dev_msg_t msg;

        memcpy(&msg, (const void *)arg, sizeof(rvm_adc_dev_msg_t));

#ifdef CONFIG_CSI_V2
        if (rvm_hal_adc_read_multiple(dev, msg.ch, msg.output, msg.num, msg.timeout)) {
            return -EIO;
        }

#endif
        break;
    }
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

static const devfs_file_ops_t devfs_adc_ops = {
    .ioctl      = _devfs_adc_ioctl,
    .poll       = NULL,
    .mmap       = NULL,
    .read       = NULL,
    .write      = NULL,
    .lseek      = NULL,
};

static devfs_ops_node_t adc_ops_node = {
    .name = "adc",
    .ops = (devfs_file_ops_t *)&devfs_adc_ops,
};

void adc_devfs_init(void)
{
    devices_add_devfs_ops_node(&adc_ops_node);
}
VFS_DEV_DRIVER_ENTRY(adc_devfs_init);

#endif