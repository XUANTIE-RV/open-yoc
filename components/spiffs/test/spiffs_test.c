#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <aos_spiffs.h>
#include <aos/log.h>
#include <aos/aos.h>
#include <yoc/yoc.h>
#include <vfs.h>

static const char *TAG = "spiffs_test";

#define TEST_ASSERT_EQUAL(name, expect, actual) do { \
                                                if (expect != actual) { \
                                                    printf("ASSERT[%s] %d\n", name, __LINE__); \
                                                    return; \
                                                } \
                                            } while(0);

#define TEST_ASSERT(name, v)  do { \
                            if (!(!!v)) { \
                                printf("ASSERT[%s] %d\n", name, __LINE__); \
                                return; \
                            } \
                        } while(0);

#define TEST_ASSERT_FALSE(name, v)  do { \
                                        if (v) { \
                                            printf("ASSERT[%s] %d\n", name, __LINE__); \
                                            return; \
                                        } \
                                    } while(0);

#define TEST_ASSERT_NEGATIVE(name, v)  do { \
                                        if (v >= 0) { \
                                            printf("ASSERT[%s] %d\n", name, __LINE__); \
                                            return; \
                                        } \
                                    } while(0);

#define TEST_ASSERT_POSITIVE(name, v)  do { \
                                        if (v <= 0) { \
                                            printf("ASSERT[%s] %d\n", name, __LINE__); \
                                            return; \
                                        } \
                                    } while(0);

#define TEST_ASSERT_0(name, v)  do { \
                                        if (v != 0) { \
                                            printf("ASSERT[%s] %d\n", name, __LINE__); \
                                            return; \
                                        } \
                                    } while(0);

#define TEST_ASSERT_NOT_0(name, v)  do { \
                                        if (v == 0) { \
                                            printf("ASSERT[%s] %d\n", name, __LINE__); \
                                            return; \
                                        } \
                                    } while(0);

#define TEST_ASSERT_NOT_NULL(name, v)   do { \
                                            if (v == NULL) { \
                                                printf("ASSERT[%s] %d\n", name, __LINE__); \
                                                return; \
                                            } \
                                        } while(0);

static void test_spiffs_create_file_with_text(const char* name, const char* text)
{
    char buf[128] = {0};

    int  fd = aos_open(name, O_CREAT | O_TRUNC | O_RDWR);
    if (fd  < 0) {
        LOGE(TAG, "fopen failed, fd:%d, %s", fd, name);
        return;
    }
    ssize_t len = aos_write(fd, text, strlen(text));
    if (len != strlen(text)) {
        LOGE(TAG, "write text failed, %d", len);
        return;
    }
    aos_lseek(fd, 0, SEEK_SET);
    len = aos_read(fd, buf, 128);
    printf("len=%d\n", len);
    printf("%s\n", buf);
    int ret = aos_close(fd);
    printf("ret=%d\n", ret);
    if (ret == 0) {
        printf("fclose ok\n");
    } else {
        printf("fclose failed\n");
    }
}

static void test_spiffs_overwrite_append(const char* filename)
{
    printf("-------------------test_spiffs_overwrite_append\n");
    /* Create new file with 'aaaa' */
    test_spiffs_create_file_with_text(filename, "aaaa");

    /* Append 'bbbb' to file */
    int fd = aos_open(filename, O_APPEND | O_RDWR);
    if (fd  < 0) {
        LOGE(TAG, "fopen failed");
        return;
    }
    printf("open %s , fd=%d, %d\n", filename, fd, __LINE__);
    char *text = "bbbb";
    char buf[128] = {0};
    if (aos_write(fd, text, strlen(text)) != strlen(text)) {
        printf("[%d]write text failed\n", __LINE__);
        return;
    }
    aos_lseek(fd, 0, SEEK_SET);
    if (aos_read(fd, buf, 128) > 0) {
        printf("read text ok, \n");
    }
    printf("======> %s\n", buf);
    aos_close(fd);

    /* Read back 8 bytes from file, verify it's 'aaaabbbb' */
    fd = aos_open(filename, O_RDONLY);
    if (fd  < 0) {
        LOGE(TAG, "fopen failed");
        return;
    }
    if (aos_read(fd, buf, 8) > 0) {
        LOGD(TAG, "read text ok, ");
    }
    if (strncmp(buf, "aaaabbbb", 8) == 0) {
        printf("[%d]verify ok...\n", __LINE__);
    } else {
        printf("verify error...\n");
    }
    aos_close(fd);

    /* Overwrite file with 'cccc' */
    test_spiffs_create_file_with_text(filename, "cccc");

    /* Verify file now only contains 'cccc' */
    fd = aos_open(filename, O_RDONLY);
    if (fd  < 0) {
        LOGE(TAG, "fopen failed");
        return;
    }
    if (aos_read(fd, buf, 4) > 0) {
        LOGD(TAG, "read text ok, ");
    }
    if (strncmp(buf, "cccc", 4) == 0) {
        printf("[%d]verify ok...\n", __LINE__);
    } else {
        printf("verify error...\n");
    }
    aos_close(fd);
}

static void test_spiffs_open_max_files(const char* filename_prefix, size_t files_count)
{
    printf("-------------------test_spiffs_open_max_files\n");
    int *fd = aos_calloc(files_count, sizeof(int));

    for (size_t i = 0; i < files_count; ++i) {
        char name[32];
        snprintf(name, sizeof(name), "%s_%d.txt", filename_prefix, i);
        fd[i] = aos_open(name, O_RDWR);
        if (fd  < 0) {
            printf("fopen failed, %s, %d\n", name, fd[i]);
        } else {
            printf("fopen ok, %s, %d\n", name, fd[i]);
        }
    }
    /* close everything and clean up */
    for (size_t i = 0; i < files_count; ++i) {
        aos_close(fd[i]);
    }
    aos_free(fd);
}

static void test_spiffs_lseek(const char* filename)
{
    printf("-------------------test_spiffs_lseek\n");
    int fd = aos_open(filename, O_CREAT | O_TRUNC | O_RDWR);
    if (fd  < 0) {
        printf("fopen failed [%s]\n", filename);
        return;
    }
    const char *text = "0123456789\n";
    char buf[128] = {0};
    aos_write(fd, text, 11);
    aos_lseek(fd, -2, SEEK_CUR);
    aos_read(fd, buf, 1);
    TEST_ASSERT_EQUAL("", '9', buf[0]);
    aos_lseek(fd, 3, SEEK_SET);
    aos_read(fd, buf, 1);
    TEST_ASSERT_EQUAL("", '3', buf[0]);
    aos_lseek(fd, -3, SEEK_END);
    aos_read(fd, buf, 1);
    TEST_ASSERT_EQUAL("", '8', buf[0]);
    aos_close(fd);
    printf("-------------------test_spiffs_lseek over\n");
}

static void test_spiffs_stat(const char* filename)
{
    printf("-------------------test_spiffs_stat\n");
    test_spiffs_create_file_with_text(filename, "foo\n");
    struct stat st;
    TEST_ASSERT_EQUAL("", 0, aos_stat(filename, &st));
    TEST_ASSERT("", (st.st_mode & S_IFREG));
    TEST_ASSERT_FALSE("", st.st_mode & S_IFDIR);
    printf("-------------------test_spiffs_stat over\n");
}

static void test_spiffs_unlink(const char* filename)
{
    printf("-------------------test_spiffs_unlink\n");
    test_spiffs_create_file_with_text(filename, "unlink\n");

    TEST_ASSERT_EQUAL("", 0, aos_unlink(filename));

    TEST_ASSERT_NEGATIVE("", aos_open(filename, O_RDWR));
    printf("-------------------test_spiffs_unlink over\n");
}

static void test_spiffs_rename(const char* filename_prefix)
{
    char name_dst[64];
    char name_src[64];

    printf("-------------------test_spiffs_rename\n");
    snprintf(name_dst, sizeof(name_dst), "%s_dst.txt", filename_prefix);
    snprintf(name_src, sizeof(name_src), "%s_src.txt", filename_prefix);

    aos_unlink(name_dst);
    aos_unlink(name_src);

    int fd = aos_open(name_src, O_CREAT | O_TRUNC | O_RDWR);
    TEST_ASSERT_POSITIVE("", fd);
    const char* str = "0123456789";
    aos_write(fd, str, strlen(str));
    aos_close(fd);
    TEST_ASSERT_0("", aos_rename(name_src, name_dst));
    fd = aos_open(name_src, O_RDONLY);
    TEST_ASSERT_NEGATIVE("", fd);
    aos_close(fd);
    fd = aos_open(name_dst, O_RDONLY);
    TEST_ASSERT_POSITIVE("", fd);
    char buf[64] = {0};
    aos_read(fd, buf, strlen(str));
    if (strcmp(str, buf) != 0) {
        printf("%d\n", __LINE__);
    }
    aos_close(fd);
    printf("-------------------test_spiffs_rename over\n");
}

static void test_spiffs_dir(const char* path)
{
    char name_dir_file[64];
    const char * file_name = "test_opd.txt";

    printf("-------------------test_spiffs_dir\n");
    snprintf(name_dir_file, sizeof(name_dir_file), "%s/%s", path, file_name);
    aos_unlink(name_dir_file);
    test_spiffs_create_file_with_text(name_dir_file, "test opendir\n");
    aos_dir_t* dir = aos_opendir(path);
    TEST_ASSERT_NOT_NULL("", dir);
    bool found = false;
    while (true) {
        aos_dirent_t* de = aos_readdir(dir);
        if (!de) {
            break;
        }
        if (strcasecmp(de->d_name, file_name) == 0) {
            found = true;
            break;
        }
    }
    TEST_ASSERT("", found);
    TEST_ASSERT_EQUAL("", 0, aos_closedir(dir));
    aos_unlink(name_dir_file);
    printf("-------------------test_spiffs_dir over\n");
}

static void _spiffs_task(void *argv)
{
    int run = 1;
    char buf[64] = {0};
    char text[64] = {0};

    printf("_spiffs_task \n");
    vfs_init();
    if (vfs_spiffs_register("spiffs")) {
        printf("spiffs register failed.\n");
        return;
    }
    while (run) {
        printf("spiffs register ok \n");
        for (int i = 0; i < 10; i++) {
            snprintf(buf, sizeof(buf), "/spiffs/t_%d.txt", i);
            snprintf(text, sizeof(text), "i am a test file%d.", i);
            test_spiffs_create_file_with_text(buf, text);
        }
        test_spiffs_open_max_files("/spiffs/t", 4);
        test_spiffs_overwrite_append("/spiffs/test_2.txt");
        test_spiffs_lseek("/spiffs/test_3.txt");
        test_spiffs_stat("/spiffs/test_4.txt");
        test_spiffs_unlink("/spiffs/test_5.txt");
        test_spiffs_rename("/spiffs/trename_");
        test_spiffs_dir("/spiffs");
        run = 0;
    }
    printf("test over, all pass\n");
    vfs_spiffs_unregister();
}

void spiffs_test(void)
{
    aos_task_t task;

    aos_task_new_ext(&task, "spiffs_test", _spiffs_task, NULL,  8 * 1024, AOS_DEFAULT_APP_PRI + 13);
}
