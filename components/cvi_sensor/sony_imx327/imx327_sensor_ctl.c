#include "cvi_sns_ctrl.h"
#include "cvi_comm_video.h"
#include "cvi_sns_ctrl.h"
#include "imx327_cmos_ex.h"
#include "drv/common.h"
#include "sensor_i2c.h"

static void imx327_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
static void imx327_linear_1080p30_init(VI_PIPE ViPipe);

const CVI_U8 imx327_i2c_addr = 0x1A;
const CVI_U32 imx327_addr_byte = 2;
const CVI_U32 imx327_data_byte = 1;

int imx327_i2c_init(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunImx327_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_init(i2c_id);
}

int imx327_i2c_exit(VI_PIPE ViPipe)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunImx327_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_exit(i2c_id);
}

int imx327_read_register(VI_PIPE ViPipe, int addr)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunImx327_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_read(i2c_id, imx327_i2c_addr, (CVI_U32)addr, imx327_addr_byte, imx327_data_byte);
}

int imx327_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 i2c_id = (CVI_U8)g_aunImx327_BusInfo[ViPipe].s8I2cDev;

	return sensor_i2c_write(i2c_id, imx327_i2c_addr, (CVI_U32)addr, imx327_addr_byte,
				(CVI_U32)data, imx327_data_byte);
}

static void delay_ms(int ms)
{
	udelay(ms * 1000);
}

void imx327_standby(VI_PIPE ViPipe)
{
	imx327_write_register(ViPipe, 0x3000, 0x01); /* STANDBY */
	imx327_write_register(ViPipe, 0x3002, 0x01); /* XTMSTA */
}

void imx327_restart(VI_PIPE ViPipe)
{
	imx327_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx327_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
	imx327_write_register(ViPipe, 0x304b, 0x0a);
}

void imx327_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastImx327[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		imx327_write_register(ViPipe,
				g_pastImx327[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastImx327[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define IMX327_CHIP_ID_ADDR	0x31dc
#define IMX327_CHIP_ID		6
#define IMX327_CHIP_ID_MASK	0x6

void imx327_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 val = imx327_read_register(ViPipe, 0x3007) & ~0x3;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		val |= 0x2;
		break;
	case ISP_SNS_FLIP:
		val |= 0x1;
		break;
	case ISP_SNS_MIRROR_FLIP:
		val |= 0x3;
		break;
	default:
		return;
	}

	imx327_write_register(ViPipe, 0x3007, val);
}

int imx327_probe(VI_PIPE ViPipe)
{
	int nVal;

	delay_ms(1); //waitting i2c stable
	if (imx327_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = imx327_read_register(ViPipe, IMX327_CHIP_ID_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((nVal & IMX327_CHIP_ID_MASK) != IMX327_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void imx327_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastImx327[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastImx327[ViPipe]->u8ImgMode;

	delay_ms(1); //waitting i2c stable
	if (imx327_i2c_init(ViPipe) != CVI_SUCCESS) {
		return ;
	}

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == IMX327_MODE_1080P30_WDR) {
			imx327_wdr_1080p30_2to1_init(ViPipe);
		}
	} else {
		imx327_linear_1080p30_init(ViPipe);
	}
	g_pastImx327[ViPipe]->bInit = CVI_TRUE;
}

void imx327_exit(VI_PIPE ViPipe)
{
	imx327_i2c_exit(ViPipe);
}

/* 1080P30 and 1080P25 */
static void imx327_linear_1080p30_init(VI_PIPE ViPipe)
{
	imx327_write_register(ViPipe, 0x3003, 0x01); /* SW RESET */
	delay_ms(4);
	imx327_write_register(ViPipe, 0x3000, 0x01); /* STANDBY */
	imx327_write_register(ViPipe, 0x3002, 0x01); /* XTMSTA */
	imx327_write_register(ViPipe, 0x3005, 0x01); /* ADBIT 10bit*/
	imx327_write_register(ViPipe, 0x3007, 0x00); /* VREVERS, ...*/
	imx327_write_register(ViPipe, 0x3009, 0x02); /**/
	imx327_write_register(ViPipe, 0x300A, 0xF0); /* BLKLEVEL*/
	imx327_write_register(ViPipe, 0x3010, 0x21);
	imx327_write_register(ViPipe, 0x3011, 0x02);
	imx327_write_register(ViPipe, 0x3014, 0x16); /* GAIN, 0x2A=>12.6dB TBD*/
	imx327_write_register(ViPipe, 0x3018, 0x65); /* VMAX[7:0]*/
	imx327_write_register(ViPipe, 0x3019, 0x04); /* VMAX[15:8]*/
	imx327_write_register(ViPipe, 0x301A, 0x00); /* VMAX[17:16]:=:0x301A[1:0]*/
	imx327_write_register(ViPipe, 0x301C, 0x30); /* HMAX[7:0], TBD*/
	imx327_write_register(ViPipe, 0x301D, 0x11); /* HMAX[15:8]*/
	imx327_write_register(ViPipe, 0x3020, 0x8C); /* SHS[7:0], TBD*/
	imx327_write_register(ViPipe, 0x3021, 0x01); /* SHS[15:8]*/
	imx327_write_register(ViPipe, 0x3022, 0x00); /* SHS[19:16]*/
	imx327_write_register(ViPipe, 0x3046, 0x01);
	imx327_write_register(ViPipe, 0x304B, 0x0A);
	imx327_write_register(ViPipe, 0x305C, 0x18); /* INCKSEL1*/
	imx327_write_register(ViPipe, 0x305D, 0x03); /* INCKSEL2*/
	imx327_write_register(ViPipe, 0x305E, 0x20); /* INCKSEL3*/
	imx327_write_register(ViPipe, 0x305F, 0x01); /* INCKSEL4*/
	imx327_write_register(ViPipe, 0x309E, 0x4A);
	imx327_write_register(ViPipe, 0x309F, 0x4A);
	imx327_write_register(ViPipe, 0x30D2, 0x19);
	imx327_write_register(ViPipe, 0x30D7, 0x03);
	imx327_write_register(ViPipe, 0x3129, 0x00);
	imx327_write_register(ViPipe, 0x313B, 0x61);
	imx327_write_register(ViPipe, 0x315E, 0x1A);
	imx327_write_register(ViPipe, 0x3164, 0x1A);
	imx327_write_register(ViPipe, 0x317C, 0x00);
	imx327_write_register(ViPipe, 0x31EC, 0x0E);
	imx327_write_register(ViPipe, 0x3405, 0x20); /* Repetition*/
	imx327_write_register(ViPipe, 0x3407, 0x03); /* physical_lane_nl*/
	imx327_write_register(ViPipe, 0x3414, 0x0A); /* opb_size_v*/
	imx327_write_register(ViPipe, 0x3418, 0x49); /* y_out_size*/
	imx327_write_register(ViPipe, 0x3419, 0x04); /* y_out_size*/
	imx327_write_register(ViPipe, 0x3441, 0x0C); /* csi_dt_fmt*/
	imx327_write_register(ViPipe, 0x3442, 0x0C); /* csi_dt_fmt*/
	imx327_write_register(ViPipe, 0x3443, 0x03); /* csi_lane_mode*/
	imx327_write_register(ViPipe, 0x3444, 0x20); /* extck_freq*/
	imx327_write_register(ViPipe, 0x3445, 0x25); /* extck_freq*/
	imx327_write_register(ViPipe, 0x3446, 0x47); /* tclkpost*/
	imx327_write_register(ViPipe, 0x3447, 0x00); /* tclkpost*/
	imx327_write_register(ViPipe, 0x3448, 0x80); /* thszero*/
	imx327_write_register(ViPipe, 0x3449, 0x00); /* thszero*/
	imx327_write_register(ViPipe, 0x344A, 0x17); /* thsprepare*/
	imx327_write_register(ViPipe, 0x344B, 0x00); /* thsprepare*/
	imx327_write_register(ViPipe, 0x344C, 0x0F); /* tclktrail*/
	imx327_write_register(ViPipe, 0x344D, 0x00); /* tclktrail*/
	imx327_write_register(ViPipe, 0x344E, 0x80); /* thstrail*/
	imx327_write_register(ViPipe, 0x344F, 0x00); /* thstrail*/
	imx327_write_register(ViPipe, 0x3450, 0x47); /* tclkzero*/
	imx327_write_register(ViPipe, 0x3451, 0x00); /* tclkzero*/
	imx327_write_register(ViPipe, 0x3452, 0x0F); /* tclkprepare*/
	imx327_write_register(ViPipe, 0x3453, 0x00); /* tckkprepare*/
	imx327_write_register(ViPipe, 0x3454, 0x0F); /* tlpx*/
	imx327_write_register(ViPipe, 0x3455, 0x00); /* tlpx*/
	imx327_write_register(ViPipe, 0x3472, 0x9C); /* x_out_size*/
	imx327_write_register(ViPipe, 0x3473, 0x07); /* x_out_size*/
	imx327_write_register(ViPipe, 0x3480, 0x49); /* incksel7*/

	imx327_default_reg_init(ViPipe);

	imx327_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx327_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
	imx327_write_register(ViPipe, 0x304b, 0x0a);

	printf("ViPipe:%d,===IMX327 1080P 30fps 12bit LINE Init OK!===\n", ViPipe);
}

static void imx327_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
	imx327_write_register(ViPipe, 0x3003, 0x01); /* SW RESET */
	delay_ms(4);
	imx327_write_register(ViPipe, 0x3000, 0x01); /* STANDBY */
	imx327_write_register(ViPipe, 0x3002, 0x01); /* XTMSTA */
	imx327_write_register(ViPipe, 0x3005, 0x01); /* ADBIT*/
	imx327_write_register(ViPipe, 0x3007, 0x00); /* VREVERS, ...*/
	imx327_write_register(ViPipe, 0x3009, 0x01); /**/
	imx327_write_register(ViPipe, 0x300A, 0xF0); /* BLKLEVEL*/
	imx327_write_register(ViPipe, 0x300C, 0x11); /* WDMODE [0] 0:Normal, 1:DOL, WDSEL [5:4] 1:DOL 2 frames*/
	imx327_write_register(ViPipe, 0x3011, 0x02);
	imx327_write_register(ViPipe, 0x3014, 0x16); /* GAIN, 0x2A=>12.6dB TBD*/
	imx327_write_register(ViPipe, 0x3018, 0x65); /* VMAX[7:0]*/
	imx327_write_register(ViPipe, 0x3019, 0x04); /* VMAX[15:8]*/
	imx327_write_register(ViPipe, 0x301A, 0x00); /* VMAX[17:16]:=:0x301A[1:0]*/
	imx327_write_register(ViPipe, 0x301C, 0x98); /* HMAX[7:0], TBD*/
	imx327_write_register(ViPipe, 0x301D, 0x08); /* HMAX[15:8]*/
	imx327_write_register(ViPipe, 0x3020, 0x02); /* SHS[7:0], TBD*/
	imx327_write_register(ViPipe, 0x3021, 0x00); /* SHS[15:8]*/
	imx327_write_register(ViPipe, 0x3022, 0x00); /* SHS[19:16]*/
	imx327_write_register(ViPipe, 0x3024, 0xC9); /* SHS2[7:0], TBD*/
	imx327_write_register(ViPipe, 0x3025, 0x07); /* SHS2[15:8]*/
	imx327_write_register(ViPipe, 0x3026, 0x00); /* SHS2[19:16]*/
	imx327_write_register(ViPipe, 0x3030, 0x0B); /* RHS1[7:0], TBD*/
	imx327_write_register(ViPipe, 0x3031, 0x00); /* RHS1[15:8]*/
	imx327_write_register(ViPipe, 0x3032, 0x00); /* RHS1[19:16]*/
	imx327_write_register(ViPipe, 0x3045, 0x05); /* DOLSCDEN [0] 1: pattern1 0: pattern2*/
	imx327_write_register(ViPipe, 0x3046, 0x01);
	imx327_write_register(ViPipe, 0x304B, 0x0A);
	imx327_write_register(ViPipe, 0x305C, 0x18); /* INCKSEL1*/
	imx327_write_register(ViPipe, 0x305D, 0x03); /* INCKSEL2*/
	imx327_write_register(ViPipe, 0x305E, 0x20); /* INCKSEL3*/
	imx327_write_register(ViPipe, 0x305F, 0x01); /* INCKSEL4*/
	imx327_write_register(ViPipe, 0x309E, 0x4A);
	imx327_write_register(ViPipe, 0x309F, 0x4A);
	imx327_write_register(ViPipe, 0x30D2, 0x19);
	imx327_write_register(ViPipe, 0x30D7, 0x03);
	imx327_write_register(ViPipe, 0x3106, 0x11); /*DOLHBFIXEN[7] 0: pattern1 1: pattern2*/
	imx327_write_register(ViPipe, 0x3129, 0x00);
	imx327_write_register(ViPipe, 0x313B, 0x61);
	imx327_write_register(ViPipe, 0x315E, 0x1A);
	imx327_write_register(ViPipe, 0x3164, 0x1A);
	imx327_write_register(ViPipe, 0x317C, 0x00);
	imx327_write_register(ViPipe, 0x31EC, 0x0E);
	imx327_write_register(ViPipe, 0x3405, 0x10); /* Repetition*/
	imx327_write_register(ViPipe, 0x3407, 0x03); /* physical_lane_nl*/
	imx327_write_register(ViPipe, 0x3414, 0x0A); /* opb_size_v*/
	imx327_write_register(ViPipe, 0x3415, 0x00); /* NULL0_SIZE_V, set to 00h when DOL*/
	imx327_write_register(ViPipe, 0x3418, 0xB4); /* y_out_size*/
	imx327_write_register(ViPipe, 0x3419, 0x08); /* y_out_size*/
	imx327_write_register(ViPipe, 0x3441, 0x0C); /* csi_dt_fmt*/
	imx327_write_register(ViPipe, 0x3442, 0x0C); /* csi_dt_fmt*/
	imx327_write_register(ViPipe, 0x3443, 0x03); /* csi_lane_mode*/
	imx327_write_register(ViPipe, 0x3444, 0x20); /* extck_freq*/
	imx327_write_register(ViPipe, 0x3445, 0x25); /* extck_freq*/
	imx327_write_register(ViPipe, 0x3446, 0x57); /* tclkpost*/
	imx327_write_register(ViPipe, 0x3447, 0x00); /* tclkpost*/
	imx327_write_register(ViPipe, 0x3448, 0x80); /* thszero*/
	imx327_write_register(ViPipe, 0x3449, 0x00); /* thszero*/
	imx327_write_register(ViPipe, 0x344A, 0x1F); /* thsprepare*/
	imx327_write_register(ViPipe, 0x344B, 0x00); /* thsprepare*/
	imx327_write_register(ViPipe, 0x344C, 0x1F); /* tclktrail*/
	imx327_write_register(ViPipe, 0x344D, 0x00); /* tclktrail*/
	imx327_write_register(ViPipe, 0x344E, 0x80); /* thstrail*/
	imx327_write_register(ViPipe, 0x344F, 0x00); /* thstrail*/
	imx327_write_register(ViPipe, 0x3450, 0x77); /* tclkzero*/
	imx327_write_register(ViPipe, 0x3451, 0x00); /* tclkzero*/
	imx327_write_register(ViPipe, 0x3452, 0x1F); /* tclkprepare*/
	imx327_write_register(ViPipe, 0x3453, 0x00); /* tckkprepare*/
	imx327_write_register(ViPipe, 0x3454, 0x17); /* tlpx*/
	imx327_write_register(ViPipe, 0x3455, 0x00); /* tlpx*/
	imx327_write_register(ViPipe, 0x3472, 0xA0); /* x_out_size*/
	imx327_write_register(ViPipe, 0x3473, 0x07); /* x_out_size*/
	imx327_write_register(ViPipe, 0x347B, 0x23); /**/
	imx327_write_register(ViPipe, 0x3480, 0x49); /* incksel7*/

	imx327_default_reg_init(ViPipe);

	if (g_au16Imx327_GainMode[ViPipe] == SNS_GAIN_MODE_SHARE) {
		imx327_write_register(ViPipe, 0x30F0, 0xF0);
		imx327_write_register(ViPipe, 0x3010, 0x21);
	} else {
		imx327_write_register(ViPipe, 0x30F0, 0x64);
		imx327_write_register(ViPipe, 0x3010, 0x61);
	}

	imx327_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx327_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
	imx327_write_register(ViPipe, 0x304b, 0x0a);

	printf("===Imx327 sensor 1080P30fps 12bit 2to1 WDR(60fps->30fps) init success!=====\n");
}
