#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <silan_irq.h>
#include "silan_syscfg_regs.h"
#include "silan_cxc_regs.h"
#include "dspalg_cxc_msg.h"

#define REG32( addr )          (*(volatile unsigned int  *)(addr))

#define CXC_MESSAGE_ACK 1
static cxc_message_cb_t g_mesg_cb[SILAN_CXC_INT_LMT] = {NULL};

static void up_udelay(int us)
{
	extern void udelay(uint32_t us);
	udelay(us);
}

int silan_cxc_memssage_receive_register(cxc_message_cb_t cb, int idx) {
	g_mesg_cb[idx] = cb;
	REG32(SILAN_CXC_RAW_INT_CLR) |= (1<<idx);
	REG32(SILAN_CXC_INT_MASK) &= ~(1<<idx);
	return 0;
}

int silan_cxc_get_mutex(int idx_mutex)
{

	int ret = 0;
	int count = 1000;
	do {
		ret = REG32(SILAN_CXC_MUTEX_BASE + 4*idx_mutex);
		if(ret) {
			up_udelay(10);
			count --;
		}
		else
			break;
		if (count < 0) {
			printf("%s %d get cxc mutex failed 0x%x\n", __func__, __LINE__, SILAN_CXC_MUTEX_BASE + 4*idx_mutex);
			return -1;
		}
	}while(1);
	//REG32(SILAN_CXC_MUTEX_BASE + 4*idx_mutex) = 1;
	return ret;
}

int  silan_cxc_release_mutex(int idx_mutex)
{
	REG32(SILAN_CXC_MUTEX_BASE + 4*idx_mutex) = 0;
	return 0;
}

int silan_cxc_message_receive(int idx_int, int idx_mutex, int idx_mailbox, int idx_ack)
{
	int i= 0;
	unsigned int data[SILAN_CXC_MAILBOX_NUM_LIMIT];
	//step1: get mutex
	silan_cxc_get_mutex(idx_mutex);

	//step2: get context & process
	memset(data, 0, SILAN_CXC_MAILBOX_NUM_LIMIT*sizeof(unsigned int));
	for (i=0; i<SILAN_CXC_MAILBOX_NUM_LIMIT; i++) {
		data[i] = REG32(SILAN_CXC_MAILBOX_BASE + 4*idx_mailbox + 4*i);
	}

	//step3: release mutex
	silan_cxc_release_mutex(idx_mutex);

	if (g_mesg_cb[idx_int])
		g_mesg_cb[idx_int]((unsigned char *)data, SILAN_CXC_MAILBOX_NUM_LIMIT*sizeof(unsigned int));

	REG32(SILAN_CXC_RAW_INT_CLR) |= 1<<idx_int;

#ifdef CXC_MESSAGE_ACK
	//step3: send ack
	REG32(SILAN_CXC_RAW_INT_SET) |= 1<<idx_ack;
#endif

	return 0;
}

static void silan_cxc_interrupt(int idx)
{

	int status = REG32(SILAN_CXC_RAW_INT_STATUS);
	//printf("cxc status:0x%x\n", status);
	if (status & (1<<SILAN_CXC_INT_DSP_WAKEUP)) {
		__sREG8(SILAN_CXC_REG_DSP_WKUP, 0) = 0;
		silan_cxc_message_receive(SILAN_CXC_INT_DSP_WAKEUP, SILAN_CXC_MUTEX_DSP_2_CK_CHANNEL, SILAN_CXC_MAILBOX_DSP_2_CK_CHANNEL, SILAN_CXC_INT_DSP_2_CK_CHANNEL_ACK);
	}

	if (status & (1<<SILAN_CXC_INT_DSP_2_CK_CHANNEL)) {
		silan_cxc_message_receive(SILAN_CXC_INT_DSP_2_CK_CHANNEL, SILAN_CXC_MUTEX_DSP_2_CK_CHANNEL, SILAN_CXC_MAILBOX_DSP_2_CK_CHANNEL, SILAN_CXC_INT_DSP_2_CK_CHANNEL_ACK);
	}

	if (status & (1<<SILAN_CXC_INT_M0_2_CK_CHANNEL)) {
		silan_cxc_message_receive(SILAN_CXC_INT_M0_2_CK_CHANNEL, SILAN_CXC_MUTEX_M0_2_CK_CHANNEL, SILAN_CXC_MAILBOX_M0_2_CK_CHANNEL, SILAN_CXC_INT_M0_2_CK_CHANNEL_ACK);
	}
}

int silan_cxc_message_send(unsigned char* context, int num, int idx_int, int idx_mutex, int idx_mailbox, int idx_ack)
{
	int ret = 0, count = 5, i = 0;
	unsigned int *data = (unsigned int *)context;
	num = (num+3)/4;
	if (num > SILAN_CXC_MAILBOX_NUM_LIMIT) {
		printf("%s %d context is exceed!!\n", __func__, __LINE__);
		return -1;
	}
	//step1: get mutex
	ret = silan_cxc_get_mutex(idx_mutex);
	if (ret != 0)
		return -1;

	//step2: fill context
	for (i=0; i<num; i++) {
		REG32(SILAN_CXC_MAILBOX_BASE + 4*idx_mailbox + 4*i) = data[i];
	}

	//step6: clear ack
	REG32(SILAN_CXC_RAW_INT_CLR) |= 1<<idx_ack;

	//step4: int set
	REG32(SILAN_CXC_RAW_INT_SET) |= 1<<idx_int;

	//step3: release mutex
	silan_cxc_release_mutex(idx_mutex);

#ifdef CXC_MESSAGE_ACK
	//step5: wait ack
	count = 1000;
	do {
		ret = REG32(SILAN_CXC_RAW_INT_STATUS);
		if(ret & (1<<idx_ack)) {
			break;
		}
		else {
			up_udelay(10);
			count --;
		}
		if (count < 0) {
			printf("%s %d get cxc ack failed\n", __func__, __LINE__);
			return -1;
		}
	}while(1);
#endif
	return 0;
}

int silan_cxc_message_m0_2_ck(unsigned char* context, int num)
{
	return silan_cxc_message_send(context, num, SILAN_CXC_INT_M0_2_CK_CHANNEL, SILAN_CXC_MUTEX_M0_2_CK_CHANNEL, SILAN_CXC_MAILBOX_M0_2_CK_CHANNEL, SILAN_CXC_INT_M0_2_CK_CHANNEL_ACK);
}

int silan_cxc_message_ck_2_dsp(unsigned char* context, int num)
{
	return silan_cxc_message_send(context, num, SILAN_CXC_INT_CK_2_DSP_CHANNEL, SILAN_CXC_MUTEX_CK_2_DSP_CHANNEL, SILAN_CXC_MAILBOX_CK_2_DSP_CHANNEL, SILAN_CXC_INT_CK_2_DSP_CHANNEL_ACK);
}

int silan_cxc_set_flash_start(int data)
{
	register volatile int *flash_start = (int *)(SILAN_CXC_MAILBOX_BASE + 4*SILAN_CXC_MAILBOX_FLASH_START);
	*flash_start = data;
	return 0;
}

int silan_cxc_get_flash_start(void)
{
	register volatile int *flash_start = (int *)(SILAN_CXC_MAILBOX_BASE + 4*SILAN_CXC_MAILBOX_FLASH_START);
	return *flash_start;
}

int silan_cxc_set_flash_stop(int data)
{
	register volatile int *flash_stop = (int *)(SILAN_CXC_MAILBOX_BASE + 4*SILAN_CXC_MAILBOX_FLASH_STOP);
	*flash_stop = data;
	return 0;
}

int silan_cxc_get_flash_stop(void)
{
	register volatile int *flash_stop = (int *)(SILAN_CXC_MAILBOX_BASE + 4*SILAN_CXC_MAILBOX_FLASH_STOP);
	return *flash_stop;
}

int silan_cxc_message_ck_2_m0(unsigned char* context, int num)
{
	silan_cxc_mesg_t *mesg = (silan_cxc_mesg_t *)context;
	int cnt = 1000000;
	if (mesg->main_cmd == SILAN_CXC_CMD_FLASH_SOLO) {
		if (silan_cxc_get_flash_start() == 1) {
			printf("flash solo already\n");
			return 0;
		}
		else {
			silan_cxc_set_flash_stop(0);
		}
	}
	int ret = silan_cxc_message_send(context, num, SILAN_CXC_INT_CK_2_M0_CHANNEL, SILAN_CXC_MUTEX_CK_2_M0_CHANNEL, SILAN_CXC_MAILBOX_CK_2_M0_CHANNEL, SILAN_CXC_INT_CK_2_M0_CHANNEL_ACK);
	if (ret == 0) {
		if (mesg->main_cmd == SILAN_CXC_CMD_FLASH_SOLO) {
			while ((silan_cxc_get_flash_start() == 0) && (cnt--)) {
				up_udelay(10);
			}
		}
		return 0;
	}
	return -1;
}

int silan_cxc_message_dsp_2_ck(unsigned char* context, int num)
{
	return silan_cxc_message_send(context, num, SILAN_CXC_INT_DSP_2_CK_CHANNEL, SILAN_CXC_MUTEX_DSP_2_CK_CHANNEL, SILAN_CXC_MAILBOX_DSP_2_CK_CHANNEL, SILAN_CXC_INT_DSP_2_CK_CHANNEL_ACK);
}

int silan_cxc_message_ck_2_dsp_custom(unsigned char* context, int num)
{
	//int i = 0;
	int len = 4*sizeof(char) + num + 1;
	char custom[4] = {0xf0, 0xf0, 0xf0, 0xf0};
	char *data = malloc(len);
	if (NULL == data) {
		return -1;
	}
	memset(data, 0, len);
	memcpy(data, custom, 4*sizeof(char));
	if (context)
		memcpy(data+4*sizeof(char), context, num);
	silan_cxc_message_ck_2_dsp((unsigned char*)data, len - 1);
	free(data);
	return 0;
}

int silan_cxc_message_ck_2_dsp_test(unsigned char* context, int num)
{
	//int i = 0;
	int len = 4*sizeof(char) + num + 1;
	char custom[4] = {0xf0, 0xf0, 0xf0, 0xf1};
	char *data = malloc(len);
	if (NULL == data) {
		return -1;
	}
	memset(data, 0, len);
	memcpy(data, custom, 4*sizeof(char));
	if (context)
		memcpy(data+4*sizeof(char), context, num);
	silan_cxc_message_ck_2_dsp((unsigned char*)data, len - 1);
	free(data);
	return 0;
}

//#include <silan_resources.h>
static int silan_cxc_reset(void)
{
	uint32_t tmp;

	tmp = REG32(SILAN_SYSCFG_REG9);
	tmp &= ~(0x1<<12);
	REG32(SILAN_SYSCFG_REG9) = tmp;
	tmp |= (0x1<<12);
	REG32(SILAN_SYSCFG_REG9) = tmp;

	return 0;
}

static cxc_hdl_t g_cxc_msg_cb = NULL;
static int cxc_message_cb_wakeup(unsigned char*data, int num)
{
#if 0
	int i;
	printf ("wakeup ");
	for(i = 0; i < num; i++)
		printf ("%02x ", data[i]);
	printf ("\n");
#endif

	silan_cxc_mesg_t msg;
	if (num >= sizeof(silan_cxc_mesg_t)) {
		memcpy(&msg, data, sizeof(silan_cxc_mesg_t));
	}

	if (g_cxc_msg_cb) {
		g_cxc_msg_cb(&msg);
	}

	return 0;
}

int silan_cxc_init(cxc_hdl_t cxc_msg_cb)
{
	int ret = 0;
	silan_cxc_reset();
	//silan_cxc_memssage_receive_register(NULL, SILAN_CXC_INT_M0_2_CK_CHANNEL);
	silan_cxc_memssage_receive_register(cxc_message_cb_wakeup, SILAN_CXC_INT_DSP_WAKEUP);
	silan_cxc_memssage_receive_register(NULL, SILAN_CXC_INT_M0_LPM_QUIT_GPIO);
	silan_cxc_memssage_receive_register(NULL, SILAN_CXC_INT_M0_LPM_QUIT_TIMER);

	silan_pic_request(PIC_IRQID_CXC, 0, (hdl_t)silan_cxc_interrupt);

	g_cxc_msg_cb = cxc_msg_cb;

	return ret;
}
