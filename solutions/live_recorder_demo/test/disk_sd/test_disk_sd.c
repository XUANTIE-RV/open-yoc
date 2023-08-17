#include <yoc/init.h>
#include <aos/cli.h>
#include <ff.h>
#include <soc.h>
#include "sys/prctl.h"
#include "pthread.h"
#include "semaphore.h"
#include "pinctrl-mars.h"
#include "disk_sd.h"
#include "fatfs_vfs.h"
#include "vfs.h"

extern void sdhci_pinmux_dump(void);
extern void sdhci_register_dump(void *base);
void dump_sdio_pin(int32_t argc, char **argv)
{
	sdhci_pinmux_dump();
}
void dump_sdio_reg(int32_t argc, char **argv)
{
	if (2 != argc) {
		printf("para error\r\n");
	} else if (0 == atoi(argv[1])) {
		sdhci_register_dump((void *)0x04310000);
	} else if (1 == atoi(argv[1])) {
		sdhci_register_dump((void *)0x04320000);
	} else {
		sdhci_register_dump((void *)0x04310000);
	}
}
ALIOS_CLI_CMD_REGISTER(dump_sdio_pin, dump_sdio_pin, dump sdio pinmux);
ALIOS_CLI_CMD_REGISTER(dump_sdio_reg, dump_sdio_reg, dump sdio reg);



static long diff_in_us(struct timespec t1, struct timespec t2)
{
	struct timespec diff;

	if (t2.tv_nsec - t1.tv_nsec < 0) {
		diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
		diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
	} else {
		diff.tv_sec  = t2.tv_sec - t1.tv_sec;
		diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
	}

	return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

void sd_rw_test(int32_t argc, char **argv)
{
	int i = 0;
	uint32_t *test = malloc(1024 * 1024);//0x80400000;
	struct timespec time[2];
	long speed, v1, v2, v3, v4;
	long time_speed, size;

	aos_remove(SD_FATFS_MOUNTPOINT"/test");
	printf("sd write speed test start wait......\n");
	int out = aos_open(SD_FATFS_MOUNTPOINT"/test", O_CREAT | O_RDWR);
	if (out < 0) {
		printf("sd write speed test aos_open failed.\n");
		printf("please check whether the SD card exists?\n");
		return;
	}
	clock_gettime(1, &time[0]);
	for(i = 0; i < 50; i++)
		aos_write(out, (void *)test, (1024 * 1024));
	clock_gettime(1, &time[1]);
	aos_close(out);

	time_speed = diff_in_us(time[0], time[1]) / 1000;
	size = 50;
	speed = (size * 1000000) / time_speed;
	v1 = speed / 1000;
	v2 = speed / 100 % 10;
	v3 = speed / 10 % 10;
	v4 = speed % 10;
	printf("sd card write speed is %ld.%ld%ld%ldM/s.\n", v1, v2, v3, v4);

	printf("sd read speed test start wait......\n");
	int inp = aos_open(SD_FATFS_MOUNTPOINT"/test", O_RDWR);
	if (inp < 0)
		printf("sd read speed test aos_open failed.\n");
	clock_gettime(1, &time[0]);
	for(i = 0; i < 50; i++)
		aos_read(inp, (void *)test, (1024 * 1024));
	clock_gettime(1, &time[1]);
	aos_close(inp);

	time_speed = diff_in_us(time[0], time[1]) / 1000;
	size = 50;
	speed = (size * 1000000) / time_speed;
	v1 = speed / 1000;
	v2 = speed / 1000 % 10;
	v3 = speed / 10 % 10;
	v4 = speed % 10;
	printf("sd card read speed is %ld.%ld%ld%ldM/s.\n", v1, v2, v3, v4);

	free(test);
/*
	struct aos_stat st;

	aos_stat(SD_FATFS_MOUNTPOINT"/aa.jpg", &st);
	aos_cli_printf("aa.jpg = %d.\r\n", st.st_size);
	uint32_t *bmp = malloc(st.st_size);

	int ou = aos_open(SD_FATFS_MOUNTPOINT"/aa.mp4", O_RDWR);
	aos_read(ou, (void *)bmp, st.st_size);
	aos_close(ou);
	aos_cli_printf("read end.\r\n");

	int in = aos_open(SD_FATFS_MOUNTPOINT"/bb.mp4", O_CREAT | O_RDWR);
	aos_write(in, (void *)bmp, st.st_size);
	aos_close(in);
	free(bmp);
*/

}
ALIOS_CLI_CMD_REGISTER(sd_rw_test, dd, dd sdcard);