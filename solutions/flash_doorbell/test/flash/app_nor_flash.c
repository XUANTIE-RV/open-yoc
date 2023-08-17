/*
 * Copyright (C) 2022-2032 CVITEK
 *
 * function:
 * 1. nor read/write rate test.
 *
 */

#include "board.h"
#include <aos/kernel.h>
#include <stdio.h>
#include <ulog/ulog.h>
#include <aos/cli.h>
//#include "cvi_sys.h"
#include "drv/spiflash.h"
#include "drv/spi.h"
#include "debug/debug_dumpsys.h"
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/adc.h>
#include <drv/pwm.h>
#include <drv/wdt.h>
#include <posix/timer.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include "yoc/partition.h"
#include "yoc/partition_flash.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
        uint32_t offset;
        uint32_t len;
} nor_op_param_t;

typedef struct {
	uint32_t idx;
	uint32_t offset;
	uint32_t size;
} test_spiflash_args_t;

void count_clock_time(struct timespec *start, struct timespec *end, struct timespec *time_gap)
{
        if (end->tv_nsec < start->tv_nsec) {
                time_gap->tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
                time_gap->tv_sec = end->tv_sec - start->tv_sec - 1;
        } else {
                time_gap->tv_sec = end->tv_sec - start->tv_sec;
                time_gap->tv_nsec = end->tv_nsec - start->tv_nsec;
        }
}

void false_division(uint32_t *val, uint64_t dividend, uint64_t divisor, int bit)
{
    int cycle = 1;
    val[0] = dividend / divisor;

    do {
        if (dividend < divisor)
                dividend *= 10;
        else
                dividend = (dividend % divisor) * 10;
        val[cycle] = dividend / divisor;
        cycle++;
    }while(cycle < bit);

}

#define RW_SPEED_TEST

void *nor_flash_test(void *args)
{
	static int ret = 0;
	int32_t i;
	uint8_t *tx_data = NULL;
	uint8_t *rx_data = NULL;
	csi_spiflash_info_t info;
	csi_spiflash_t spiflash_handle;
	struct timespec start, end, time_gap;
	unsigned long long spend_time;
	uint32_t val[4] = {0};

	test_spiflash_args_t spiflash_args;
	nor_op_param_t *nor_param = (nor_op_param_t *)args;

	spiflash_args.idx = 0;
	spiflash_args.offset = nor_param->offset;
	spiflash_args.size = nor_param->len;

	csi_spiflash_spi_init(&spiflash_handle, spiflash_args.idx, NULL);
	csi_spiflash_get_flash_info(&spiflash_handle, &info);

	if (spiflash_args.size == 0)
		return &ret;
#if 0
	if (spiflash_args.offset & (info.sector_size - 1)) {
		printf("offset not align to %u \n", info.sector_size);
		ret = -1;
		return &ret;
	}
#endif
	if (spiflash_args.size & (info.sector_size - 1))
		spiflash_args.size = (spiflash_args.size + (info.sector_size - 1)) &~((info.sector_size - 1));

	if ((spiflash_args.offset + spiflash_args.size) > info.flash_size) {
		printf("the range is greater flash size:%#x \n", info.flash_size);
		return &ret;
	}

	csi_spiflash_erase(&spiflash_handle, spiflash_args.offset, spiflash_args.size);

	tx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
	if (tx_data == NULL) {
		printf("malloc tx data buffer failed, ret:%d \n", ret);
		ret = -1;
		return &ret;
	}
	memset(tx_data, 0x45, sizeof(uint8_t) * spiflash_args.size);

	rx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
	if (rx_data == NULL) {
		printf("malloc rx data buffer failed!\n");
		ret = -1;
		return &ret;
	}
	memset(rx_data, 0xa, sizeof(uint8_t) * spiflash_args.size);

#ifdef  RW_SPEED_TEST
	clock_gettime(CLOCK_REALTIME, &start);
#endif

	ret = csi_spiflash_program(&spiflash_handle, spiflash_args.offset, tx_data, spiflash_args.size);
	if (ret != spiflash_args.size) {
		printf("nor flash program failed!\n");
		goto free;
	}

#ifdef  RW_SPEED_TEST
	clock_gettime(CLOCK_REALTIME, &end);
	count_clock_time(&start, &end, &time_gap);
	spend_time = time_gap.tv_sec * 1000000000 + time_gap.tv_nsec;
	printf("write spend time %llu ns ,data size:%#x byte\n", spend_time, spiflash_args.size);
	false_division(val, nor_param->len, spend_time / 1000, 4);
	printf("write speed is %u.%u%u%u MB/s\n", val[0], val[1], val[2], val[3]);
#endif

#ifdef  RW_SPEED_TEST
	clock_gettime(CLOCK_REALTIME, &start);
#endif
	ret = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, spiflash_args.size);
	if (ret < 0) {
		printf("nor flash read failed, ret:%d \n", ret);
		goto free;
	}

#ifdef  RW_SPEED_TEST
	clock_gettime(CLOCK_REALTIME, &end);
	count_clock_time(&start, &end, &time_gap);
	spend_time = time_gap.tv_sec * 1000000000 + time_gap.tv_nsec;
	printf("read spend time %llu ns ,data size:%#x byte\n", spend_time, spiflash_args.size);
	false_division(val, nor_param->len, spend_time / 1000, 4);
	printf("read speed is %u.%u%u%u MB/s\n", val[0], val[1], val[2], val[3]);
#endif
	ret = memcmp(tx_data, rx_data, spiflash_args.size);
	if (ret == 0) {
		printf("the results of spiflash reading and programing are equal\n");
		ret = 0;
	} else {
		for (i = 0; i < spiflash_args.size; i++) {
			if (rx_data[i] != 0x45)
				break;
		}
		printf("%d data is same,the rest data is not same\n", i);
		printf("the results of spiflash reading and programing are not equal\n");
		ret = -2;
	}

free:
	free(tx_data);
	free(rx_data);

	csi_spiflash_spi_uninit(&spiflash_handle);
	return &ret;
}

void *nor_flash_read_test(void *args)
{
	static int ret = 0;
	uint8_t *rx_data = NULL;
	csi_spiflash_info_t info;
	csi_spiflash_t spiflash_handle;
	struct timespec start, end, time_gap;
	unsigned long long spend_time;
	uint32_t val[4] = {0};

	test_spiflash_args_t spiflash_args;
	nor_op_param_t *nor_param = (nor_op_param_t *)args;

	spiflash_args.idx = 0;
	spiflash_args.offset = nor_param->offset;
	spiflash_args.size = nor_param->len;

	csi_spiflash_spi_init(&spiflash_handle, spiflash_args.idx, NULL);
	csi_spiflash_get_flash_info(&spiflash_handle, &info);

	if (spiflash_args.size == 0)
		return &ret;
#if 0
	if (spiflash_args.offset & (info.sector_size - 1)) {
		printf("offset not align to %u \n", info.sector_size);
		ret = -1;
		return &ret;
	}
#endif
	if (spiflash_args.size & (info.sector_size - 1))
		spiflash_args.size = (spiflash_args.size + (info.sector_size - 1)) &~((info.sector_size - 1));

	if ((spiflash_args.offset + spiflash_args.size) > info.flash_size) {
		printf("the range is greater flash size:%#x \n", info.flash_size);
		return &ret;
	}

	rx_data = (uint8_t *)malloc(sizeof(uint8_t) * spiflash_args.size);
	if (rx_data == NULL) {
		printf("malloc rx data buffer failed!\n");
		ret = -1;
		return &ret;
	}
	memset(rx_data, 0xa, sizeof(uint8_t) * spiflash_args.size);

#ifdef  RW_SPEED_TEST
	clock_gettime(CLOCK_REALTIME, &start);
#endif
	printf("[%s %d]===> read data......\n", __func__, __LINE__);
	ret = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, spiflash_args.size);
	if (ret < 0) {
		printf("nor flash read failed, ret:%d \n", ret);
		goto free;
	}

#ifdef  RW_SPEED_TEST
	clock_gettime(CLOCK_REALTIME, &end);
	count_clock_time(&start, &end, &time_gap);
	spend_time = time_gap.tv_sec * 1000000000 + time_gap.tv_nsec;
	printf("read spend time %llu ns ,data size:%#x byte\n", spend_time, spiflash_args.size);
	false_division(val, nor_param->len, spend_time / 1000, 4);
	printf("read speed is %u.%u%u%u MB/s\n", val[0], val[1], val[2], val[3]);
#endif

free:
	free(rx_data);

	csi_spiflash_spi_uninit(&spiflash_handle);
	return &ret;
}

static uint32_t count = 0;
void* print_task(void *count)
{	//char buf[128] = {0};
	for (int i = 0; i < 10000; i++) {
		//sprintf(buf, "[%s %d]====> i:%u",__func__,__LINE__, *(uint32_t *)count);
		printf("# %u #\n", *(uint32_t *)count);
		*(uint32_t *)count += 1;
		//printf("%s\n", buf);
	}
	return count;
}

int nor_test_thread(int argc, char **argv)
{
	//int *val = NULL;
	nor_op_param_t param = { 0x100000, 0x100000 };

	printf("creat thread for test!\n");
#if 1
	int ret;
	pthread_t id, id1;
	ret = pthread_create(&id, NULL, nor_flash_read_test, &param);
	if (ret < 0) {
		printf("creat thread failed!\n");
		return ret;
	}

	ret = pthread_create(&id1, NULL, print_task, &count);
	if (ret < 0) {
		printf("creat thread failed!\n");
		return ret;
	}

	ret = pthread_join(id, NULL);
	if (ret != 0) {
		printf("join thread failed!\n");
		return -1;
	}

	ret = pthread_join(id1, NULL);
	if (ret != 0) {
		printf("join thread failed!\n");
		return -1;
	}

	//while(1) {
		aos_msleep(30000);
	//}
#endif
	//val = (int *)nor_flash_test(&param);
	//printf("nor test exit, ret:%d\n", *val);

	return 0;
}
ALIOS_CLI_CMD_REGISTER(nor_test_thread, t_test, pthread test);

static int nor_cmd_test(int argc, char **argv)
{
	nor_op_param_t param = {0};
	int *ret = NULL;

	if (argc != 3) {
		printf("Usage: nor_test offset len \n");
		return 0;
	}

	param.offset =  strtol(argv[1], NULL, 0);
	param.len = strtol(argv[2], NULL, 0);
	ret = nor_flash_test(&param);
	if (*ret < 0) {
		printf("nor flash test failed!\n");
		return -1;
	}
	return 0;
}
ALIOS_CLI_CMD_REGISTER(nor_cmd_test, nor_test, spi nor flash read/write test);

void hex_dump(char *buf, int len, int addr)
{
	int i, j, k, extra_len;
	char binstr[128];

	extra_len = 0;
	for (i = 0; i < len; i++) {
		if ((i % 16) == 0) {
			extra_len = 0;
			extra_len += sprintf(binstr + extra_len, "%08x -", i + addr);
			extra_len += sprintf(binstr + extra_len, " %02x", (unsigned char)buf[i]);
		} else if ((i % 16) == 15) {
			extra_len += sprintf(binstr + extra_len, " %02x", (unsigned char)buf[i]);
			extra_len += sprintf(binstr + extra_len, "  ");
			for (j = i - 15; j <= i; j++)
				extra_len += sprintf(binstr + extra_len, "%c", ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
			printf("%s\n", binstr);
		} else {
			extra_len += sprintf(binstr + extra_len, " %02x", (unsigned char)buf[i]);
		}
	}

	if ((i % 16) != 0) {
		k = 16 - (i % 16);
		for (j = 0; j < k; j++)
			extra_len += sprintf(binstr + extra_len, "   ");
		extra_len += sprintf(binstr + extra_len, "  ");
		k = 16 - k;
		for (j = i - k; j < i; j++)
			extra_len += sprintf(binstr + extra_len, "%c", ('!' < buf[j] && buf[j] <= '~') ? buf[j] : '.');
		printf("%s\n", binstr);
	}
}

void *nor_flash_dump(void *args)
{
	static int ret = 0;
	char *rx_data = NULL;
	csi_spiflash_info_t info;
	csi_spiflash_t spiflash_handle;

	test_spiflash_args_t spiflash_args;
	nor_op_param_t *nor_param = (nor_op_param_t *)args;

	spiflash_args.idx = 0;
	spiflash_args.offset = nor_param->offset;
	spiflash_args.size = nor_param->len;

	csi_spiflash_spi_init(&spiflash_handle, spiflash_args.idx, NULL);
	csi_spiflash_get_flash_info(&spiflash_handle, &info);

	if (spiflash_args.size == 0)
		return &ret;

	if ((spiflash_args.offset + spiflash_args.size) > info.flash_size) {
		printf("the range is greater flash size:%#x \n", info.flash_size);
		ret = -1;
		return &ret;
	}

	rx_data = (char *)malloc(spiflash_args.size);
	if (rx_data == NULL) {
		printf("malloc rx data buffer failed!\n");
		ret = -1;
		return &ret;
	}
	memset(rx_data, 0x0, sizeof(uint8_t) * spiflash_args.size);
	ret = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, spiflash_args.size);
	if (ret < 0) {
		printf("nor flash read failed, ret:%d \n", ret);
		goto free;
	}
	hex_dump(rx_data, spiflash_args.size, spiflash_args.offset);
free:
	free(rx_data);

	csi_spiflash_spi_uninit(&spiflash_handle);
	return &ret;
}

static int nor_dump_data(int argc, char **argv)
{
	nor_op_param_t param = {0};
	int *ret = NULL;

	if (argc != 3) {
		printf("Usage: nor_dump offset len \n");
		return 0;
	}

	param.offset =  strtol(argv[1], NULL, 0);
	param.len = strtol(argv[2], NULL, 0);
	ret = nor_flash_dump(&param);
	if (*ret < 0 || *ret != param.len) {
		printf("read data failed!\n");
		return -1;
	}
	return 0;
}
ALIOS_CLI_CMD_REGISTER(nor_dump_data, nor_dump, spi nor flash dump data);

#define TESTNORFLASH_FILENAME "/mnt/sd/test_nor_flash"
#define TESTNORFLASH_PARTITION "misc"
#define TESTNORFLASH_TEST_COUNT 3000

static int _test_nor_flash_write(unsigned char * file_data, unsigned int file_length, unsigned int partition_start_addr)
{
	int ret = 0;
	int writesize = 0;
	csi_spiflash_info_t info;
	csi_spiflash_t spiflash_handle;
	test_spiflash_args_t spiflash_args;


	spiflash_args.idx = 0;
	spiflash_args.offset = partition_start_addr;
	spiflash_args.size = file_length;

	csi_spiflash_spi_init(&spiflash_handle, 0, NULL);
	csi_spiflash_get_flash_info(&spiflash_handle, &info);

	if (spiflash_args.size == 0) {
		ret = -1;
		goto EXIT;
	}
	if (spiflash_args.size & (info.sector_size - 1))
		spiflash_args.size = (spiflash_args.size + (info.sector_size - 1)) &~((info.sector_size - 1));

	if ((spiflash_args.offset + spiflash_args.size) > info.flash_size) {
		printf("the range is greater flash size:%#x \n", info.flash_size);
		ret = -1;
		goto EXIT;
	}
	printf("spiflash_args.size is %d \n", spiflash_args.size);
	
	csi_spiflash_erase(&spiflash_handle, spiflash_args.offset, spiflash_args.size);
	writesize = csi_spiflash_program(&spiflash_handle, spiflash_args.offset, file_data, spiflash_args.size);
	if (writesize != spiflash_args.size) {
		printf("nor flash program failed!\n");
		ret = -1;
	}
EXIT:
	csi_spiflash_spi_uninit(&spiflash_handle);
	return ret;
}

static int _test_nor_flash_dump_compare(unsigned char * file_data, unsigned int file_length, unsigned int partition_start_addr)
{
	int ret = 0;
	int readsize = 0;
	unsigned char *rx_data = NULL;
	csi_spiflash_info_t info;
	csi_spiflash_t spiflash_handle;

	test_spiflash_args_t spiflash_args;

	spiflash_args.idx = 0;
	spiflash_args.offset = partition_start_addr;
	spiflash_args.size = file_length;

	csi_spiflash_spi_init(&spiflash_handle, spiflash_args.idx, NULL);
	csi_spiflash_get_flash_info(&spiflash_handle, &info);

	if (spiflash_args.size == 0) {
		printf("spiflash_args.size == 0 \n");
		ret = -1;
		csi_spiflash_spi_uninit(&spiflash_handle);
		return ret;
	}

	if ((spiflash_args.offset + spiflash_args.size) > info.flash_size) {
		printf("the range is greater flash size:%#x \n", info.flash_size);
		ret = -1;
		csi_spiflash_spi_uninit(&spiflash_handle);
		return ret;
	}

	rx_data = (unsigned char *)malloc(sizeof(uint8_t) * spiflash_args.size);
	if (rx_data == NULL) {
		printf("malloc rx data buffer failed!\n");
		ret = -1;
		csi_spiflash_spi_uninit(&spiflash_handle);
		return ret;
	}
	memset(rx_data, 0x0, sizeof(uint8_t) * spiflash_args.size);
	readsize = csi_spiflash_read(&spiflash_handle, spiflash_args.offset, rx_data, spiflash_args.size);
	if (readsize < 0) {
		printf("nor flash read failed, ret:%d \n", ret);
		ret = -1;
		goto free;
	}
	if(readsize != spiflash_args.size) {
		printf("csi_spiflash_read readsize %d err \n", readsize);
		ret = -1;
		goto free;
	}
	if(memcmp(file_data, rx_data, spiflash_args.size) != 0) {
		printf("compare memcmp err \n");
		ret = -1;
	}
free:
	free(rx_data);
	csi_spiflash_spi_uninit(&spiflash_handle);
	return ret;

}

static int test_nor_flash(int argc, char **argv)
{
	//限定条件 (1).使用msic分区 (2).限定imtb存在  (3).指定SD卡文件名:/mnt/sd/test_nor_flash
	//1.使用分区检测msic size(parttion_size)注意空闲内存需要大于或者等于 msic size的两倍
	//2.探测需要压测文件是否存在
	//3.检测文件size是否超过分区大小
	//4.写入压测文件到flash分区
	//5.dump数据到内存 与SD卡文件作比较 如果数据异常则退出报错
	//6.循环3000次 4-5步骤
	partition_t parttion = 0;
	partition_info_t * parttion_info = NULL;
	unsigned int parttion_size = 0;
	unsigned int parttion_start_addr = 0;
	int filefd = 0;
	unsigned int file_length = 0;
	unsigned char * file_data = NULL;
	unsigned int readnum = 0;

	parttion = partition_open(TESTNORFLASH_PARTITION);
	if (parttion < 0) {
		printf("partition_open %s failed! ret = %d.\n",TESTNORFLASH_PARTITION, parttion);
		return -1;
	}
	printf("partition_open %s success.\n", TESTNORFLASH_PARTITION);
	parttion_info = partition_info_get(parttion);
	partition_close(parttion);
	parttion_size = parttion_info->length;
	parttion_start_addr = parttion_info->start_addr;

	if(access(TESTNORFLASH_FILENAME, F_OK) != 0) {
		printf("%s file no exit \n",TESTNORFLASH_FILENAME);
		return -1;
	}

	filefd = open(TESTNORFLASH_FILENAME, O_RDONLY);
	if(filefd <0) {
		printf("open %s err\n",TESTNORFLASH_FILENAME);
		return -1;
	}
	file_length = lseek(filefd, 0, SEEK_END);
	lseek(filefd, 0, SEEK_SET);
	printf("file size is %d \n", file_length);
	printf("parttion_size is %d \n",parttion_size);
	printf("parttion_start_addr is 0X%x \n",parttion_start_addr);
	if(file_length > parttion_size) {
		printf("file size [%d] over  parttion_size [%d] \n", file_length, parttion_size);
		return -1;
	}
	file_data = (unsigned char *)malloc(file_length);
	if( file_data == NULL) {
		printf("%s file_data malloc size %d error\n",__func__, file_length);
		goto EXIT;
	}
	readnum = read(filefd, file_data, file_length);
	if(readnum != file_length) {
		printf("readnum %d != file_length %d\n",readnum, file_length);
		goto EXIT;
	}
	for(int i = 0; i < TESTNORFLASH_TEST_COUNT; i++) {
		if(_test_nor_flash_write(file_data, file_length, parttion_start_addr) != 0) {
			printf("_test_nor_flash_write failed \n");
			printf("test_nor_flash failed \n");
			goto EXIT;
		}
		usleep(50*1000);
		if(_test_nor_flash_dump_compare(file_data, file_length, parttion_start_addr) != 0) {
			printf("_test_nor_flash_dump_compare failed \n");
			printf("test_nor_flash failed \n");
			goto EXIT;
		}
		usleep(50*1000);
	}
	printf("test_nor_flash success \n");
EXIT:
	if(file_data) {
		free(file_data);
	}
	if(filefd > 0) {
		close(filefd);
	}
	return 0;
}
ALIOS_CLI_CMD_REGISTER(test_nor_flash, test_nor_flash, test_nor_flash);
