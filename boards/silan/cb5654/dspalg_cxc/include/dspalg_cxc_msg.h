/*
 * silan_cxc.h
 *
 * Modify Date: 2016-3-21 10:52
 * MaintainedBy: qinling <qinling@silan.com.cn>
 */

#ifndef __SILAN_CXC_H__
#define __SILAN_CXC_H__
#include <stdint.h>
#include <soc.h>

/* 
 * Hardware
 * ------------
 *  CXC has 32 interrupt vectors, 32 mutex registers, and 64 mailbox registers.
 *
 * 
 * Driver
 * ------------
 *  cxc core id is same with cxc interrupt id,
 *  each message has 8 parameters, command, argc, and argv[6];
 */
#define SILAN_CXC_MUTEX_BASE	(SILAN_CXC_BASE + 0x0)
#define SILAN_CXC_MAILBOX_BASE	(SILAN_CXC_BASE + 0x400)
#define SILAN_CXC_MAILBOX_BASE	(SILAN_CXC_BASE + 0x400)
#define SILAN_CXC_RAW_INT_STATUS (SILAN_CXC_BASE + 0x800)
#define SILAN_CXC_RAW_INT_SET	(SILAN_CXC_BASE + 0x804)
#define SILAN_CXC_RAW_INT_CLR	(SILAN_CXC_BASE + 0x808)
#define SILAN_CXC_RISC_INT_MASK	(SILAN_CXC_BASE + 0x80C)
#define SILAN_CXC_DSP_INT_MASK	(SILAN_CXC_BASE + 0x810)
#define SILAN_CXC_MCU_INT_MASK	(SILAN_CXC_BASE + 0x814)

#define SILAN_CXC_INT_MASK SILAN_CXC_RISC_INT_MASK

#define SILAN_CXC_REG_DSP_WKUP	(SILAN_CXC_BASE + 0x444)
#define SILAN_CXC_REG_DSP_VAD	(SILAN_CXC_BASE + 0x411)
#define SILAN_CXC_REG_M0_WKUP	(SILAN_CXC_BASE + 0x404)
#define SILAN_CXC_REG_DSP_CHECK	(SILAN_CXC_BASE + 0x454)
#define SILAN_CXC_REG_DSP_MODE	(SILAN_CXC_BASE + 0x468)

typedef enum {
	SILAN_CXC_MUTEX_CHANNEL0 = 8,
	SILAN_CXC_MUTEX_CHANNEL1 = 9,
	SILAN_CXC_MUTEX_CHANNEL2 = 12,
	SILAN_CXC_MUTEX_CHANNEL3 = 13,
	SILAN_CXC_MUTEX_PM_M0_CK = 16,
	SILAN_CXC_MUTEX_LMT = 32,
}silan_cxc_mutex_e;
#define SILAN_CXC_MUTEX_CK_2_DSP_CHANNEL SILAN_CXC_MUTEX_CHANNEL0
#define SILAN_CXC_MUTEX_DSP_2_CK_CHANNEL SILAN_CXC_MUTEX_CHANNEL1
#define SILAN_CXC_MUTEX_CK_2_M0_CHANNEL SILAN_CXC_MUTEX_CHANNEL2
#define SILAN_CXC_MUTEX_M0_2_CK_CHANNEL SILAN_CXC_MUTEX_CHANNEL3
#if SILAN_CXC_MUTEX_LMT >= 32
#error Too many SILAN_CXC_MUTEX!!!
#endif

typedef enum {
	SILAN_CXC_MAILBOX_CHANNEL2 = 0,
	SILAN_CXC_MAILBOX_FLASH_START= 1,
	SILAN_CXC_MAILBOX_FLASH_STOP= 2,
	SILAN_CXC_MAILBOX_CHANNEL3 = 4,
	SILAN_CXC_MAILBOX_CHANNEL0 = 8,
	SILAN_CXC_MAILBOX_CHANNEL1 = 12,
	SILAN_CXC_MAILBOX_LMT = 32,
}silan_cxc_mailbox_e;
#define SILAN_CXC_MAILBOX_CK_2_DSP_CHANNEL SILAN_CXC_MAILBOX_CHANNEL0
#define SILAN_CXC_MAILBOX_DSP_2_CK_CHANNEL SILAN_CXC_MAILBOX_CHANNEL1
#define SILAN_CXC_MAILBOX_CK_2_M0_CHANNEL SILAN_CXC_MAILBOX_CHANNEL2
#define SILAN_CXC_MAILBOX_M0_2_CK_CHANNEL SILAN_CXC_MAILBOX_CHANNEL3
#define SILAN_CXC_MAILBOX_NUM_LIMIT 4
#if SILAN_CXC_MAILBOX_LMT >= 32
#error Too many SILAN_CXC_MAILBOX!!!
#endif

typedef enum {
	SILAN_CXC_INT_DSPINNER_DSP2RISC = 4,
	SILAN_CXC_INT_CHANNEL0 = 8,
	SILAN_CXC_INT_CHANNEL1 = 9,
	SILAN_CXC_INT_CHANNEL0_ACK = 10,
	SILAN_CXC_INT_CHANNEL1_ACK = 11,
	SILAN_CXC_INT_CHANNEL2 = 12,
	SILAN_CXC_INT_CHANNEL3 = 13,
	SILAN_CXC_INT_CHANNEL2_ACK = 14,
	SILAN_CXC_INT_CHANNEL3_ACK = 15,
	SILAN_CXC_INT_M0_LPM_QUIT_GPIO = 20,
	SILAN_CXC_INT_M0_LPM_QUIT_TIMER = 21,
	SILAN_CXC_INT_DSP_WAKEUP = 24,
	SILAN_CXC_INT_LMT = 32,
}silan_cxc_int_e;
#define SILAN_CXC_INT_CK_2_DSP_CHANNEL SILAN_CXC_INT_CHANNEL0
#define SILAN_CXC_INT_DSP_2_CK_CHANNEL SILAN_CXC_INT_CHANNEL1
#define SILAN_CXC_INT_CK_2_DSP_CHANNEL_ACK SILAN_CXC_INT_CHANNEL0_ACK
#define SILAN_CXC_INT_DSP_2_CK_CHANNEL_ACK SILAN_CXC_INT_CHANNEL1_ACK
#define SILAN_CXC_INT_CK_2_M0_CHANNEL SILAN_CXC_INT_CHANNEL2
#define SILAN_CXC_INT_M0_2_CK_CHANNEL SILAN_CXC_INT_CHANNEL3
#define SILAN_CXC_INT_CK_2_M0_CHANNEL_ACK SILAN_CXC_INT_CHANNEL2_ACK
#define SILAN_CXC_INT_M0_2_CK_CHANNEL_ACK SILAN_CXC_INT_CHANNEL3_ACK
#if SILAN_CXC_INT_LMT >= 32
#error Too many SILAN_CXC_INT!!!
#endif

typedef enum {
	SILAN_CXC_CMD_LPM_NULL = -1,
	SILAN_CXC_CMD_LPM_SDRAM_PD,
	SILAN_CXC_CMD_LPM_CK_PD,
	SILAN_CXC_CMD_LPM_CK_RST,
	SILAN_CXC_CMD_LPM_SPLL_DN,
	SILAN_CXC_CMD_LPM_RCL,
	SILAN_CXC_CMD_LPM_RCH,
	SILAN_CXC_CMD_LPM_MODULE_PD,
	SILAN_CXC_CMD_LPM_MIC_RECORD,
	SILAN_CXC_CMD_LPM_LOG,
	SILAN_CXC_CMD_LPM_LIMIT = 9,
}silan_cxc_lpm_cmd_e;

typedef enum {
	SILAN_CXC_CMD_PARAM_NULL = -1,
	SILAN_CXC_CMD_PARAM_TIMER_LIMIT,
	SILAN_CXC_CMD_PARAM_VAD_THRESHOLD,
	SILAN_CXC_CMD_PARAM_LIMIT = 8,
}silan_cxc_param_cmd_e;

typedef enum {
	SILAN_CXC_CMD_FREQ_NULL = -1,
	SILAN_CXC_CMD_FREQ_SYSPLL = 0,
	SILAN_CXC_CMD_FREQ_BUSDIV = 2,
	SILAN_CXC_CMD_FREQ_DSPDIV,
	SILAN_CXC_CMD_FREQ_SFDIV,
	SILAN_CXC_CMD_FREQ_LIMIT,
}silan_cxc_freq_cmd_e;

typedef enum {
	SILAN_CXC_CMD_NULL = -1,
	SILAN_CXC_CMD_LPM_ENTER,
	SILAN_CXC_CMD_PARAM_SET,
	SILAN_CXC_CMD_LPM_QUIT,
	SILAN_CXC_CMD_SUPER_LPM_ENTER,
	SILAN_CXC_CMD_VAD_DISABLE,
	SILAN_CXC_CMD_FREQ_SET,
	SILAN_CXC_CMD_FREQ_QUIT,
	SILAN_CXC_CMD_FLASH_SOLO,
	SILAN_CXC_CMD_LIMIT,
}silan_cxc_cmd_e;

typedef enum {
	SILAN_CXC_DSP_CMD_NULL = -1,
	SILAN_CXC_DSP_CMD_RESP = 1,
	SILAN_CXC_DSP_CMD_MESG,
	SILAN_CXC_DSP_CMD_STAT,
	SILAN_CXC_DSP_CMD_ERR,
}silan_cxc_dsp_cmd_e;

typedef enum {
	SILAN_CXC_DSP_WKTYPE_DIANXIN = 1,
	SILAN_CXC_DSP_WKTYPE_ROBOT = 2,
}silan_cxc_dsp_wktype_e;

typedef enum {
	SILAN_CXC_DSP_WK_CMD_NULL = -1,
	SILAN_CXC_DSP_WK_CMD_LEFT = 1,
	SILAN_CXC_DSP_WK_CMD_RIGHT,
	SILAN_CXC_DSP_WK_CMD_HEAD,
	SILAN_CXC_DSP_WK_CMD_BACK,
	SILAN_CXC_DSP_WK_CMD_STOP,
	SILAN_CXC_DSP_WK_CMD_DANCE,
}silan_cxc_dsp_wk_cmd_e;

typedef int (*cxc_message_cb_t)(unsigned char*data, int num);

/*
 * m0 wakeup source:
 * gpio(4)		//8 bits * 4
 * iic(2)		//4 bits * 2
 * uart(4)		//4 bits * 4
 * spi(2)		//4 bits * 2
 * mic/vad(1)	//1 bits * 1
 */

#define CXC_ARGV_LPM_WK_SRC		0
#define CXC_ARGV_LPM_WK_GPIO_0	1
#define CXC_ARGV_LPM_WK_GPIO_1	2
#define CXC_ARGV_LPM_WK_GPIO_2	3
#define CXC_ARGV_LPM_WK_GPIO_3	4
#define CXC_ARGV_LPM_WK_IIC		5
#define CXC_ARGV_LPM_WK_UART	6
#define CXC_ARGV_LPM_WK_SPI		7
#define CXC_ARGV_LPM_WK_MIC		8
#define CXC_ARGV_LPM_WK_GPIO_TIMEOUT	9

#define CXC_WKSRC_GPIO_OFFSET	0
#define CXC_WKSRC_IIC_OFFSET	1
#define CXC_WKSRC_UART_OFFSET	2
#define CXC_WKSRC_SPI_OFFSET	3
#define CXC_WKSRC_MIC_OFFSET	4
#define CXC_WKSRC_TIMER_OFFSET	5

#define CXC_WKSRC_IIC_0		0
#define CXC_WKSRC_IIC_1		1

#define CXC_WKSRC_UART_0	0
#define CXC_WKSRC_UART_1	1
#define CXC_WKSRC_UART_2	2
#define CXC_WKSRC_UART_3	3

#define CXC_WKSRC_SPI_0		0
#define CXC_WKSRC_SPI_1		1

#define CXC_WKSRC_MIC		0

#define CXC_ARGC_PARAM_TIMER_LIMIT 4
#define CXC_ARGC_PARAM_VAD_THRESHOLD 2

#define FLASH_SOLO_CNT (10)
typedef struct {
	uint8_t main_cmd;
	uint8_t	mode;
	uint8_t	sub_cmd;
	uint8_t argc;
	uint8_t	argv[12];
} __attribute__((aligned(1))) silan_cxc_mesg_t;

typedef void (*cxc_hdl_t)(silan_cxc_mesg_t *mesg);

#define SILAN_CXC_MUTEX_RESET         0
#define SILAN_CXC_MUTEX_LOG           1
#define SILAN_CXC_MUTEX_LMT          32
int silan_cxc_mutex_lock(int mutex);
int silan_cxc_mutex_unlock(int mutex);
int silan_cxc_mutex_test(int mutex);

int silan_cxc_memssage_receive_register(cxc_message_cb_t cb, int idx);
int silan_cxc_message_m0_2_ck(unsigned char* context, int num);
int silan_cxc_message_ck_2_dsp(unsigned char* context, int num);
int silan_cxc_message_ck_2_m0(unsigned char* context, int num);
int silan_cxc_message_dsp_2_ck(unsigned char* context, int num);
int silan_cxc_message_ck_2_dsp_custom(unsigned char* context, int num);
int silan_cxc_message_ck_2_dsp_test(unsigned char* context, int num);
int silan_cxc_init(cxc_hdl_t cxc_msg_cb);

#endif  //__SILAN_CXC_H__
