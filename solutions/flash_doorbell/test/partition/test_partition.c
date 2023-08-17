#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aos/cli.h"
#include "vfs.h"
#include "yoc/partition.h"
#include "yoc/partition_flash.h"

static int cli_update_norflash_by_name(int argc, char** argv) {
    int ret = 0;
    char fileName[32] = {0};
    char writeBuf[4096] = {0};
    int writeLen = 0;
    int readFd = 0;
    partition_t partitionFd = 0;
    partition_info_t* partitionInfo = NULL;

    if (argc != 3) {
        printf("update_by_name [partition Name] [file name]\n");
        return 0;
    }

    if (strlen(argv[2]) > (sizeof(fileName) - 1)) {
        printf("[file name] is too length.\n");
        return 0;
    }
    memcpy(fileName, argv[2], strlen(argv[2]));
    ret = aos_access(fileName, F_OK);
    if (ret != 0) {
        printf("can't find %s.\n", fileName);
        return 0;
    }
    printf("aos_access find %s ...\n", fileName);

    readFd = aos_open(fileName, O_RDWR);
    if (readFd < 0) {
        printf("open %s failed!\n", fileName);
        ret = -1;
        goto UPDATE_ERR1;
    }
    printf("aos_openn open %s success!\n", fileName);

    partitionFd = partition_open(argv[1]);
    if (partitionFd < 0) {
        printf("partition_open %s failed! ret = %d.\n", argv[1], partitionFd);
        ret = -1;
        goto UPDATE_ERR2;
    }
    printf("partition_open %s success.\n", argv[1]);
    partitionInfo = partition_info_get(partitionFd);
    if (partitionInfo == NULL) {
        printf("partition_info_get partitionFd[%d] failed!\n", partitionFd);
        goto UPDATE_ERR3;
    }
    printf("partition_info_get partitionFd[%d] success.\n", partitionFd);

    ret = partition_erase(partitionFd, 0x0,
                          partitionInfo->length / partitionInfo->sector_size);
    if (ret != 0) {
        printf("partition_erase block %d failed.\n",
               partitionInfo->length / partitionInfo->sector_size);
        goto UPDATE_ERR3;
    }
    printf("partition_erase success.\n");

    off_t tmpOffset = 0;
    while (1) {
        writeLen = aos_read(readFd, writeBuf, sizeof(writeBuf));
        if (writeLen <= 0) {
            printf("read end. And write success.\n");
            break;
        }
        ret = partition_write(partitionFd, tmpOffset, writeBuf, writeLen);
        if (ret < 0) {
            printf("partition_write error!ret = %d.\n", ret);
            goto UPDATE_ERR3;
        }
        tmpOffset += writeLen;
    }
    printf("write 0x%x to 0x%lx , total 0x%lx\n", partitionInfo->start_addr,
           tmpOffset, tmpOffset);
    printf("update %s to partition [%s] success.\n", fileName, argv[1]);

UPDATE_ERR3:
    partition_close(partitionFd);
UPDATE_ERR2:
    aos_close(readFd);
UPDATE_ERR1:
    return ret;
}
ALIOS_CLI_CMD_REGISTER(cli_update_norflash_by_name,
                       update_by_name,
                       update cvitek norflash by partition name);

static int cli_update_partition_flash(int argc, char** argv) {
    int ret = 0;
    char fileName[32] = {0};
    char writeBuf[4096] = {0};
    int writeLen = 0;
    int readFd = 0;
    void* partitionFd = NULL;
    aos_stat_t fileStat = {0};

    if (argc != 3) {
        printf("update_flash [start addr] [file name]\n");
        return 0;
    }

    if (strlen(argv[2]) > (sizeof(fileName) - 1)) {
        printf("[file name] is too length.\n");
        return 0;
    }
    memcpy(fileName, argv[2], strlen(argv[2]));
    ret = aos_access(fileName, F_OK);
    if (ret != 0) {
        printf("can't find %s.\n", fileName);
        return 0;
    }
    printf("aos_access find %s ...\n", fileName);

    ret = aos_stat(fileName, &fileStat);
    if (ret != 0) {
        printf("can't get file stat.ret = %d\n", ret);
        return 0;
    }
    writeLen = fileStat.st_size;
    printf("file [%s] length is 0x%x\n", fileName, writeLen);

    readFd = aos_open(fileName, O_RDWR);
    if (readFd < 0) {
        printf("open %s failed!\n", fileName);
        return -1;
    }
    printf("aos_openn open %s success!\n", fileName);

    partitionFd = partition_flash_open(0);
    if (partitionFd == NULL) {
        printf("partition_open %s failed!\n", argv[1]);
        aos_close(readFd);
        return -1;
    }
    printf("partition_flash_open success.\n");

    unsigned long tmpOffset = strtol(argv[1], NULL, 16);
    printf("offset = 0x%lx\n", tmpOffset);

    ret = partition_flash_erase(partitionFd, tmpOffset, writeLen);
    if (ret != 0) {
        printf("partition_flash_erase start Addr 0x%lx leng 0x%x failed.\n",
               tmpOffset, writeLen);
        partition_flash_close(partitionFd);
        aos_close(readFd);
        return -1;
    }
    printf("partition_flash_erase start Addr 0x%lx leng 0x%x success.\n",
           tmpOffset, writeLen);

    while (1) {
        writeLen = aos_read(readFd, writeBuf, sizeof(writeBuf));
        if (writeLen <= 0) {
            printf("read end. And write success.\n");
            break;
        }
        ret = partition_flash_write(partitionFd, tmpOffset, writeBuf, writeLen);
        if (ret < 0) {
            printf("partition_write error!ret = %d.\n", ret);
            partition_flash_close(partitionFd);
            aos_close(readFd);
            return -1;
        }
        tmpOffset += writeLen;
    }
    printf("write %s to 0x%lx , total 0x%lx\n", argv[1], tmpOffset, tmpOffset);
    printf("update %s success.\n", fileName);

    partition_flash_close(partitionFd);
    aos_close(readFd);
    return ret;
}
ALIOS_CLI_CMD_REGISTER(cli_update_partition_flash,
                       update_flash,
                       update cvitek norflash by partition flash);

static int cli_read_partition(int argc, char** argv) {
    if (argc != 3) {
        printf("read_partition [part name] [output]\n");
        return 0;
    }
    int sdFd = 0, ret = 0;
    partition_t partFd = 0;
    partition_info_t* partitionInfo = NULL;
    char readBuf[1024] = {0};

    sdFd = aos_open(argv[2], O_CREAT | O_RDWR | O_TRUNC);
    if (sdFd < 0) {
        printf("aos_open %s fialed!\n", argv[2]);
        return -1;
    }

    partFd = partition_open(argv[1]);
    if (partFd < 0) {
        printf("partition_open %s failed!\n", argv[1]);
        ret = partFd;
        goto READ_PART1;
    }
    partitionInfo = partition_info_get(partFd);
    if (partitionInfo == NULL) {
        printf("partition_info_get failed!\n");
        goto READ_PART2;
    }

    off_t tmpOffset = 0;
    while (1) {
        memset(readBuf, 0, sizeof(readBuf));
        ret = partition_read(partFd, tmpOffset, readBuf, sizeof(readBuf));
        if (ret < 0) {
            printf("[%d]read end.\n", __LINE__);
            break;
        }

        ret = aos_write(sdFd, readBuf, sizeof(readBuf));
        if (ret < 0) {
            printf("aos_write failed! ret = %d\n", ret);
            goto READ_PART2;
        } else if (ret == 0) {
            printf("aos_write end!ret = %d\n", ret);
            ret = 0;
            break;
        }
        tmpOffset += sizeof(readBuf);
        printf("read : %d %% \n" , (int)(((float)tmpOffset / (float)(partitionInfo->length))*100));
        if (tmpOffset >= partitionInfo->length) {
            printf("[%d]read end.offset = %ld , leng = %d\n", __LINE__,
                   tmpOffset, partitionInfo->length);
            ret = 0;
            break;
        }
    }
READ_PART2:
    partition_close(partFd);
READ_PART1:
    aos_close(sdFd);
    return ret;
}
ALIOS_CLI_CMD_REGISTER(cli_read_partition,
                       read_partition,
                       read partition to sd - card);

static int cli_erase_partition(int argc, char** argv) {
    if (argc != 2) {
        printf("erase_partition [partition name]\n");
        return 0;
    }
    partition_t partFd = 0;
    partition_info_t* partitionInfo = NULL;

    partFd = partition_open(argv[1]);
    if (partFd < 0) {
        printf("partition_open %s failed!\n", argv[1]);
        return -1;
    }
    partitionInfo = partition_info_get(partFd);
    if (partitionInfo == NULL) {
        printf("partition_info_get failed!\n");
        partition_close(partFd);
        return -1;
    }

    int ret = partition_erase(
        partFd, 0, partitionInfo->length / partitionInfo->sector_size);
    if (ret != 0) {
        printf("partition_erase failed!\n");
    }
    partition_close(partFd);
    return ret;
}
ALIOS_CLI_CMD_REGISTER(cli_erase_partition, erase_partition, erase partition);