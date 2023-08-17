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
#include "ext4_vfs.h"

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

void print_usage_dd(void)
{
    printf("\r\n");
    printf("\t dd [storage] [size(M)] \r\n");
    printf("\t su: dd sd 512\r\n");
    printf("\t su: dd emmc 1024\r\n");
}

void sd_rw_test(int32_t argc, char **argv)
{
	int i = 0;
	uint32_t *test = malloc(1024 * 1024);//0x80400000;
	struct timespec time[2];
	long speed, v1, v2, v3, v4;
	long time_speed, size;
	char test_file[256] = {0,};
	char storage_name[256] = {0,};

	if (argc < 2) {
        printf("input param less than 2: \r\n");
        print_usage_dd();
		return;
	}
	memcpy(storage_name,argv[1],strlen(argv[1]));
	if (strcmp(storage_name, "sd") == 0) {
		sprintf(test_file,"%s/test",SD_FATFS_MOUNTPOINT);
	} else if(strcmp(storage_name, "emmc") == 0) {
		sprintf(test_file,"%s/test",EXT4_MOUNTPOINT);
	} else if(strcmp(storage_name, "help") == 0) {
		print_usage_dd();
		return ;
	} else {
		printf("invaild param: %s\n",argv[1]);
		return ;
	}
	size = argv[2] == NULL ? 512 : atoi(argv[2]); 	//default 512M
	printf("test_file = %s,size = %ld MB\n",test_file,size);

	aos_remove(test_file);
	printf("%s write speed test start wait......\n",storage_name);
	int out = aos_open(test_file, O_CREAT | O_RDWR);
	if (out < 0) {
		printf("%s  write speed test aos_open failed.\n",storage_name);
		printf("please check whether the %s exists?\n",storage_name);
		return;
	}

	clock_gettime(1, &time[0]);
	for(i = 0; i < size; i++)
		aos_write(out, (void *)test, (1024 * 1024));
	clock_gettime(1, &time[1]);
	aos_close(out);

	time_speed = diff_in_us(time[0], time[1]) / 1000;
	speed = (size * 1000000) / time_speed;
	v1 = speed / 1000;
	v2 = speed / 100 % 10;
	v3 = speed / 10 % 10;
	v4 = speed % 10;
	printf("%s write speed is %ld.%ld%ld%ldM/s.\n",storage_name, v1, v2, v3, v4);

	printf("%s read speed test start wait......\n",storage_name);
	int inp = aos_open(test_file, O_RDWR);
	if (inp < 0)
		printf("%s  read speed test aos_open failed.\n",storage_name);
	clock_gettime(1, &time[0]);
	for(i = 0; i < size; i++)
		aos_read(inp, (void *)test, (1024 * 1024));
	clock_gettime(1, &time[1]);
	aos_close(inp);

	time_speed = diff_in_us(time[0], time[1]) / 1000;
	speed = (size * 1000000) / time_speed;
	v1 = speed / 1000;
	v2 = speed / 1000 % 10;
	v3 = speed / 10 % 10;
	v4 = speed % 10;
	printf("%s read speed is %ld.%ld%ld%ldM/s.\n",storage_name,v1, v2, v3, v4);

	free(test);
/*
	aos_stat_t st;

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