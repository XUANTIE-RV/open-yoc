#include <stdio.h>
#include <aos/aos.h>
#include <aos/log.h>
#include <aos/cli.h>
#include <yoc/partition.h>

#define TAG "flash"

#define TEST_TIMES 10
#define TEST_BUFFER_SIZE (1*1024*1024)
#define TEST_DATA_MBYTE (TEST_TIMES*TEST_BUFFER_SIZE/1024/1024)

void flash_test_read()
{
    int i;

    partition_t handle = partition_open("misc");
    if (handle < 0) {
        return;
    }

    partition_info_t *lp = hal_flash_get_info(handle);
    printf("flash info length = %d, sector_size=%d\n", lp->length, lp->sector_size);

    int   buff_size = TEST_BUFFER_SIZE;
    char *buff      = malloc(buff_size);
    memset(buff, 0xAA, buff_size);

    LOGI(TAG, "START");
    int beg = aos_now_ms();
    for (i = 0; i < TEST_TIMES; i++) {
        if (partition_read(handle, 0, buff, buff_size) >= 0) {

        } else {
            printf("read error\n");
        }
    }
    int end = aos_now_ms();
    LOGI(TAG, "END");
    LOGI(TAG, "%.3fMB/s",  (TEST_DATA_MBYTE * 1000.0) / (end - beg) );

    LOG_HEXDUMP(TAG, buff, 128);

    free(buff);

    partition_close(handle);
}

void flash_test_write()
{
    int i;

    partition_t handle = partition_open("misc");
    if (handle < 0) {
        return;
    }

    partition_info_t *lp = hal_flash_get_info(handle);
    printf("flash info length = %d, sector_size=%d\n", lp->length, lp->sector_size);

    int   buff_size = TEST_BUFFER_SIZE;
    char *buff      = malloc(buff_size);
    memset(buff, 0x55, buff_size);

    LOGI(TAG, "START");
    int beg = aos_now_ms();
    for (i = 0; i < TEST_TIMES; i++) {
        if (partition_write(handle, 0, buff, buff_size) >= 0) {

        } else {
            printf("write error\n");
        }
    }
    int end = aos_now_ms();
    LOGI(TAG, "END");
    LOGI(TAG, "%.3fMB/s",  (TEST_DATA_MBYTE * 1000.0) / (end - beg) );
    free(buff);

    partition_close(handle);
}

void flash_test_erase()
{
    int i;

    partition_t handle = partition_open("misc");
    if (handle < 0) {
        return;
    }

    partition_info_t *lp = hal_flash_get_info(handle);
    printf("flash info length = %d, sector_size=%d\n", lp->length, lp->sector_size);

    int   buff_size = TEST_BUFFER_SIZE;

    LOGI(TAG, "START");
    int beg = aos_now_ms();
    for (i = 0; i < TEST_TIMES; i++) {
        if (partition_erase(handle, 0, buff_size / lp->sector_size) >= 0) {

        } else {
            printf("erase error\n");
        }
    }
    int end = aos_now_ms();
    LOGI(TAG, "END");
    LOGI(TAG, "%.3fMB/s",  (TEST_DATA_MBYTE * 1000.0) / (end - beg) );

    partition_close(handle);
}

static void cmd_app_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (argc < 2) {
        return;
    }

    if (strcmp(argv[1], "read") == 0) {
        flash_test_read();
    } else if (strcmp(argv[1], "write") == 0) {
        flash_test_write();
    } else if (strcmp(argv[1], "erase") == 0) {
        flash_test_erase();
    } else {
        ;
    }
}

void test_flash_cmd(void)
{
    static const struct cli_command cmd_info = {"flash", "flash", cmd_app_func};

    aos_cli_register_command(&cmd_info);
}
