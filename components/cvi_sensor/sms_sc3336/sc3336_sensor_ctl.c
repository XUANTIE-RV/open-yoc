#include <unistd.h>

#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "drv/common.h"
#include "sensor_i2c.h"
#include "sc3336_cmos_ex.h"

#define SC3336_CHIP_ID_HI_ADDR		0x3107
#define SC3336_CHIP_ID_LO_ADDR		0x3108
#define SC3336_CHIP_ID			0xcc41

static void sc3336_linear_1296P30_init(VI_PIPE ViPipe);

CVI_U8 sc3336_i2c_addr = 0x30;        /* I2C Address of SC3336 */
const CVI_U32 sc3336_addr_byte = 2;
const CVI_U32 sc3336_data_byte = 1;

int sc3336_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC3336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_init(i2c_id);
}

int sc3336_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC3336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_exit(i2c_id);
}

int sc3336_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC3336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_read(i2c_id, sc3336_i2c_addr, (CVI_U32)addr, sc3336_addr_byte, sc3336_data_byte);
}

int sc3336_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSC3336_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_write(i2c_id, sc3336_i2c_addr, (CVI_U32)addr, sc3336_addr_byte,
		(CVI_U32)data, sc3336_data_byte);
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void sc3336_standby(VI_PIPE ViPipe)
{
	sc3336_write_register(ViPipe, 0x0100, 0x00);
}

void sc3336_restart(VI_PIPE ViPipe)
{
	sc3336_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc3336_write_register(ViPipe, 0x0100, 0x01);
}

void sc3336_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC3336[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastSC3336[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			sc3336_write_register(ViPipe,
				g_pastSC3336[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC3336[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void sc3336_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc3336_write_register(ViPipe, 0x3221, val);
}

int sc3336_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	delay_ms(4);
	if (sc3336_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc3336_read_register(ViPipe, SC3336_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc3336_read_register(ViPipe, SC3336_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC3336_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void sc3336_init(VI_PIPE ViPipe)
{
	sc3336_i2c_init(ViPipe);

	//linear mode only
	sc3336_linear_1296P30_init(ViPipe);

	g_pastSC3336[ViPipe]->bInit = CVI_TRUE;
}

void sc3336_exit(VI_PIPE ViPipe)
{
	sc3336_i2c_exit(ViPipe);
}

/* 1296P30 and 1296P25 */
static void sc3336_linear_1296P30_init(VI_PIPE ViPipe)
{
	sc3336_write_register(ViPipe, 0x0103, 0x01);
	sc3336_write_register(ViPipe, 0x36e9, 0x80);
	sc3336_write_register(ViPipe, 0x37f9, 0x80);
	sc3336_write_register(ViPipe, 0x301f, 0x01);
	sc3336_write_register(ViPipe, 0x30b8, 0x33);
	sc3336_write_register(ViPipe, 0x3253, 0x10);
	sc3336_write_register(ViPipe, 0x325f, 0x20);
	sc3336_write_register(ViPipe, 0x3301, 0x04);
	sc3336_write_register(ViPipe, 0x3306, 0x50);
	sc3336_write_register(ViPipe, 0x3309, 0xa8);
	sc3336_write_register(ViPipe, 0x330a, 0x00);
	sc3336_write_register(ViPipe, 0x330b, 0xd8);
	sc3336_write_register(ViPipe, 0x3314, 0x13);
	sc3336_write_register(ViPipe, 0x331f, 0x99);
	sc3336_write_register(ViPipe, 0x3333, 0x10);
	sc3336_write_register(ViPipe, 0x3334, 0x40);
	sc3336_write_register(ViPipe, 0x335e, 0x06);
	sc3336_write_register(ViPipe, 0x335f, 0x0a);
	sc3336_write_register(ViPipe, 0x3364, 0x5e);
	sc3336_write_register(ViPipe, 0x337c, 0x02);
	sc3336_write_register(ViPipe, 0x337d, 0x0e);
	sc3336_write_register(ViPipe, 0x3390, 0x01);
	sc3336_write_register(ViPipe, 0x3391, 0x03);
	sc3336_write_register(ViPipe, 0x3392, 0x07);
	sc3336_write_register(ViPipe, 0x3393, 0x04);
	sc3336_write_register(ViPipe, 0x3394, 0x04);
	sc3336_write_register(ViPipe, 0x3395, 0x04);
	sc3336_write_register(ViPipe, 0x3396, 0x08);
	sc3336_write_register(ViPipe, 0x3397, 0x0b);
	sc3336_write_register(ViPipe, 0x3398, 0x1f);
	sc3336_write_register(ViPipe, 0x3399, 0x04);
	sc3336_write_register(ViPipe, 0x339a, 0x0a);
	sc3336_write_register(ViPipe, 0x339b, 0x3a);
	sc3336_write_register(ViPipe, 0x339c, 0xa0);
	sc3336_write_register(ViPipe, 0x33a2, 0x04);
	sc3336_write_register(ViPipe, 0x33ac, 0x08);
	sc3336_write_register(ViPipe, 0x33ad, 0x1c);
	sc3336_write_register(ViPipe, 0x33ae, 0x10);
	sc3336_write_register(ViPipe, 0x33af, 0x30);
	sc3336_write_register(ViPipe, 0x33b1, 0x80);
	sc3336_write_register(ViPipe, 0x33b3, 0x48);
	sc3336_write_register(ViPipe, 0x33f9, 0x60);
	sc3336_write_register(ViPipe, 0x33fb, 0x74);
	sc3336_write_register(ViPipe, 0x33fc, 0x4b);
	sc3336_write_register(ViPipe, 0x33fd, 0x5f);
	sc3336_write_register(ViPipe, 0x349f, 0x03);
	sc3336_write_register(ViPipe, 0x34a6, 0x4b);
	sc3336_write_register(ViPipe, 0x34a7, 0x5f);
	sc3336_write_register(ViPipe, 0x34a8, 0x20);
	sc3336_write_register(ViPipe, 0x34a9, 0x18);
	sc3336_write_register(ViPipe, 0x34ab, 0xe8);
	sc3336_write_register(ViPipe, 0x34ac, 0x01);
	sc3336_write_register(ViPipe, 0x34ad, 0x00);
	sc3336_write_register(ViPipe, 0x34f8, 0x5f);
	sc3336_write_register(ViPipe, 0x34f9, 0x18);
	sc3336_write_register(ViPipe, 0x3630, 0xc0);
	sc3336_write_register(ViPipe, 0x3631, 0x84);
	sc3336_write_register(ViPipe, 0x3632, 0x64);
	sc3336_write_register(ViPipe, 0x3633, 0x32);
	sc3336_write_register(ViPipe, 0x363b, 0x03);
	sc3336_write_register(ViPipe, 0x363c, 0x08);
	sc3336_write_register(ViPipe, 0x3641, 0x38);
	sc3336_write_register(ViPipe, 0x3670, 0x4e);
	sc3336_write_register(ViPipe, 0x3674, 0xc0);
	sc3336_write_register(ViPipe, 0x3675, 0xc0);
	sc3336_write_register(ViPipe, 0x3676, 0xc0);
	sc3336_write_register(ViPipe, 0x3677, 0x86);
	sc3336_write_register(ViPipe, 0x3678, 0x86);
	sc3336_write_register(ViPipe, 0x3679, 0x86);
	sc3336_write_register(ViPipe, 0x367c, 0x48);
	sc3336_write_register(ViPipe, 0x367d, 0x49);
	sc3336_write_register(ViPipe, 0x367e, 0x4b);
	sc3336_write_register(ViPipe, 0x367f, 0x5f);
	sc3336_write_register(ViPipe, 0x3690, 0x32);
	sc3336_write_register(ViPipe, 0x3691, 0x32);
	sc3336_write_register(ViPipe, 0x3692, 0x42);
	sc3336_write_register(ViPipe, 0x369c, 0x4b);
	sc3336_write_register(ViPipe, 0x369d, 0x5f);
	sc3336_write_register(ViPipe, 0x36b0, 0x87);
	sc3336_write_register(ViPipe, 0x36b1, 0x90);
	sc3336_write_register(ViPipe, 0x36b2, 0xa1);
	sc3336_write_register(ViPipe, 0x36b3, 0xd8);
	sc3336_write_register(ViPipe, 0x36b4, 0x49);
	sc3336_write_register(ViPipe, 0x36b5, 0x4b);
	sc3336_write_register(ViPipe, 0x36b6, 0x4f);
	sc3336_write_register(ViPipe, 0x370f, 0x01);
	sc3336_write_register(ViPipe, 0x3722, 0x09);
	sc3336_write_register(ViPipe, 0x3724, 0x41);
	sc3336_write_register(ViPipe, 0x3725, 0xc1);
	sc3336_write_register(ViPipe, 0x3771, 0x09);
	sc3336_write_register(ViPipe, 0x3772, 0x09);
	sc3336_write_register(ViPipe, 0x3773, 0x05);
	sc3336_write_register(ViPipe, 0x377a, 0x48);
	sc3336_write_register(ViPipe, 0x377b, 0x5f);
	sc3336_write_register(ViPipe, 0x3904, 0x04);
	sc3336_write_register(ViPipe, 0x3905, 0x8c);
	sc3336_write_register(ViPipe, 0x391d, 0x04);
	sc3336_write_register(ViPipe, 0x3921, 0x20);
	sc3336_write_register(ViPipe, 0x3926, 0x21);
	sc3336_write_register(ViPipe, 0x3933, 0x80);
	sc3336_write_register(ViPipe, 0x3934, 0x0a);
	sc3336_write_register(ViPipe, 0x3935, 0x00);
	sc3336_write_register(ViPipe, 0x3936, 0x2a);
	sc3336_write_register(ViPipe, 0x3937, 0x6a);
	sc3336_write_register(ViPipe, 0x3938, 0x6a);
	sc3336_write_register(ViPipe, 0x39dc, 0x02);
	sc3336_write_register(ViPipe, 0x3e01, 0x53);
	sc3336_write_register(ViPipe, 0x3e02, 0xe0);
	sc3336_write_register(ViPipe, 0x3e09, 0x00);
	sc3336_write_register(ViPipe, 0x440e, 0x02);
	sc3336_write_register(ViPipe, 0x4509, 0x20);
	sc3336_write_register(ViPipe, 0x5ae0, 0xfe);
	sc3336_write_register(ViPipe, 0x5ae1, 0x40);
	sc3336_write_register(ViPipe, 0x5ae2, 0x38);
	sc3336_write_register(ViPipe, 0x5ae3, 0x30);
	sc3336_write_register(ViPipe, 0x5ae4, 0x28);
	sc3336_write_register(ViPipe, 0x5ae5, 0x38);
	sc3336_write_register(ViPipe, 0x5ae6, 0x30);
	sc3336_write_register(ViPipe, 0x5ae7, 0x28);
	sc3336_write_register(ViPipe, 0x5ae8, 0x3f);
	sc3336_write_register(ViPipe, 0x5ae9, 0x34);
	sc3336_write_register(ViPipe, 0x5aea, 0x2c);
	sc3336_write_register(ViPipe, 0x5aeb, 0x3f);
	sc3336_write_register(ViPipe, 0x5aec, 0x34);
	sc3336_write_register(ViPipe, 0x5aed, 0x2c);
	sc3336_write_register(ViPipe, 0x36e9, 0x54);
	sc3336_write_register(ViPipe, 0x37f9, 0x27);

	sc3336_default_reg_init(ViPipe);

	sc3336_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC3336 1296P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
