#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "cvi_sns_ctrl.h"
#include "gc2053_cmos_ex.h"
#include "drv/common.h"
#include "sensor_i2c.h"
#include <unistd.h>


#define GC2053_CHIP_ID_ADDR_H   0xf0
#define GC2053_CHIP_ID_ADDR_L   0xf1
#define GC2053_CHIP_ID          0x2053

static void gc2053_linear_1080p30_init(VI_PIPE ViPipe);

CVI_U8 gc2053_i2c_addr = 0x37;//0x6e
const CVI_U32 gc2053_addr_byte = 1;
const CVI_U32 gc2053_data_byte = 1;
//static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int gc2053_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunGc2053_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_init(i2c_id);
}

int gc2053_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunGc2053_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_exit(i2c_id);
}

int gc2053_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunGc2053_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_read(i2c_id, gc2053_i2c_addr, (CVI_U32)addr, gc2053_addr_byte, gc2053_data_byte);
}


int gc2053_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunGc2053_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_write(i2c_id, gc2053_i2c_addr, (CVI_U32)addr, gc2053_addr_byte,
				(CVI_U32)data, gc2053_data_byte);
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void gc2053_standby(VI_PIPE ViPipe)
{
	gc2053_write_register(ViPipe, 0x3e, 0x00);
	gc2053_write_register(ViPipe, 0xf7, 0x00);
	gc2053_write_register(ViPipe, 0xfc, 0x01);
	gc2053_write_register(ViPipe, 0xf9, 0x83);

	printf("gc2053_standby\n");
}

void gc2053_restart(VI_PIPE ViPipe)
{
	gc2053_write_register(ViPipe, 0xf9, 0x82);
	delay_ms(2);
	gc2053_write_register(ViPipe, 0xf7, 0x01);
	gc2053_write_register(ViPipe, 0xfc, 0x8e);
	gc2053_write_register(ViPipe, 0x3e, 0x91);

	printf("gc2053_restart\n");
}
#if 0
void gc2053_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastGc2053[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		gc2053_write_register(ViPipe,
				g_pastGc2053[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastGc2053[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}
#endif
void gc2053_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 value = 0;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		value = 0x01;
		break;
	case ISP_SNS_FLIP:
		value = 0x02;
		break;
	case ISP_SNS_MIRROR_FLIP:
		value = 0x03;
		break;
	default:
		return;
	}
	gc2053_write_register(ViPipe, 0xfe, 0x00);
	gc2053_write_register(ViPipe, 0x17, value);
}

int gc2053_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	usleep(50);
	if (gc2053_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = gc2053_read_register(ViPipe, GC2053_CHIP_ID_ADDR_H);
	nVal2 = gc2053_read_register(ViPipe, GC2053_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != GC2053_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void gc2053_init(VI_PIPE ViPipe)
{
	WDR_MODE_E enWDRMode = g_pastGc2053[ViPipe]->enWDRMode;

	gc2053_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "not surpport this WDR_MODE_E!\n");
	} else {
		gc2053_linear_1080p30_init(ViPipe);
	}
	g_pastGc2053[ViPipe]->bInit = CVI_TRUE;
}

void gc2053_exit(VI_PIPE ViPipe)
{
	gc2053_i2c_exit(ViPipe);
}

static void gc2053_linear_1080p30_init(VI_PIPE ViPipe)
{
	/****system****/
	gc2053_write_register(ViPipe, 0xfe, 0x80);
	gc2053_write_register(ViPipe, 0xfe, 0x80);
	gc2053_write_register(ViPipe, 0xfe, 0x80);
	gc2053_write_register(ViPipe, 0xfe, 0x00);
	gc2053_write_register(ViPipe, 0xf2, 0x00);
	gc2053_write_register(ViPipe, 0xf3, 0x00);
	gc2053_write_register(ViPipe, 0xf4, 0x36);
	gc2053_write_register(ViPipe, 0xf5, 0xc0);
	gc2053_write_register(ViPipe, 0xf6, 0x44);
	gc2053_write_register(ViPipe, 0xf7, 0x01);
	gc2053_write_register(ViPipe, 0xf8, 0x2c);
	gc2053_write_register(ViPipe, 0xf9, 0x42);
	gc2053_write_register(ViPipe, 0xfc, 0x8e);
	/****CISCTL & ANALOG****/
	gc2053_write_register(ViPipe, 0xfe, 0x00);
	gc2053_write_register(ViPipe, 0x87, 0x18);
	gc2053_write_register(ViPipe, 0xee, 0x30);
	gc2053_write_register(ViPipe, 0xd0, 0xb7);
	gc2053_write_register(ViPipe, 0x03, 0x04);
	gc2053_write_register(ViPipe, 0x04, 0x60);
	gc2053_write_register(ViPipe, 0x05, 0x04);
	gc2053_write_register(ViPipe, 0x06, 0x4c);
	gc2053_write_register(ViPipe, 0x07, 0x00);
	gc2053_write_register(ViPipe, 0x08, 0x11);
	gc2053_write_register(ViPipe, 0x09, 0x00);
	gc2053_write_register(ViPipe, 0x0a, 0x02);
	gc2053_write_register(ViPipe, 0x0b, 0x00);
	gc2053_write_register(ViPipe, 0x0c, 0x02);
	gc2053_write_register(ViPipe, 0x0d, 0x04);
	gc2053_write_register(ViPipe, 0x0e, 0x40);
	gc2053_write_register(ViPipe, 0x12, 0xe2);
	gc2053_write_register(ViPipe, 0x13, 0x16);
	gc2053_write_register(ViPipe, 0x19, 0x0a);
	gc2053_write_register(ViPipe, 0x21, 0x1c);
	gc2053_write_register(ViPipe, 0x28, 0x0a);
	gc2053_write_register(ViPipe, 0x29, 0x24);
	gc2053_write_register(ViPipe, 0x2b, 0x04);
	gc2053_write_register(ViPipe, 0x32, 0xf8);
	gc2053_write_register(ViPipe, 0x37, 0x03);
	gc2053_write_register(ViPipe, 0x39, 0x15);
	gc2053_write_register(ViPipe, 0x41, 0x04);
	gc2053_write_register(ViPipe, 0x42, 0x65);
	gc2053_write_register(ViPipe, 0x43, 0x07);
	gc2053_write_register(ViPipe, 0x44, 0x40);
	gc2053_write_register(ViPipe, 0x46, 0x0b);
	gc2053_write_register(ViPipe, 0x4b, 0x20);
	gc2053_write_register(ViPipe, 0x4e, 0x08);
	gc2053_write_register(ViPipe, 0x55, 0x20);
	gc2053_write_register(ViPipe, 0x66, 0x05);
	gc2053_write_register(ViPipe, 0x67, 0x05);
	gc2053_write_register(ViPipe, 0x77, 0x01);
	gc2053_write_register(ViPipe, 0x78, 0x00);
	gc2053_write_register(ViPipe, 0x7c, 0x93);
	gc2053_write_register(ViPipe, 0x8c, 0x12);
	gc2053_write_register(ViPipe, 0x8d, 0x92);
	gc2053_write_register(ViPipe, 0x90, 0x00);
	gc2053_write_register(ViPipe, 0x9d, 0x10);
	gc2053_write_register(ViPipe, 0xce, 0x7c);
	gc2053_write_register(ViPipe, 0xd2, 0x41);
	gc2053_write_register(ViPipe, 0xd3, 0xdc);
	gc2053_write_register(ViPipe, 0xe6, 0x50);
	/*gain*/
	gc2053_write_register(ViPipe, 0xb6, 0xc0);
	gc2053_write_register(ViPipe, 0xb0, 0x70);
	gc2053_write_register(ViPipe, 0xb1, 0x01);
	gc2053_write_register(ViPipe, 0xb2, 0x00);
	gc2053_write_register(ViPipe, 0xb3, 0x00);
	gc2053_write_register(ViPipe, 0xb4, 0x00);
	gc2053_write_register(ViPipe, 0xb8, 0x01);
	gc2053_write_register(ViPipe, 0xb9, 0x00);
	/*blk*/
	gc2053_write_register(ViPipe, 0x26, 0x30);
	gc2053_write_register(ViPipe, 0xfe, 0x01);
	gc2053_write_register(ViPipe, 0x40, 0x23);
	gc2053_write_register(ViPipe, 0x55, 0x07);
	gc2053_write_register(ViPipe, 0x60, 0x40);
	gc2053_write_register(ViPipe, 0xfe, 0x04);
	gc2053_write_register(ViPipe, 0x14, 0x78);
	gc2053_write_register(ViPipe, 0x15, 0x78);
	gc2053_write_register(ViPipe, 0x16, 0x78);
	gc2053_write_register(ViPipe, 0x17, 0x78);
	/*window*/
	gc2053_write_register(ViPipe, 0xfe, 0x01);
	gc2053_write_register(ViPipe, 0x91, 0x00);
	gc2053_write_register(ViPipe, 0x92, 0x00);
	gc2053_write_register(ViPipe, 0x93, 0x00);
	gc2053_write_register(ViPipe, 0x94, 0x03);
	gc2053_write_register(ViPipe, 0x95, 0x04);
	gc2053_write_register(ViPipe, 0x96, 0x38);
	gc2053_write_register(ViPipe, 0x97, 0x07);
	gc2053_write_register(ViPipe, 0x98, 0x80);
	/*ISP*/
	gc2053_write_register(ViPipe, 0xfe, 0x01);
	gc2053_write_register(ViPipe, 0x01, 0x05);
	gc2053_write_register(ViPipe, 0x02, 0x89);
	gc2053_write_register(ViPipe, 0x04, 0x01);
	gc2053_write_register(ViPipe, 0x07, 0xa6);
	gc2053_write_register(ViPipe, 0x08, 0xa9);
	gc2053_write_register(ViPipe, 0x09, 0xa8);
	gc2053_write_register(ViPipe, 0x0a, 0xa7);
	gc2053_write_register(ViPipe, 0x0b, 0xff);
	gc2053_write_register(ViPipe, 0x0c, 0xff);
	gc2053_write_register(ViPipe, 0x0f, 0x00);
	gc2053_write_register(ViPipe, 0x50, 0x1c);
	gc2053_write_register(ViPipe, 0x89, 0x03);
	gc2053_write_register(ViPipe, 0xfe, 0x04);
	gc2053_write_register(ViPipe, 0x28, 0x86);
	gc2053_write_register(ViPipe, 0x29, 0x86);
	gc2053_write_register(ViPipe, 0x2a, 0x86);
	gc2053_write_register(ViPipe, 0x2b, 0x68);
	gc2053_write_register(ViPipe, 0x2c, 0x68);
	gc2053_write_register(ViPipe, 0x2d, 0x68);
	gc2053_write_register(ViPipe, 0x2e, 0x68);
	gc2053_write_register(ViPipe, 0x2f, 0x68);
	gc2053_write_register(ViPipe, 0x30, 0x4f);
	gc2053_write_register(ViPipe, 0x31, 0x68);
	gc2053_write_register(ViPipe, 0x32, 0x67);
	gc2053_write_register(ViPipe, 0x33, 0x66);
	gc2053_write_register(ViPipe, 0x34, 0x66);
	gc2053_write_register(ViPipe, 0x35, 0x66);
	gc2053_write_register(ViPipe, 0x36, 0x66);
	gc2053_write_register(ViPipe, 0x37, 0x66);
	gc2053_write_register(ViPipe, 0x38, 0x62);
	gc2053_write_register(ViPipe, 0x39, 0x62);
	gc2053_write_register(ViPipe, 0x3a, 0x62);
	gc2053_write_register(ViPipe, 0x3b, 0x62);
	gc2053_write_register(ViPipe, 0x3c, 0x62);
	gc2053_write_register(ViPipe, 0x3d, 0x62);
	gc2053_write_register(ViPipe, 0x3e, 0x62);
	gc2053_write_register(ViPipe, 0x3f, 0x62);
	/****DVP & MIPI****/
	gc2053_write_register(ViPipe, 0xfe, 0x01);
	gc2053_write_register(ViPipe, 0x9a, 0x06);
	gc2053_write_register(ViPipe, 0xfe, 0x00);
	gc2053_write_register(ViPipe, 0x7b, 0x2a);
	gc2053_write_register(ViPipe, 0x23, 0x2d);
	gc2053_write_register(ViPipe, 0xfe, 0x03);
	gc2053_write_register(ViPipe, 0x01, 0x27);
	gc2053_write_register(ViPipe, 0x02, 0x56);
	gc2053_write_register(ViPipe, 0x03, 0x8e);
	gc2053_write_register(ViPipe, 0x12, 0x80);
	gc2053_write_register(ViPipe, 0x13, 0x07);
	gc2053_write_register(ViPipe, 0x15, 0x12);
	gc2053_write_register(ViPipe, 0xfe, 0x00);
	gc2053_write_register(ViPipe, 0x3e, 0x91);

	//gc2053_default_reg_init(ViPipe);
	printf("ViPipe:%d,===GC2053 1080P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

