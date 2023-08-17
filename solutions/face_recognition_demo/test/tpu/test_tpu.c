
#include "aos/kernel.h"
#include "asm/barrier.h"
// #include "atoi.h"
#include "test_tpu.h"
#include "cvi_tpu_interface.h"
#include "cvi_tpu_proc.h"
#include "aos/cli.h"

int cvi_tpu_tdma_test_002(void)
{
	struct cvi_tpu_device *ndev = NULL;

	ndev = cvi_tpu_open();
	if (ndev == NULL) {
		aos_cli_printf("open tpu device failed!\r\n");
		return -1;
	}
#if 0
	aos_cli_printf("ioctl CVITPU_WAIT_PIO\r\n");
	cvi_tpu_ioctl(ndev, CVITPU_WAIT_PIO, NULL);
#endif
	aos_cli_printf("tdmaBaseAddr = 0x%x , tiuBaseAddr = 0x%x\r\n", \
		   ndev->tdma_paddr, ndev->tiu_paddr);

	//suspend / resume
	// aos_cli_printf("platform_tpu_suspend()\r\n");
	// platform_tpu_suspend(ndev);
	// aos_cli_printf("platform_tpu_resume()\r\n");
	// platform_tpu_resume(ndev);

	//set pio cfg
#define TEST_NUM        20
	char *paddr_src[TEST_NUM];
	char *paddr_dst[TEST_NUM];
	struct cvi_tdma_copy_arg pioCfg[TEST_NUM];
	struct cvi_cache_op_arg opCfg;
	char *p, *q;
	for (uint32_t pioCfgIndex = 0; pioCfgIndex < TEST_NUM; pioCfgIndex++) {
		pioCfg[pioCfgIndex].enable_2d = 0;
		pioCfg[pioCfgIndex].seq_no = pioCfgIndex;
		pioCfg[pioCfgIndex].leng_bytes = 1024;
		paddr_src[pioCfgIndex] = (char *)aos_malloc(pioCfg[pioCfgIndex].leng_bytes + 0x3f);
		paddr_dst[pioCfgIndex] = (char *)aos_malloc(pioCfg[pioCfgIndex].leng_bytes + 0x3f);
		if (paddr_dst[pioCfgIndex] == NULL || paddr_src[pioCfgIndex] == NULL) {
			aos_cli_printf("aos_malloc src/dst failed!\r\n");
			return -1;
		}
		pioCfg[pioCfgIndex].paddr_dst = (uintptr_t)paddr_dst[pioCfgIndex];
		pioCfg[pioCfgIndex].paddr_src = (uintptr_t)paddr_src[pioCfgIndex];
		// 64 byte align
		pioCfg[pioCfgIndex].paddr_dst = (pioCfg[pioCfgIndex].paddr_dst + 0x3f) & (~0x3f);
		pioCfg[pioCfgIndex].paddr_src = (pioCfg[pioCfgIndex].paddr_src + 0x3f) & (~0x3f);
		p = (char *)pioCfg[pioCfgIndex].paddr_src;
		for (uint32_t tmpIndex = 0; tmpIndex < pioCfg[pioCfgIndex].leng_bytes; tmpIndex++) {
			p[tmpIndex] = 'A' + pioCfgIndex;
		}

		//tpu cache flush
		opCfg.paddr = pioCfg[pioCfgIndex].paddr_src;
		opCfg.size = pioCfg[pioCfgIndex].leng_bytes;
		cvi_tpu_ioctl(ndev, CVITPU_DMABUF_FLUSH, (unsigned long)&opCfg);

		opCfg.paddr = pioCfg[pioCfgIndex].paddr_dst;
		opCfg.size = pioCfg[pioCfgIndex].leng_bytes;
		cvi_tpu_ioctl(ndev, CVITPU_DMABUF_INVLD, (unsigned long)&opCfg);

		//tpu tdma process
		aos_cli_printf("platform_run_pio()\r\n");
		cvi_tpu_ioctl(ndev, CVITPU_SUBMIT_PIO, (unsigned long) & (pioCfg[pioCfgIndex]));
	}
	aos_cli_printf("CVITPU_WAIT_PIO\r\n");

	//wait for pio done
	int result = 0;
	struct cvi_tdma_wait_arg waitCfg = {.seq_no = 1, .ret = 0};
	for (uint32_t pioCfgIndex = 0; pioCfgIndex < TEST_NUM; pioCfgIndex++) {
		waitCfg.seq_no = pioCfgIndex;
		waitCfg.ret = 0;
		aos_cli_printf("CVITPU_WAIT_PIO\r\n");
		cvi_tpu_ioctl(ndev, CVITPU_WAIT_PIO, (unsigned long)&waitCfg);
		//tpu release
		//tpu dma invalidate
		opCfg.paddr = pioCfg[pioCfgIndex].paddr_dst;
		opCfg.size = pioCfg[pioCfgIndex].leng_bytes;

		aos_cli_printf("CVITPU_DMABUF_INVLD\r\n");
		cvi_tpu_ioctl(ndev, CVITPU_DMABUF_INVLD, (unsigned long)&opCfg);
		aos_cli_printf("CVITPU_DMABUF_INVLD end\r\n");
		//printf dst data
		p = (char *)pioCfg[pioCfgIndex].paddr_src;
		q = (char *)pioCfg[pioCfgIndex].paddr_dst;
		
		for (uint32_t tmpIndex = 0; tmpIndex < pioCfg[pioCfgIndex].leng_bytes; tmpIndex++) {
			if (p[tmpIndex] != q[tmpIndex]) {
				aos_cli_printf("[%s %d] [%d\t%d]:%d->%d error\r\n", 
				__FUNCTION__, __LINE__, pioCfgIndex, tmpIndex, p[tmpIndex], q[tmpIndex]);
				result = 1;
			} else {
				// aos_cli_printf("[%s %d] [%d\t%d]:%d->%d success\r\n", __FUNCTION__, __LINE__, pioCfgIndex, tmpIndex, p[tmpIndex], q[tmpIndex]);
			}
		}
		// aos_cli_printf("\r\n");
	}
	if(result)
		aos_cli_printf("test error\r\n");
	else
		aos_cli_printf("test success\r\n");
	for (uint32_t pioCfgIndex = 0; pioCfgIndex < TEST_NUM; pioCfgIndex++) {
		if(NULL != paddr_src[pioCfgIndex])
			aos_free(paddr_src[pioCfgIndex]);
		if(NULL != paddr_dst[pioCfgIndex])
			aos_free(paddr_dst[pioCfgIndex]);
	}
	cvi_tpu_close();

	return 0;
}

int cvi_tpu_tdma_test_001(void)
{
	struct cvi_tpu_device *ndev = NULL;

	ndev = cvi_tpu_open();
	if (ndev == NULL) {
		aos_cli_printf("open tpu device failed!\r\n");
		return -1;
	}
#if 0
	aos_cli_printf("ioctl CVITPU_WAIT_PIO\r\n");
	cvi_tpu_ioctl(ndev, CVITPU_WAIT_PIO, NULL);
#endif
	aos_cli_printf("tdmaBaseAddr = 0x%lx , tiuBaseAddr = 0x%lx\r\n", \
		   ndev->tdma_paddr, ndev->tiu_paddr);

	//suspend / resume
	// aos_cli_printf("platform_tpu_suspend()\r\n");
	// platform_tpu_suspend(ndev);
	// aos_cli_printf("platform_tpu_resume()\r\n");
	// platform_tpu_resume(ndev);

	//set pio cfg
	struct cvi_tdma_copy_arg pioCfg;
	pioCfg.enable_2d = 0;
	pioCfg.seq_no = 1;
	pioCfg.leng_bytes = 1024;
	char *paddr_src = (char *)aos_malloc(pioCfg.leng_bytes + 0x3f);
	char *paddr_dst = (char *)aos_malloc(pioCfg.leng_bytes + 0x3f);
	aos_cli_printf("[%s %d] paddr_src=0x%x  paddr_dst=0x%x\r\n", __FUNCTION__, __LINE__, paddr_src, paddr_dst);

	if (paddr_dst == NULL || paddr_src == NULL) {
		aos_cli_printf("aos_malloc src/dst failed!\r\n");
		return -1;
	}
	pioCfg.paddr_dst = (uintptr_t)paddr_dst;
	pioCfg.paddr_src = (uintptr_t)paddr_src;
	// 64 byte align
	pioCfg.paddr_dst = (pioCfg.paddr_dst + 0x3f) & (~0x3f);
	pioCfg.paddr_src = (pioCfg.paddr_src + 0x3f) & (~0x3f);
	aos_cli_printf("[%s %d] pioCfg.paddr_src=0x%x  pioCfg.paddr_dst=0x%x\r\n", __FUNCTION__, __LINE__, pioCfg.paddr_src,
		   pioCfg.paddr_dst);
	char *p = (char *)pioCfg.paddr_src;
	for (uint32_t tmpIndex = 0; tmpIndex < pioCfg.leng_bytes; tmpIndex++) {
		p[tmpIndex] = tmpIndex % 200;
	}

	//tpu cache flush
	struct cvi_cache_op_arg opCfg;
	opCfg.paddr = pioCfg.paddr_src;
	opCfg.size = pioCfg.leng_bytes;
	cvi_tpu_ioctl(ndev, CVITPU_DMABUF_FLUSH, (unsigned long)&opCfg);
	opCfg.paddr = pioCfg.paddr_dst;
	opCfg.size = pioCfg.leng_bytes;
	cvi_tpu_ioctl(ndev, CVITPU_DMABUF_INVLD, (unsigned long)&opCfg);

	//tpu tdma process
	aos_cli_printf("CVITPU_SUBMIT_PIO\r\n");
	cvi_tpu_ioctl(ndev, CVITPU_SUBMIT_PIO, (unsigned long)&pioCfg);
	//wait for pio done
	struct cvi_tdma_wait_arg waitCfg = {.seq_no = 1, .ret = 0};
	aos_cli_printf("CVITPU_WAIT_PIO\r\n");
	cvi_tpu_ioctl(ndev, CVITPU_WAIT_PIO, (unsigned long)&waitCfg);
	//tpu release
	//tpu dma invalidate
	opCfg.paddr = pioCfg.paddr_dst;
	opCfg.size = pioCfg.leng_bytes;
	aos_cli_printf("CVITPU_DMABUF_INVLD\r\n");
	cvi_tpu_ioctl(ndev, CVITPU_DMABUF_INVLD, (unsigned long)&opCfg);
	//printf dst data
	char *q = (char *)pioCfg.paddr_src;
	p = (char *)pioCfg.paddr_dst;
	int result = 0;
	for (uint32_t tmpIndex = 0; tmpIndex < pioCfg.leng_bytes; tmpIndex++) {
		if(q[tmpIndex] != p[tmpIndex])
			result = 1;
		aos_cli_printf("[%d]:%d->%d\t", tmpIndex, q[tmpIndex], p[tmpIndex]);
		if (tmpIndex % 5 == 0)
			aos_cli_printf("\r\n");
	}
	if(result)
		aos_cli_printf("\r\ntest error\r\n");
	else
		aos_cli_printf("\r\ntest success\r\n");
	if(NULL != paddr_src)
		aos_free(paddr_src);
	if(NULL != paddr_dst)
		aos_free(paddr_dst);
	cvi_tpu_close();
	return 0;
}

void cvi_tpu_test(int32_t argc, char **argv)
{
	if (2 != argc) {
		aos_cli_printf("para error\r\n");
	} else if (1 == atoi(argv[1])) {
		cvi_tpu_tdma_test_001();
	} else if (2 == atoi(argv[1])) {
		cvi_tpu_tdma_test_002();
	} else {
		aos_cli_printf("para error\r\n");
	}
}
ALIOS_CLI_CMD_REGISTER(cvi_tpu_test, tpu_tdma, tpu tdma);

