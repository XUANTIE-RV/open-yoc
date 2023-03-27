#include <string.h>
#include <aos/cli.h>
#include <aos/aos.h>
#include <sys/stat.h>
#include <vfs.h>
#include <dirent.h>

static void cmd_fs_test_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    printf("write ...\r\n");
    int fd = aos_open("/fstest.bin", (O_WRONLY | O_CREAT | O_TRUNC));
    char * testbuffer = aos_malloc(1024);

    long long t1 = aos_now_ms();
    for (int i = 0; i < 256; i++) {
        aos_write(fd, testbuffer, 1024);
    }
    long long t2 = aos_now_ms();

    printf("write speed %lld Byte/s\r\n", (256 * 1024 * 1000) / (t2 - t1));

    aos_close(fd);


    printf("read ...\r\n");
    fd = aos_open("/fstest.bin", O_RDONLY);

    t1 = aos_now_ms();
    for (int i = 0; i < 256; i++) {
        aos_read(fd, testbuffer, 1024);
    }
    t2 = aos_now_ms();

    aos_close(fd);
    printf("read speed %lld Byte/s\r\n", (256 * 1024 * 1000) / (t2 - t1));

    aos_free(testbuffer);

    aos_unlink("/fstest.bin");
    printf("fs test done\r\n");
}

void cli_reg_cmd_fstst(void)
{
    static const struct cli_command cmd_info = { "fstst", "file system test", cmd_fs_test_func };
    aos_cli_register_command(&cmd_info);
}
