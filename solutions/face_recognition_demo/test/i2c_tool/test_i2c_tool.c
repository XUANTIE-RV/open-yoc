#include <drv/iic.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <aos/kernel.h>

#define TAG "iic_tool"

static csi_iic_t iic_adapter_list[5] = {0};

#if 0
/* iic0 */
// mmio_write_32(0x0300116C, 0x0);//PAD_MIPIRX4N/CV_4WTDI_CR_SCL0
// mmio_write_32(0x03001170, 0x0);//PAD_MIPIRX4P/CV_4WTMS_CR_SDA0
// mmio_write_32(0x03001C3C, (mmio_read_32(0x03001C3C) | (0x1 << 2)) & ~(0x1 << 3));
// mmio_write_32(0x03001C38, (mmio_read_32(0x03001C38) | (0x1 << 2)) & ~(0x1 << 3));
/* iic2 */
// mmio_write_32(0x030011AC, 0x4);//PAD_MIPI_TXM1/IIC2_SDA
// mmio_write_32(0x030011B0, 0x4);//PAD_MIPI_TXP1/IIC2_SCL
// mmio_write_32(0x03001C78, (mmio_read_32(0x03001C78) | (0x1 << 2)) & ~(0x1 << 3));
// mmio_write_32(0x03001C7C, (mmio_read_32(0x03001C7C) | (0x1 << 2)) & ~(0x1 << 3));
int iic_write(csi_iic_t *iic)
{
#if 0
    int dev_addr = 0x56;
    int mem_addr = 0x10;
    char data[] = {0x14};
    int size = sizeof(data)/sizeof(data[0]);
    int timeout = 1000;
    int ret = 0;

    ret = csi_iic_mem_send(iic, dev_addr, mem_addr, IIC_MEM_ADDR_SIZE_8BIT, data, size, timeout);
    if (ret != size) {
        printf("csi_iic_mem_send error\n");
        ret = -1;
    }
#else
    int dev_addr = 0x56;
    char data[] = {0x10, 0x14};
    int size = sizeof(data)/sizeof(data[0]);
    int timeout = 1000;
    int ret = 0;

    ret = csi_iic_master_send(iic, dev_addr, data, size, timeout);
#endif

    return ret;
}

int iic_read(csi_iic_t *iic)
{
#if 0
    int dev_addr = 0x56;
    int mem_addr = 0x10;
    char data[1] = {0};
    int size = sizeof(data)/sizeof(data[0]);
    int timeout = 1000;
    int ret = 0;

    ret = csi_iic_mem_receive(iic, dev_addr, mem_addr, IIC_MEM_ADDR_SIZE_8BIT, data, size, timeout);
    if (ret != size) {
        printf("csi_iic_mem_receive error\n");
        ret = -1;
    }
#else
    int dev_addr = 0x56;
    char data[1] = {0};
    int size = sizeof(data)/sizeof(data[0]);
    int timeout = 1000;
    int ret = 0;

    data[0] = 0x10;
    csi_iic_master_send(iic, dev_addr, data, size, timeout);
    data[0] = 0x0;
    csi_iic_master_receive(iic, dev_addr, data, size, timeout);
#endif

    return ret;
}
#endif

static csi_error_t i2c_init_idx(csi_iic_t *iic, uint32_t idx,
                                csi_iic_mode_t mode,
                                csi_iic_addr_mode_t addr_mode,
                                csi_iic_speed_t speed)
{
    if (csi_iic_init(iic, idx)) {
        LOGE(TAG, "iic-%d csi_iic_init error", idx);
        return CSI_ERROR;
    }

    if (csi_iic_mode(iic, mode)){
        LOGE(TAG, "iic-%d csi_iic_mode error", idx);
        goto INIT_FAIL;
    }

    if (csi_iic_addr_mode(iic, addr_mode)) {
        LOGE(TAG, "iic-%d csi_iic_addr_mode error", idx);
        goto INIT_FAIL;
    }

    if (csi_iic_speed(iic, speed)) {
        LOGE(TAG, "iic-%d csi_iic_speed error", idx);
        goto INIT_FAIL;
    }

    return CSI_OK;

INIT_FAIL:
    csi_iic_uninit(iic);
    return CSI_ERROR;
}

static void i2c_uninit_idx(csi_iic_t *iic)
{
    csi_iic_uninit(iic);
}

static void i2c_detect_cmd(uint32_t idx)
{
    uint32_t i, j;
    csi_iic_t *iic = &iic_adapter_list[idx];
    uint32_t data_w = 0x1;
    uint8_t data;
    uint32_t timeout = 1;

    i2c_init_idx(iic, idx, IIC_MODE_MASTER, IIC_ADDRESS_7BIT,
                 IIC_BUS_SPEED_STANDARD);

    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

    for (i = 0; i < 128; i+= 16) {
        printf("%02x: ", i);
        for(j = 0; j < 16; j++) {
            fflush(stdout);
            /* Try to read the first location of the chip */
            if (csi_iic_master_receive(iic, i+j, &data, data_w, timeout) < 0)
				printf("-- ");
			else
				printf("%02x ", i+j);
        }
        printf("\n");
    }

    i2c_uninit_idx(iic);
}

static void i2c_read_cmd(uint32_t idx, uint32_t device_addr,
                    uint32_t reg_addr, uint32_t reg_width,
                    uint32_t data_width)
{
    uint32_t i = data_width;
    csi_iic_t *iic = &iic_adapter_list[idx];
    csi_iic_mem_addr_size_t reg_addr_len;
    uint8_t value[4] = {0};
    uint32_t timeout = 1;

    i2c_init_idx(iic, idx, IIC_MODE_MASTER, IIC_ADDRESS_7BIT,
                 IIC_BUS_SPEED_STANDARD);

    switch(reg_width) {
    default:
    case 0x1:reg_addr_len = IIC_MEM_ADDR_SIZE_8BIT;break;
    case 0x2:reg_addr_len = IIC_MEM_ADDR_SIZE_16BIT;break;
    }

    csi_iic_mem_receive(iic, device_addr, reg_addr, reg_addr_len,
                        value, data_width, timeout);

    i2c_uninit_idx(iic);

    while (i--) {
        printf("%#2x ", value[i]);
    }
    printf("\n");
}

static void i2c_write_cmd(uint32_t idx, uint32_t device_addr,
                     uint32_t reg_addr, uint32_t data,
                     uint32_t reg_width, uint32_t data_width)
{
    uint32_t i = data_width, j = 0;
    csi_iic_t *iic = &iic_adapter_list[idx];
    csi_iic_mem_addr_size_t reg_addr_len;
    uint8_t value[4] = {0};
    uint32_t timeout = 1;

    i2c_init_idx(iic, idx, IIC_MODE_MASTER, IIC_ADDRESS_7BIT,
                 IIC_BUS_SPEED_STANDARD);

    switch(reg_width) {
    default:
    case 0x1:reg_addr_len = IIC_MEM_ADDR_SIZE_8BIT;break;
    case 0x2:reg_addr_len = IIC_MEM_ADDR_SIZE_16BIT;break;
    }

    while (i--) {
        value[j++] = (data >> (i * 8)) & 0xff;
    }

    csi_iic_mem_send(iic, device_addr, reg_addr, reg_addr_len,
                     value, data_width, timeout);

    i2c_uninit_idx(iic);
}

static void print_usage(void)
{
    printf("usage:\n\tiic detect <idx>"
            "\n\tiic read <idx> <device_addr> <reg_addr> <reg_width> <data_width>"
            "\n\tiic write <idx> <device_addr> <reg_addr> <data> <reg_width> <data_width>\n");
}

void iic_tool_cmd(int argc, char **argv)
{
    uint32_t idx;
    uint32_t device_addr;
    uint32_t reg_addr;
    uint32_t data;
    uint32_t reg_width;
    uint32_t data_width;
    char *cmd = NULL;

    if (argc < 3) {
        print_usage();
        return;
    }

    cmd = argv[1];
    idx = strtol(argv[2], NULL, 16);

    if (!strcmp(cmd, "detect") && argc == 3) {
        i2c_detect_cmd(idx);
    } else if (!strcmp(cmd, "read") && argc == 7) {
        device_addr = strtol(argv[3], NULL, 16);
        reg_addr = strtol(argv[4], NULL, 16);
        reg_width = strtol(argv[5], NULL, 16);
        data_width = strtol(argv[6], NULL, 16);
        i2c_read_cmd(idx, device_addr, reg_addr, reg_width, data_width);
    } else if (!strcmp(cmd, "write") && argc == 8) {
        device_addr = strtol(argv[3], NULL, 16);
        reg_addr = strtol(argv[4], NULL, 16);
        data = strtol(argv[5], NULL, 16);
        reg_width = strtol(argv[6], NULL, 16);
        data_width = strtol(argv[7], NULL, 16);
        i2c_write_cmd(idx, device_addr, reg_addr, data, reg_width, data_width);
    } else {
        print_usage();
    }
}

ALIOS_CLI_CMD_REGISTER(iic_tool_cmd, iic, iic detect/read/write tool);
