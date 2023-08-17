#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "cvi_sns_ctrl.h"
#include "drv/common.h"
#include "sensor_i2c.h"
#include <unistd.h>

#include "sc030iot_cmos_ex.h"

#define SC030IOT_CHIP_ID_ADDR_H	0xf7
#define SC030IOT_CHIP_ID_ADDR_L	0xf8
#define SC030IOT_CHIP_ID		0x9a46


static void sc030iot_linear_480p30_init(VI_PIPE ViPipe);

CVI_U8 sc030iot_i2c_addr = 0x68;
const CVI_U32 sc030iot_addr_byte = 1;
const CVI_U32 sc030iot_data_byte = 1;

int sc030iot_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSc030iot_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_init(i2c_id);
}

int sc030iot_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSc030iot_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_exit(i2c_id);

}

int sc030iot_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSc030iot_BusInfo[ViPipe].s8I2cDev;

    return sensor_i2c_read(i2c_id, sc030iot_i2c_addr, (CVI_U32)addr, sc030iot_addr_byte, sc030iot_data_byte);
}

int sc030iot_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunSc030iot_BusInfo[ViPipe].s8I2cDev;

    return sensor_i2c_write(i2c_id, sc030iot_i2c_addr, (CVI_U32)addr, sc030iot_addr_byte,
            (CVI_U32)data, sc030iot_data_byte);
}

int sc030iot_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	// udelay(50);
	if (sc030iot_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = sc030iot_read_register(ViPipe, SC030IOT_CHIP_ID_ADDR_H);
	nVal2 = sc030iot_read_register(ViPipe, SC030IOT_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != SC030IOT_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void sc030iot_init(VI_PIPE ViPipe)
{
	sc030iot_i2c_init(ViPipe);

	sc030iot_linear_480p30_init(ViPipe);

	g_pastSc030iot[ViPipe]->bInit = CVI_TRUE;
}

void sc030iot_exit(VI_PIPE ViPipe)
{
	sc030iot_i2c_exit(ViPipe);
}

static void sc030iot_linear_480p30_init(VI_PIPE ViPipe)
{
	sc030iot_write_register(ViPipe, 0xf0, 0x30);
	sc030iot_write_register(ViPipe, 0x01, 0x00);
	sc030iot_write_register(ViPipe, 0x02, 0x00);
	sc030iot_write_register(ViPipe, 0x22, 0x00);
	sc030iot_write_register(ViPipe, 0x19, 0x00);
	sc030iot_write_register(ViPipe, 0x3f, 0x82);
	sc030iot_write_register(ViPipe, 0x30, 0x02);
	sc030iot_write_register(ViPipe, 0x31, 0x08);
	sc030iot_write_register(ViPipe, 0x37, 0x00);
	sc030iot_write_register(ViPipe, 0xf0, 0x01);
	sc030iot_write_register(ViPipe, 0x76, 0x81);
	sc030iot_write_register(ViPipe, 0x77, 0x8c);
	sc030iot_write_register(ViPipe, 0x78, 0xe1);
	sc030iot_write_register(ViPipe, 0x79, 0xc1);
	sc030iot_write_register(ViPipe, 0xf4, 0x0a);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x37, 0x79);
	sc030iot_write_register(ViPipe, 0x31, 0x82);
	sc030iot_write_register(ViPipe, 0x3e, 0x60);
	sc030iot_write_register(ViPipe, 0x30, 0xf0);
	sc030iot_write_register(ViPipe, 0x33, 0x33);
	sc030iot_write_register(ViPipe, 0xf0, 0x32);
	sc030iot_write_register(ViPipe, 0x48, 0x02);
	sc030iot_write_register(ViPipe, 0xf0, 0x33);
	sc030iot_write_register(ViPipe, 0x02, 0x12);
	sc030iot_write_register(ViPipe, 0x7c, 0x02);
	sc030iot_write_register(ViPipe, 0x7d, 0x0e);
	sc030iot_write_register(ViPipe, 0xa2, 0x04);
	sc030iot_write_register(ViPipe, 0x5e, 0x06);
	sc030iot_write_register(ViPipe, 0x5f, 0x0a);
	sc030iot_write_register(ViPipe, 0x0b, 0x58);
	sc030iot_write_register(ViPipe, 0x06, 0x38);
	sc030iot_write_register(ViPipe, 0xf0, 0x32);
	sc030iot_write_register(ViPipe, 0x48, 0x02);
	sc030iot_write_register(ViPipe, 0xf0, 0x39);
	sc030iot_write_register(ViPipe, 0x02, 0x70);
	sc030iot_write_register(ViPipe, 0xf0, 0x45);
	sc030iot_write_register(ViPipe, 0x09, 0x1c);
	sc030iot_write_register(ViPipe, 0xf0, 0x37);
	sc030iot_write_register(ViPipe, 0x22, 0x0d);
	sc030iot_write_register(ViPipe, 0xf0, 0x33);
	sc030iot_write_register(ViPipe, 0x33, 0x00);
	sc030iot_write_register(ViPipe, 0x34, 0x00);
	sc030iot_write_register(ViPipe, 0xb1, 0x00);
	sc030iot_write_register(ViPipe, 0x0b, 0x54);
	sc030iot_write_register(ViPipe, 0xb2, 0x78);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x11, 0x80);
	sc030iot_write_register(ViPipe, 0xf0, 0x30);
	sc030iot_write_register(ViPipe, 0x38, 0x44);
	sc030iot_write_register(ViPipe, 0xf0, 0x33);
	sc030iot_write_register(ViPipe, 0xb3, 0x51);
	sc030iot_write_register(ViPipe, 0x01, 0x10);
	sc030iot_write_register(ViPipe, 0x0b, 0x6c);
	sc030iot_write_register(ViPipe, 0x06, 0x24);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x31, 0x82);
	sc030iot_write_register(ViPipe, 0x3e, 0xe0);
	sc030iot_write_register(ViPipe, 0x30, 0xf0);
	sc030iot_write_register(ViPipe, 0x33, 0x33);
	sc030iot_write_register(ViPipe, 0xf0, 0x34);
	sc030iot_write_register(ViPipe, 0x9f, 0x02);
	sc030iot_write_register(ViPipe, 0xa6, 0x40);
	sc030iot_write_register(ViPipe, 0xa7, 0x47);
	sc030iot_write_register(ViPipe, 0xe8, 0x5f);
	sc030iot_write_register(ViPipe, 0xa8, 0x51);
	sc030iot_write_register(ViPipe, 0xa9, 0x44);
	sc030iot_write_register(ViPipe, 0xe9, 0x36);
	sc030iot_write_register(ViPipe, 0xf0, 0x33);
	sc030iot_write_register(ViPipe, 0xb3, 0x51);
	sc030iot_write_register(ViPipe, 0x64, 0x17);
	sc030iot_write_register(ViPipe, 0x90, 0x01);
	sc030iot_write_register(ViPipe, 0x91, 0x03);
	sc030iot_write_register(ViPipe, 0x92, 0x07);
	sc030iot_write_register(ViPipe, 0x01, 0x10);
	sc030iot_write_register(ViPipe, 0x93, 0x10);
	sc030iot_write_register(ViPipe, 0x94, 0x10);
	sc030iot_write_register(ViPipe, 0x95, 0x10);
	sc030iot_write_register(ViPipe, 0x96, 0x01);
	sc030iot_write_register(ViPipe, 0x97, 0x07);
	sc030iot_write_register(ViPipe, 0x98, 0x1f);
	sc030iot_write_register(ViPipe, 0x99, 0x10);
	sc030iot_write_register(ViPipe, 0x9a, 0x20);
	sc030iot_write_register(ViPipe, 0x9b, 0x28);
	sc030iot_write_register(ViPipe, 0x9c, 0x28);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x70, 0x56);
	sc030iot_write_register(ViPipe, 0x7c, 0x43);
	sc030iot_write_register(ViPipe, 0x7d, 0x47);
	sc030iot_write_register(ViPipe, 0x74, 0x00);
	sc030iot_write_register(ViPipe, 0x75, 0x00);
	sc030iot_write_register(ViPipe, 0x76, 0x00);
	sc030iot_write_register(ViPipe, 0xb6, 0x40);
	sc030iot_write_register(ViPipe, 0xb7, 0x41);
	sc030iot_write_register(ViPipe, 0xb8, 0x43);
	sc030iot_write_register(ViPipe, 0xb9, 0x47);
	sc030iot_write_register(ViPipe, 0xba, 0x4f);
	sc030iot_write_register(ViPipe, 0xb0, 0x8b);
	sc030iot_write_register(ViPipe, 0xb1, 0x8b);
	sc030iot_write_register(ViPipe, 0xb2, 0x8b);
	sc030iot_write_register(ViPipe, 0xb3, 0x9b);
	sc030iot_write_register(ViPipe, 0xb4, 0xb8);
	sc030iot_write_register(ViPipe, 0xb5, 0xf0);
	sc030iot_write_register(ViPipe, 0x7e, 0x41);
	sc030iot_write_register(ViPipe, 0x7f, 0x47);
	sc030iot_write_register(ViPipe, 0x77, 0x80);
	sc030iot_write_register(ViPipe, 0x78, 0x84);
	sc030iot_write_register(ViPipe, 0x79, 0x8a);
	sc030iot_write_register(ViPipe, 0xa0, 0x47);
	sc030iot_write_register(ViPipe, 0xa1, 0x5f);
	sc030iot_write_register(ViPipe, 0x96, 0x22);
	sc030iot_write_register(ViPipe, 0x97, 0x22);
	sc030iot_write_register(ViPipe, 0x98, 0x33);
	sc030iot_write_register(ViPipe, 0xf0, 0x3f);
	sc030iot_write_register(ViPipe, 0x03, 0x97);
	sc030iot_write_register(ViPipe, 0xf0, 0x00);
	sc030iot_write_register(ViPipe, 0xf0, 0x01);
	sc030iot_write_register(ViPipe, 0x73, 0x00);
	sc030iot_write_register(ViPipe, 0x74, 0xe0);
	sc030iot_write_register(ViPipe, 0x70, 0x00);
	sc030iot_write_register(ViPipe, 0x71, 0x80);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x37, 0x74);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x11, 0x80);
	sc030iot_write_register(ViPipe, 0xf0, 0x01);
	sc030iot_write_register(ViPipe, 0x79, 0xc1);
	sc030iot_write_register(ViPipe, 0xf0, 0x37);
	sc030iot_write_register(ViPipe, 0x24, 0x21);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x41, 0x00);
	sc030iot_write_register(ViPipe, 0xe9, 0x20);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x32, 0x44);
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0xa0, 0x47);
	sc030iot_write_register(ViPipe, 0xa1, 0x5f);
	sc030iot_write_register(ViPipe, 0x96, 0x22);
	sc030iot_write_register(ViPipe, 0x97, 0x22);
	sc030iot_write_register(ViPipe, 0x98, 0x22);
	sc030iot_write_register(ViPipe, 0xf0, 0x00);
	sc030iot_write_register(ViPipe, 0x72, 0x38);
	sc030iot_write_register(ViPipe, 0x7a, 0x80);
	sc030iot_write_register(ViPipe, 0x85, 0x18);
	sc030iot_write_register(ViPipe, 0x9b, 0x35);
	sc030iot_write_register(ViPipe, 0x9e, 0x20);
	sc030iot_write_register(ViPipe, 0xd0, 0x66);
	sc030iot_write_register(ViPipe, 0xd1, 0x34);
	sc030iot_write_register(ViPipe, 0Xd3, 0x44);
	sc030iot_write_register(ViPipe, 0xd6, 0x44);
	sc030iot_write_register(ViPipe, 0xb0, 0x41);
	sc030iot_write_register(ViPipe, 0xb2, 0x48);
	sc030iot_write_register(ViPipe, 0xb3, 0xf4);
	sc030iot_write_register(ViPipe, 0xb4, 0x0b);
	sc030iot_write_register(ViPipe, 0xb5, 0x78);
	sc030iot_write_register(ViPipe, 0xba, 0xff);
	sc030iot_write_register(ViPipe, 0xbb, 0xc0);
	sc030iot_write_register(ViPipe, 0xbc, 0x90);
	sc030iot_write_register(ViPipe, 0xbd, 0x3a);
	sc030iot_write_register(ViPipe, 0xc1, 0x67);
	sc030iot_write_register(ViPipe, 0xf0, 0x01);
	sc030iot_write_register(ViPipe, 0x20, 0x11);
	sc030iot_write_register(ViPipe, 0x23, 0x90);
	sc030iot_write_register(ViPipe, 0x24, 0x15);
	sc030iot_write_register(ViPipe, 0x25, 0x87);
	sc030iot_write_register(ViPipe, 0xbc, 0x9f);
	sc030iot_write_register(ViPipe, 0xbd, 0x3a);
	sc030iot_write_register(ViPipe, 0x48, 0xe6);
	sc030iot_write_register(ViPipe, 0x49, 0xc0);
	sc030iot_write_register(ViPipe, 0x4a, 0xd0);
	sc030iot_write_register(ViPipe, 0x4b, 0x48);
	sc030iot_write_register(ViPipe, 0xf0, 0x00);
	sc030iot_write_register(ViPipe, 0x71, 0x92);
	sc030iot_write_register(ViPipe, 0x7c, 0x03);
	sc030iot_write_register(ViPipe, 0x84, 0xb4);
	sc030iot_write_register(ViPipe, 0xf0, 0x33);
	sc030iot_write_register(ViPipe, 0x14, 0x95);

	// cvbs_off
	sc030iot_write_register(ViPipe, 0xf0, 0x36);
	sc030iot_write_register(ViPipe, 0x11, 0x80);
	sc030iot_write_register(ViPipe, 0xf0, 0x01);
	sc030iot_write_register(ViPipe, 0x79, 0xc1);

	printf("ViPipe:%d,===SC030IOT 480P 30fps 8bit LINEAR Init OK!===\n", ViPipe);
}