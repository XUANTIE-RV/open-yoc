#include <unistd.h>

#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "drv/common.h"
#include "sensor_i2c.h"
#include "sc200ai_cmos_ex.h"

static void sc200ai_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
static void sc200ai_linear_1080p30_init(VI_PIPE ViPipe);

const CVI_U8 sc200ai_i2c_addr = 0x30;        /* I2C Address of SC200AI */
const CVI_U32 sc200ai_addr_byte = 2;
const CVI_U32 sc200ai_data_byte = 1;
//static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc200ai_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC200AI_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_init(i2c_id);
}

int sc200ai_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC200AI_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_exit(i2c_id);
}

int sc200ai_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC200AI_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_read(i2c_id, sc200ai_i2c_addr, (CVI_U32)addr, sc200ai_addr_byte, sc200ai_data_byte);
}

int sc200ai_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC200AI_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_write(i2c_id, sc200ai_i2c_addr, (CVI_U32)addr, sc200ai_addr_byte,
		(CVI_U32)data, sc200ai_data_byte);
}

static void delay_ms(int ms)
{
	udelay(ms * 1000);
}

void sc200ai_standby(VI_PIPE ViPipe)
{
	sc200ai_write_register(ViPipe, 0x0100, 0x00);
}

void sc200ai_restart(VI_PIPE ViPipe)
{
	sc200ai_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc200ai_write_register(ViPipe, 0x0100, 0x01);
}

void sc200ai_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC200AI[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc200ai_write_register(ViPipe,
				g_pastSC200AI[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC200AI[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC200AI_CHIP_ID_HI_ADDR		0x3107
#define SC200AI_CHIP_ID_LO_ADDR		0x3108
#define SC200AI_CHIP_ID			0xcb1c

void sc200ai_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc200ai_write_register(ViPipe, 0x3221, val);
}

int sc200ai_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	delay_ms(4);
	if (sc200ai_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc200ai_read_register(ViPipe, SC200AI_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc200ai_read_register(ViPipe, SC200AI_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC200AI_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc200ai_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_BOOL          bInit;
	CVI_U8            u8ImgMode;

	bInit       = g_pastSC200AI[ViPipe]->bInit;
	enWDRMode   = g_pastSC200AI[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastSC200AI[ViPipe]->u8ImgMode;

	sc200ai_i2c_init(ViPipe);

	/* When sensor first init, config all registers */
	if (bInit == CVI_FALSE) {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			if (u8ImgMode == SC200AI_MODE_1080P30_WDR) {
				/* SC200AI_MODE_1080P30_WDR */
				sc200ai_wdr_1080p30_2to1_init(ViPipe);
			} else {
			}
		} else {
			sc200ai_linear_1080p30_init(ViPipe);
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			if (u8ImgMode == SC200AI_MODE_1080P30_WDR) {
				/* SC200AI_MODE_1080P30_WDR */
				sc200ai_wdr_1080p30_2to1_init(ViPipe);
			} else {
			}
		} else {
			sc200ai_linear_1080p30_init(ViPipe);
		}
	}
	g_pastSC200AI[ViPipe]->bInit = CVI_TRUE;
}

void sc200ai_exit(VI_PIPE ViPipe)
{
	sc200ai_i2c_exit(ViPipe);
}

/* 1080P30 and 1080P25 */
static void sc200ai_linear_1080p30_init(VI_PIPE ViPipe)
{
	sc200ai_write_register(ViPipe, 0x0103, 0x01);
	sc200ai_write_register(ViPipe, 0x0100, 0x00);
	sc200ai_write_register(ViPipe, 0x36e9, 0x80);
	sc200ai_write_register(ViPipe, 0x36f9, 0x80);
	sc200ai_write_register(ViPipe, 0x59e0, 0x60);
	sc200ai_write_register(ViPipe, 0x59e1, 0x08);
	sc200ai_write_register(ViPipe, 0x59e2, 0x3f);
	sc200ai_write_register(ViPipe, 0x59e3, 0x18);
	sc200ai_write_register(ViPipe, 0x59e4, 0x18);
	sc200ai_write_register(ViPipe, 0x59e5, 0x3f);
	sc200ai_write_register(ViPipe, 0x59e6, 0x06);
	sc200ai_write_register(ViPipe, 0x59e7, 0x02);
	sc200ai_write_register(ViPipe, 0x59e8, 0x38);
	sc200ai_write_register(ViPipe, 0x59e9, 0x10);
	sc200ai_write_register(ViPipe, 0x59ea, 0x0c);
	sc200ai_write_register(ViPipe, 0x59eb, 0x10);
	sc200ai_write_register(ViPipe, 0x59ec, 0x04);
	sc200ai_write_register(ViPipe, 0x59ed, 0x02);
	sc200ai_write_register(ViPipe, 0x59ee, 0xa0);
	sc200ai_write_register(ViPipe, 0x59ef, 0x08);
	sc200ai_write_register(ViPipe, 0x59f4, 0x18);
	sc200ai_write_register(ViPipe, 0x59f5, 0x10);
	sc200ai_write_register(ViPipe, 0x59f6, 0x0c);
	sc200ai_write_register(ViPipe, 0x59f7, 0x10);
	sc200ai_write_register(ViPipe, 0x59f8, 0x06);
	sc200ai_write_register(ViPipe, 0x59f9, 0x02);
	sc200ai_write_register(ViPipe, 0x59fa, 0x18);
	sc200ai_write_register(ViPipe, 0x59fb, 0x10);
	sc200ai_write_register(ViPipe, 0x59fc, 0x0c);
	sc200ai_write_register(ViPipe, 0x59fd, 0x10);
	sc200ai_write_register(ViPipe, 0x59fe, 0x04);
	sc200ai_write_register(ViPipe, 0x59ff, 0x02);
	sc200ai_write_register(ViPipe, 0x3e16, 0x00);
	sc200ai_write_register(ViPipe, 0x3e17, 0x80);
	sc200ai_write_register(ViPipe, 0x3f09, 0x48);
	sc200ai_write_register(ViPipe, 0x3e01, 0x8c);
	sc200ai_write_register(ViPipe, 0x3e02, 0x20);
	sc200ai_write_register(ViPipe, 0x391f, 0x18);
	sc200ai_write_register(ViPipe, 0x363a, 0x1f);
	sc200ai_write_register(ViPipe, 0x3637, 0x1b);
	sc200ai_write_register(ViPipe, 0x391d, 0x14);
	sc200ai_write_register(ViPipe, 0x330b, 0x88);
	sc200ai_write_register(ViPipe, 0x3908, 0x41);
	sc200ai_write_register(ViPipe, 0x3333, 0x10);
	sc200ai_write_register(ViPipe, 0x3301, 0x20);
	sc200ai_write_register(ViPipe, 0x3304, 0x40);
	sc200ai_write_register(ViPipe, 0x331e, 0x39);
	sc200ai_write_register(ViPipe, 0x330f, 0x02);
	sc200ai_write_register(ViPipe, 0x3306, 0x32);
	sc200ai_write_register(ViPipe, 0x363c, 0x0e);
	sc200ai_write_register(ViPipe, 0x363b, 0xc6);
	sc200ai_write_register(ViPipe, 0x3622, 0x16);
	sc200ai_write_register(ViPipe, 0x5787, 0x10);
	sc200ai_write_register(ViPipe, 0x5788, 0x06);
	sc200ai_write_register(ViPipe, 0x578a, 0x10);
	sc200ai_write_register(ViPipe, 0x578b, 0x06);
	sc200ai_write_register(ViPipe, 0x5790, 0x10);
	sc200ai_write_register(ViPipe, 0x5791, 0x10);
	sc200ai_write_register(ViPipe, 0x5792, 0x00);
	sc200ai_write_register(ViPipe, 0x5793, 0x10);
	sc200ai_write_register(ViPipe, 0x5794, 0x10);
	sc200ai_write_register(ViPipe, 0x5795, 0x00);
	sc200ai_write_register(ViPipe, 0x5799, 0x00);
	sc200ai_write_register(ViPipe, 0x57c7, 0x10);
	sc200ai_write_register(ViPipe, 0x57c8, 0x06);
	sc200ai_write_register(ViPipe, 0x57ca, 0x10);
	sc200ai_write_register(ViPipe, 0x57cb, 0x06);
	sc200ai_write_register(ViPipe, 0x57d1, 0x10);
	sc200ai_write_register(ViPipe, 0x57d4, 0x10);
	sc200ai_write_register(ViPipe, 0x57d9, 0x00);
	sc200ai_write_register(ViPipe, 0x369c, 0x40);
	sc200ai_write_register(ViPipe, 0x369d, 0x48);
	sc200ai_write_register(ViPipe, 0x3690, 0x34);
	sc200ai_write_register(ViPipe, 0x3691, 0x33);
	sc200ai_write_register(ViPipe, 0x3692, 0x44);
	sc200ai_write_register(ViPipe, 0x3670, 0x0a);
	sc200ai_write_register(ViPipe, 0x367c, 0x48);
	sc200ai_write_register(ViPipe, 0x367d, 0x58);
	sc200ai_write_register(ViPipe, 0x3674, 0x82);
	sc200ai_write_register(ViPipe, 0x3675, 0x76);
	sc200ai_write_register(ViPipe, 0x3676, 0x78);
	sc200ai_write_register(ViPipe, 0x3253, 0x08);
	sc200ai_write_register(ViPipe, 0x301f, 0x03);
	sc200ai_write_register(ViPipe, 0x36e9, 0x20);
	sc200ai_write_register(ViPipe, 0x36f9, 0x27);

	sc200ai_default_reg_init(ViPipe);

	sc200ai_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC200AI 1080P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

static void sc200ai_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
	sc200ai_write_register(ViPipe, 0x0103, 0x01);
	sc200ai_write_register(ViPipe, 0x0100, 0x00);
	sc200ai_write_register(ViPipe, 0x36e9, 0x80);
	sc200ai_write_register(ViPipe, 0x36f9, 0x80);
	sc200ai_write_register(ViPipe, 0x59e0, 0x60);
	sc200ai_write_register(ViPipe, 0x59e1, 0x08);
	sc200ai_write_register(ViPipe, 0x59e2, 0x3f);
	sc200ai_write_register(ViPipe, 0x59e3, 0x18);
	sc200ai_write_register(ViPipe, 0x59e4, 0x18);
	sc200ai_write_register(ViPipe, 0x59e5, 0x3f);
	sc200ai_write_register(ViPipe, 0x59e6, 0x06);
	sc200ai_write_register(ViPipe, 0x59e7, 0x02);
	sc200ai_write_register(ViPipe, 0x59e8, 0x38);
	sc200ai_write_register(ViPipe, 0x59e9, 0x10);
	sc200ai_write_register(ViPipe, 0x59ea, 0x0c);
	sc200ai_write_register(ViPipe, 0x59eb, 0x10);
	sc200ai_write_register(ViPipe, 0x59ec, 0x04);
	sc200ai_write_register(ViPipe, 0x59ed, 0x02);
	sc200ai_write_register(ViPipe, 0x59ee, 0xa0);
	sc200ai_write_register(ViPipe, 0x59ef, 0x08);
	sc200ai_write_register(ViPipe, 0x59f4, 0x18);
	sc200ai_write_register(ViPipe, 0x59f5, 0x10);
	sc200ai_write_register(ViPipe, 0x59f6, 0x0c);
	sc200ai_write_register(ViPipe, 0x59f7, 0x10);
	sc200ai_write_register(ViPipe, 0x59f8, 0x06);
	sc200ai_write_register(ViPipe, 0x59f9, 0x02);
	sc200ai_write_register(ViPipe, 0x59fa, 0x18);
	sc200ai_write_register(ViPipe, 0x59fb, 0x10);
	sc200ai_write_register(ViPipe, 0x59fc, 0x0c);
	sc200ai_write_register(ViPipe, 0x59fd, 0x10);
	sc200ai_write_register(ViPipe, 0x59fe, 0x04);
	sc200ai_write_register(ViPipe, 0x59ff, 0x02);
	sc200ai_write_register(ViPipe, 0x3e16, 0x00);
	sc200ai_write_register(ViPipe, 0x3e17, 0x80);
	sc200ai_write_register(ViPipe, 0x3f09, 0x48);
	sc200ai_write_register(ViPipe, 0x391f, 0x10);
	sc200ai_write_register(ViPipe, 0x363a, 0x1f);
	sc200ai_write_register(ViPipe, 0x3908, 0x41);
	sc200ai_write_register(ViPipe, 0x330d, 0x16);
	sc200ai_write_register(ViPipe, 0x3302, 0x0c);
	sc200ai_write_register(ViPipe, 0x3303, 0x08);
	sc200ai_write_register(ViPipe, 0x3308, 0x10);
	sc200ai_write_register(ViPipe, 0x3310, 0x02);
	sc200ai_write_register(ViPipe, 0x334c, 0x08);
	sc200ai_write_register(ViPipe, 0x330f, 0x02);
	sc200ai_write_register(ViPipe, 0x330e, 0x1c);
	sc200ai_write_register(ViPipe, 0x331c, 0x04);
	sc200ai_write_register(ViPipe, 0x3320, 0x07);
	sc200ai_write_register(ViPipe, 0x33ac, 0x08);
	sc200ai_write_register(ViPipe, 0x33ae, 0x10);
	sc200ai_write_register(ViPipe, 0x3356, 0x09);
	sc200ai_write_register(ViPipe, 0x33af, 0x19);
	sc200ai_write_register(ViPipe, 0x3333, 0x10);
	sc200ai_write_register(ViPipe, 0x3622, 0x16);
	sc200ai_write_register(ViPipe, 0x3630, 0xa0);
	sc200ai_write_register(ViPipe, 0x36eb, 0x0c);
	sc200ai_write_register(ViPipe, 0x36ec, 0x0c);
	sc200ai_write_register(ViPipe, 0x36fd, 0x14);
	sc200ai_write_register(ViPipe, 0x5787, 0x10);
	sc200ai_write_register(ViPipe, 0x5788, 0x06);
	sc200ai_write_register(ViPipe, 0x578a, 0x10);
	sc200ai_write_register(ViPipe, 0x578b, 0x06);
	sc200ai_write_register(ViPipe, 0x5790, 0x10);
	sc200ai_write_register(ViPipe, 0x5791, 0x10);
	sc200ai_write_register(ViPipe, 0x5792, 0x00);
	sc200ai_write_register(ViPipe, 0x5793, 0x10);
	sc200ai_write_register(ViPipe, 0x5794, 0x10);
	sc200ai_write_register(ViPipe, 0x5795, 0x00);
	sc200ai_write_register(ViPipe, 0x5799, 0x00);
	sc200ai_write_register(ViPipe, 0x57c7, 0x10);
	sc200ai_write_register(ViPipe, 0x57c8, 0x06);
	sc200ai_write_register(ViPipe, 0x57ca, 0x10);
	sc200ai_write_register(ViPipe, 0x57cb, 0x06);
	sc200ai_write_register(ViPipe, 0x57d1, 0x10);
	sc200ai_write_register(ViPipe, 0x57d4, 0x10);
	sc200ai_write_register(ViPipe, 0x57d9, 0x00);
	sc200ai_write_register(ViPipe, 0x3364, 0x17);
	sc200ai_write_register(ViPipe, 0x3390, 0x08);
	sc200ai_write_register(ViPipe, 0x3391, 0x18);
	sc200ai_write_register(ViPipe, 0x3392, 0x38);
	sc200ai_write_register(ViPipe, 0x3301, 0x06);
	sc200ai_write_register(ViPipe, 0x3393, 0x06);
	sc200ai_write_register(ViPipe, 0x3394, 0x06);
	sc200ai_write_register(ViPipe, 0x3395, 0x06);
	sc200ai_write_register(ViPipe, 0x3396, 0x08);
	sc200ai_write_register(ViPipe, 0x3397, 0x18);
	sc200ai_write_register(ViPipe, 0x3398, 0x38);
	sc200ai_write_register(ViPipe, 0x3399, 0x06);
	sc200ai_write_register(ViPipe, 0x339a, 0x0a);
	sc200ai_write_register(ViPipe, 0x339b, 0x10);
	sc200ai_write_register(ViPipe, 0x339c, 0x20);
	sc200ai_write_register(ViPipe, 0x369c, 0x40);
	sc200ai_write_register(ViPipe, 0x369d, 0x48);
	sc200ai_write_register(ViPipe, 0x3690, 0x34);
	sc200ai_write_register(ViPipe, 0x3691, 0x33);
	sc200ai_write_register(ViPipe, 0x3692, 0x44);
	sc200ai_write_register(ViPipe, 0x3670, 0x0a);
	sc200ai_write_register(ViPipe, 0x367c, 0x48);
	sc200ai_write_register(ViPipe, 0x367d, 0x58);
	sc200ai_write_register(ViPipe, 0x3674, 0x82);
	sc200ai_write_register(ViPipe, 0x3675, 0x76);
	sc200ai_write_register(ViPipe, 0x3676, 0x78);
	sc200ai_write_register(ViPipe, 0x3637, 0x36);
	sc200ai_write_register(ViPipe, 0x3304, 0x60);
	sc200ai_write_register(ViPipe, 0x3309, 0x70);
	sc200ai_write_register(ViPipe, 0x331e, 0x51);
	sc200ai_write_register(ViPipe, 0x331f, 0x61);
	sc200ai_write_register(ViPipe, 0x3306, 0x30);
	sc200ai_write_register(ViPipe, 0x330b, 0x80);
	sc200ai_write_register(ViPipe, 0x363c, 0x0e);
	sc200ai_write_register(ViPipe, 0x363b, 0xc6);
	sc200ai_write_register(ViPipe, 0x3253, 0x08);
	sc200ai_write_register(ViPipe, 0x3220, 0x53);
	sc200ai_write_register(ViPipe, 0x3250, 0x3f);
	sc200ai_write_register(ViPipe, 0x320e, 0x08);
	sc200ai_write_register(ViPipe, 0x320f, 0xca);
	sc200ai_write_register(ViPipe, 0x4816, 0xb1);
	sc200ai_write_register(ViPipe, 0x3e00, 0x01);
	sc200ai_write_register(ViPipe, 0x3e01, 0x06);
	sc200ai_write_register(ViPipe, 0x3e02, 0x00);
	sc200ai_write_register(ViPipe, 0x3e04, 0x10);
	sc200ai_write_register(ViPipe, 0x3e05, 0x60);
	sc200ai_write_register(ViPipe, 0x3e06, 0x00);
	sc200ai_write_register(ViPipe, 0x3e07, 0x80);
	sc200ai_write_register(ViPipe, 0x3e08, 0x03);
	sc200ai_write_register(ViPipe, 0x3e09, 0x40);
	sc200ai_write_register(ViPipe, 0x3e10, 0x00);
	sc200ai_write_register(ViPipe, 0x3e11, 0x80);
	sc200ai_write_register(ViPipe, 0x3e12, 0x03);
	sc200ai_write_register(ViPipe, 0x3e13, 0x40);
	sc200ai_write_register(ViPipe, 0x3e23, 0x00);
	sc200ai_write_register(ViPipe, 0x3e24, 0x88);
	sc200ai_write_register(ViPipe, 0x301f, 0x02);
	sc200ai_write_register(ViPipe, 0x36e9, 0x20);
	sc200ai_write_register(ViPipe, 0x36f9, 0x24);

	sc200ai_default_reg_init(ViPipe);

	sc200ai_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(50);

	printf("===SC200AI sensor 1080P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");
}
