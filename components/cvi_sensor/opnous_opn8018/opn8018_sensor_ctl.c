#include <unistd.h>
 
#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "drv/common.h"
#include "sensor_i2c.h"
#include "opn8018_cmos_ex.h"

static void opn8018_linear_480p120_init(VI_PIPE ViPipe);

CVI_U8 opn8018_i2c_addr = 0x3c;        /* I2C Address of OPN8018 */
const CVI_U32 opn8018_addr_byte = 1;
const CVI_U32 opn8018_data_byte = 1;
//static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int opn8018_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunOpn8018_BusInfo[ViPipe].s8I2cDev;
	return sensor_i2c_init(i2c_id);
}

int opn8018_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunOpn8018_BusInfo[ViPipe].s8I2cDev;
	return sensor_i2c_exit(i2c_id);
}

int opn8018_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunOpn8018_BusInfo[ViPipe].s8I2cDev;
	return sensor_i2c_read(i2c_id, opn8018_i2c_addr, (CVI_U32)addr, opn8018_addr_byte, opn8018_data_byte);
}

int opn8018_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunOpn8018_BusInfo[ViPipe].s8I2cDev;
	return sensor_i2c_write(i2c_id, opn8018_i2c_addr, (CVI_U32)addr, opn8018_addr_byte,
		(CVI_U32)data, opn8018_data_byte);
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void opn8018_standby(VI_PIPE ViPipe)
{
	opn8018_write_register(ViPipe, 0x05, 0x80); /* standby */
	opn8018_write_register(ViPipe, 0x06, 0x00); /* standby */
}

void opn8018_restart(VI_PIPE ViPipe)
{
	opn8018_write_register(ViPipe, 0x05, 0xff);
	opn8018_write_register(ViPipe, 0x06, 0x7f); /* restart */
}

void opn8018_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastOpn8018[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastOpn8018[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			opn8018_write_register(ViPipe,
				g_pastOpn8018[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOpn8018[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void opn8018_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 flip, mirror;

	flip = opn8018_read_register(ViPipe, 0x3820);
	mirror = opn8018_read_register(ViPipe, 0x3821);

	flip &= ~(0x1 << 2);
	mirror &= ~(0x1 << 2);

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		mirror |= 0x1 << 2;
		break;
	case ISP_SNS_FLIP:
		flip |= 0x1 << 2;
		break;
	case ISP_SNS_MIRROR_FLIP:
		flip |= 0x1 << 2;
		mirror |= 0x1 << 2;
		break;
	default:
		return;
	}
}

int opn8018_probe(VI_PIPE ViPipe)
{
	UNUSED(ViPipe);
	return CVI_SUCCESS;
}


void opn8018_init(VI_PIPE ViPipe)
{
	opn8018_i2c_init(ViPipe);

	opn8018_linear_480p120_init(ViPipe);

	g_pastOpn8018[ViPipe]->bInit = CVI_TRUE;
}

void opn8018_exit(VI_PIPE ViPipe)
{
	opn8018_i2c_exit(ViPipe);
}

/* 1944P30 and 1944P25 */
static void opn8018_linear_480p120_init(VI_PIPE ViPipe)
{

	opn8018_write_register(ViPipe, 0x0C, 0xFF);
	opn8018_write_register(ViPipe, 0x0D, 0x80);
	opn8018_write_register(ViPipe, 0x00, 0x64);
	opn8018_write_register(ViPipe, 0x69, 0x42);
	opn8018_write_register(ViPipe, 0x6B, 0x00);
	opn8018_write_register(ViPipe, 0x28, 0x00);
	opn8018_write_register(ViPipe, 0x29, 0x8D);
	opn8018_write_register(ViPipe, 0x2A, 0x00);
	opn8018_write_register(ViPipe, 0x2B, 0x14);
	opn8018_write_register(ViPipe, 0x2C, 0x00);
	opn8018_write_register(ViPipe, 0x2D, 0x20);
	opn8018_write_register(ViPipe, 0x36, 0x00);
	opn8018_write_register(ViPipe, 0x37, 0x16);
	opn8018_write_register(ViPipe, 0x38, 0x00);
	opn8018_write_register(ViPipe, 0x39, 0x08);
	opn8018_write_register(ViPipe, 0x3A, 0x00);
	opn8018_write_register(ViPipe, 0x3B, 0x20);
	opn8018_write_register(ViPipe, 0x3C, 0x00);
	opn8018_write_register(ViPipe, 0x3D, 0x08);
	opn8018_write_register(ViPipe, 0x3E, 0x00);
	opn8018_write_register(ViPipe, 0x3F, 0x2A);
	opn8018_write_register(ViPipe, 0x40, 0x00);
	opn8018_write_register(ViPipe, 0x41, 0x08);
	opn8018_write_register(ViPipe, 0x42, 0x00);
	opn8018_write_register(ViPipe, 0x43, 0x1A);
	opn8018_write_register(ViPipe, 0x44, 0x00);
	opn8018_write_register(ViPipe, 0x45, 0x24);
	opn8018_write_register(ViPipe, 0x09, 0x02);
	opn8018_write_register(ViPipe, 0x0F, 0x0B);
	opn8018_write_register(ViPipe, 0x10, 0xB8);
	opn8018_write_register(ViPipe, 0x13, 0x05);
	opn8018_write_register(ViPipe, 0x14, 0xFC);
	opn8018_write_register(ViPipe, 0x15, 0x0B);
	opn8018_write_register(ViPipe, 0x16, 0xB8);
	opn8018_write_register(ViPipe, 0x17, 0x05);
	opn8018_write_register(ViPipe, 0x18, 0xFD);
	opn8018_write_register(ViPipe, 0x19, 0x05);
	opn8018_write_register(ViPipe, 0x1A, 0xFE);
	opn8018_write_register(ViPipe, 0x1B, 0x0B);
	opn8018_write_register(ViPipe, 0x1C, 0xB6);
	opn8018_write_register(ViPipe, 0x1D, 0x02);
	opn8018_write_register(ViPipe, 0x1E, 0xBB);
	opn8018_write_register(ViPipe, 0x1F, 0x02);
	opn8018_write_register(ViPipe, 0x20, 0x44);
	opn8018_write_register(ViPipe, 0x25, 0x00);
	opn8018_write_register(ViPipe, 0x26, 0xDA);
	opn8018_write_register(ViPipe, 0x27, 0xC0);
	opn8018_write_register(ViPipe, 0x52, 0x04);
	opn8018_write_register(ViPipe, 0x53, 0x18);
	opn8018_write_register(ViPipe, 0x0E, 0x7F);
	opn8018_write_register(ViPipe, 0x0D, 0xFF);
	opn8018_write_register(ViPipe, 0x6C, 0x00);
	opn8018_write_register(ViPipe, 0x6D, 0x01);
	opn8018_write_register(ViPipe, 0x6F, 0x0F);

//	opn8018_default_reg_init(ViPipe);
	delay_ms(100);

	printf("ViPipe:%d,===OPN8018 480P 30fps 12bit LINE Init OK!===\n", ViPipe);
}

