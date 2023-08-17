#include <unistd.h>

#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "drv/common.h"
#include "sensor_i2c.h"
#include "sc2336_cmos_ex.h"

static void sc2336_linear_1080p30_init(VI_PIPE ViPipe);

const CVI_U8 sc2336_i2c_addr = 0x30;        /* I2C Address of SC2336 */
const CVI_U32 sc2336_addr_byte = 2;
const CVI_U32 sc2336_data_byte = 1;
//static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc2336_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC2336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_init(i2c_id);
}

int sc2336_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC2336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_exit(i2c_id);
}

int sc2336_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC2336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_read(i2c_id, sc2336_i2c_addr, (CVI_U32)addr, sc2336_addr_byte, sc2336_data_byte);
}

int sc2336_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC2336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_write(i2c_id, sc2336_i2c_addr, (CVI_U32)addr, sc2336_addr_byte,
		(CVI_U32)data, sc2336_data_byte);
}

static void delay_ms(int ms)
{
	udelay(ms * 1000);
}

void sc2336_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			sc2336_write_register(ViPipe, addr, data);
	}
}

void sc2336_standby(VI_PIPE ViPipe)
{
	sc2336_write_register(ViPipe, 0x0100, 0x00);
}

void sc2336_restart(VI_PIPE ViPipe)
{
	sc2336_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc2336_write_register(ViPipe, 0x0100, 0x01);
}

void sc2336_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC2336[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc2336_write_register(ViPipe,
				g_pastSC2336[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC2336[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC2336_CHIP_ID_HI_ADDR		0x3107
#define SC2336_CHIP_ID_LO_ADDR		0x3108
#define SC2336_CHIP_ID			0xcb3a

void sc2336_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 val = 0;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		val |= 0x6;
		break;
	case ISP_SNS_FLIP:
		val |= 0x60;
		break;
	case ISP_SNS_MIRROR_FLIP:
		val |= 0x66;
		break;
	default:
		return;
	}

	sc2336_write_register(ViPipe, 0x3221, val);
}

int sc2336_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc2336_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc2336_read_register(ViPipe, SC2336_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc2336_read_register(ViPipe, SC2336_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC2336_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc2336_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_BOOL          bInit;

	bInit       = g_pastSC2336[ViPipe]->bInit;
	enWDRMode   = g_pastSC2336[ViPipe]->enWDRMode;

	sc2336_i2c_init(ViPipe);

	/* When sensor first init, config all registers */
	if (bInit == CVI_FALSE) {
		if (enWDRMode == WDR_MODE_NONE) {
			sc2336_linear_1080p30_init(ViPipe);
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else {
		if (enWDRMode == WDR_MODE_NONE) {
			sc2336_linear_1080p30_init(ViPipe);
		}
	}
	g_pastSC2336[ViPipe]->bInit = CVI_TRUE;
}

void sc2336_exit(VI_PIPE ViPipe)
{
	sc2336_i2c_exit(ViPipe);
}

/* 1080P30 and 1080P25 */
static void sc2336_linear_1080p30_init(VI_PIPE ViPipe)
{
	sc2336_write_register(ViPipe, 0x0103, 0x01);
	sc2336_write_register(ViPipe, 0x0100, 0x00);
	sc2336_write_register(ViPipe, 0x36e9, 0x80);
	sc2336_write_register(ViPipe, 0x37f9, 0x80);
	sc2336_write_register(ViPipe, 0x301f, 0x01);
	sc2336_write_register(ViPipe, 0x3106, 0x05);
	sc2336_write_register(ViPipe, 0x3248, 0x04);
	sc2336_write_register(ViPipe, 0x3249, 0x0b);
	sc2336_write_register(ViPipe, 0x3253, 0x08);
	sc2336_write_register(ViPipe, 0x3301, 0x09);
	sc2336_write_register(ViPipe, 0x3302, 0xff);
	sc2336_write_register(ViPipe, 0x3303, 0x10);
	sc2336_write_register(ViPipe, 0x3306, 0x60);
	sc2336_write_register(ViPipe, 0x3307, 0x02);
	sc2336_write_register(ViPipe, 0x330a, 0x01);
	sc2336_write_register(ViPipe, 0x330b, 0x10);
	sc2336_write_register(ViPipe, 0x330c, 0x16);
	sc2336_write_register(ViPipe, 0x330d, 0xff);
	sc2336_write_register(ViPipe, 0x3318, 0x02);
	sc2336_write_register(ViPipe, 0x3321, 0x0a);
	sc2336_write_register(ViPipe, 0x3327, 0x0e);
	sc2336_write_register(ViPipe, 0x332b, 0x12);
	sc2336_write_register(ViPipe, 0x3333, 0x10);
	sc2336_write_register(ViPipe, 0x3334, 0x40);
	sc2336_write_register(ViPipe, 0x335e, 0x06);
	sc2336_write_register(ViPipe, 0x335f, 0x0a);
	sc2336_write_register(ViPipe, 0x3364, 0x1f);
	sc2336_write_register(ViPipe, 0x337c, 0x02);
	sc2336_write_register(ViPipe, 0x337d, 0x0e);
	sc2336_write_register(ViPipe, 0x3390, 0x09);
	sc2336_write_register(ViPipe, 0x3391, 0x0f);
	sc2336_write_register(ViPipe, 0x3392, 0x1f);
	sc2336_write_register(ViPipe, 0x3393, 0x20);
	sc2336_write_register(ViPipe, 0x3394, 0x20);
	sc2336_write_register(ViPipe, 0x3395, 0xff);
	sc2336_write_register(ViPipe, 0x33a2, 0x04);
	sc2336_write_register(ViPipe, 0x33b1, 0x80);
	sc2336_write_register(ViPipe, 0x33b2, 0x68);
	sc2336_write_register(ViPipe, 0x33b3, 0x42);
	sc2336_write_register(ViPipe, 0x33f9, 0x70);
	sc2336_write_register(ViPipe, 0x33fb, 0xd0);
	sc2336_write_register(ViPipe, 0x33fc, 0x0f);
	sc2336_write_register(ViPipe, 0x33fd, 0x1f);
	sc2336_write_register(ViPipe, 0x349f, 0x03);
	sc2336_write_register(ViPipe, 0x34a6, 0x0f);
	sc2336_write_register(ViPipe, 0x34a7, 0x1f);
	sc2336_write_register(ViPipe, 0x34a8, 0x42);
	sc2336_write_register(ViPipe, 0x34a9, 0x06);
	sc2336_write_register(ViPipe, 0x34aa, 0x01);
	sc2336_write_register(ViPipe, 0x34ab, 0x23);
	sc2336_write_register(ViPipe, 0x34ac, 0x01);
	sc2336_write_register(ViPipe, 0x34ad, 0x84);
	sc2336_write_register(ViPipe, 0x3630, 0xf4);
	sc2336_write_register(ViPipe, 0x3633, 0x22);
	sc2336_write_register(ViPipe, 0x3639, 0xf4);
	sc2336_write_register(ViPipe, 0x363c, 0x47);
	sc2336_write_register(ViPipe, 0x3670, 0x09);
	sc2336_write_register(ViPipe, 0x3674, 0xf4);
	sc2336_write_register(ViPipe, 0x3675, 0xfb);
	sc2336_write_register(ViPipe, 0x3676, 0xed);
	sc2336_write_register(ViPipe, 0x367c, 0x09);
	sc2336_write_register(ViPipe, 0x367d, 0x0f);
	sc2336_write_register(ViPipe, 0x3690, 0x33);
	sc2336_write_register(ViPipe, 0x3691, 0x33);
	sc2336_write_register(ViPipe, 0x3692, 0x43);
	sc2336_write_register(ViPipe, 0x3698, 0x89);
	sc2336_write_register(ViPipe, 0x3699, 0x96);
	sc2336_write_register(ViPipe, 0x369a, 0xd0);
	sc2336_write_register(ViPipe, 0x369b, 0xd0);
	sc2336_write_register(ViPipe, 0x369c, 0x09);
	sc2336_write_register(ViPipe, 0x369d, 0x0f);
	sc2336_write_register(ViPipe, 0x36a2, 0x09);
	sc2336_write_register(ViPipe, 0x36a3, 0x0f);
	sc2336_write_register(ViPipe, 0x36a4, 0x1f);
	sc2336_write_register(ViPipe, 0x36d0, 0x01);
	sc2336_write_register(ViPipe, 0x3722, 0xe1);
	sc2336_write_register(ViPipe, 0x3724, 0x41);
	sc2336_write_register(ViPipe, 0x3725, 0xc1);
	sc2336_write_register(ViPipe, 0x3728, 0x20);
	sc2336_write_register(ViPipe, 0x3900, 0x0d);
	sc2336_write_register(ViPipe, 0x3905, 0x98);
	sc2336_write_register(ViPipe, 0x391b, 0x81);
	sc2336_write_register(ViPipe, 0x391c, 0x10);
	sc2336_write_register(ViPipe, 0x3933, 0x81);
	sc2336_write_register(ViPipe, 0x3934, 0xc5);
	sc2336_write_register(ViPipe, 0x3940, 0x68);
	sc2336_write_register(ViPipe, 0x3941, 0x00);
	sc2336_write_register(ViPipe, 0x3942, 0x01);
	sc2336_write_register(ViPipe, 0x3943, 0xc6);
	sc2336_write_register(ViPipe, 0x3952, 0x02);
	sc2336_write_register(ViPipe, 0x3953, 0x0f);
	sc2336_write_register(ViPipe, 0x3e01, 0x45);
	sc2336_write_register(ViPipe, 0x3e02, 0xf0);
	sc2336_write_register(ViPipe, 0x3e08, 0x1f);
	sc2336_write_register(ViPipe, 0x3e1b, 0x14);
	sc2336_write_register(ViPipe, 0x440e, 0x02);
	sc2336_write_register(ViPipe, 0x4509, 0x38);
	sc2336_write_register(ViPipe, 0x5799, 0x06);
	sc2336_write_register(ViPipe, 0x5ae0, 0xfe);
	sc2336_write_register(ViPipe, 0x5ae1, 0x40);
	sc2336_write_register(ViPipe, 0x5ae2, 0x30);
	sc2336_write_register(ViPipe, 0x5ae3, 0x28);
	sc2336_write_register(ViPipe, 0x5ae4, 0x20);
	sc2336_write_register(ViPipe, 0x5ae5, 0x30);
	sc2336_write_register(ViPipe, 0x5ae6, 0x28);
	sc2336_write_register(ViPipe, 0x5ae7, 0x20);
	sc2336_write_register(ViPipe, 0x5ae8, 0x3c);
	sc2336_write_register(ViPipe, 0x5ae9, 0x30);
	sc2336_write_register(ViPipe, 0x5aea, 0x28);
	sc2336_write_register(ViPipe, 0x5aeb, 0x3c);
	sc2336_write_register(ViPipe, 0x5aec, 0x30);
	sc2336_write_register(ViPipe, 0x5aed, 0x28);
	sc2336_write_register(ViPipe, 0x5aee, 0xfe);
	sc2336_write_register(ViPipe, 0x5aef, 0x40);
	sc2336_write_register(ViPipe, 0x5af4, 0x30);
	sc2336_write_register(ViPipe, 0x5af5, 0x28);
	sc2336_write_register(ViPipe, 0x5af6, 0x20);
	sc2336_write_register(ViPipe, 0x5af7, 0x30);
	sc2336_write_register(ViPipe, 0x5af8, 0x28);
	sc2336_write_register(ViPipe, 0x5af9, 0x20);
	sc2336_write_register(ViPipe, 0x5afa, 0x3c);
	sc2336_write_register(ViPipe, 0x5afb, 0x30);
	sc2336_write_register(ViPipe, 0x5afc, 0x28);
	sc2336_write_register(ViPipe, 0x5afd, 0x3c);
	sc2336_write_register(ViPipe, 0x5afe, 0x30);
	sc2336_write_register(ViPipe, 0x5aff, 0x28);
	sc2336_write_register(ViPipe, 0x36e9, 0x20);
	sc2336_write_register(ViPipe, 0x37f9, 0x27);
	sc2336_write_register(ViPipe, 0x0100, 0x01);

	sc2336_default_reg_init(ViPipe);

	sc2336_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC2336 1080P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
