/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#ifdef AOS_COMP_RAMFS
#include "ramfs.h"
#endif

static void libc_freertos_example(int argc, char **argv)
{
    (void)argc;
    (void)argv;
	int fd;
    int ret;
    char teststring = "1234";
    char readbuf[10];
	void *paddr = NULL;

	paddr = malloc(10);
	if(paddr == NULL){
		printf("libc_freertos: malloc fail!\r\n");
        return;
	}else{
		free(paddr);
		printf("libc_freertos: malloc OK!\r\n");
	}

	#ifdef AOS_COMP_RAMFS
    ramfs_register("/test");
    fd = open("/test/file1", O_RDWR);
    if(fd < 0){
        printf("libc_freertos: ramfs open fail!\r\n");
        return;
    }
    ret = write(fd, teststring, 5);
    if(ret < 0){
        printf("libc_freertos: ramfs write fail!\r\n");
        close(fd);
        return;
    }
    lseek(fd, 0, SEEK_SET);
    ret = read(fd, readbuf, 5);
    if(ret < 0){
        printf("libc_freertos: ramfs read fail!\r\n");
        close(fd);
        return;
    }
    if(strncmp(readbuf, teststring, 5)){
        printf("libc_freertos: ramfs test fail! readbuf:%s\r\n",readbuf);
    }else{
        printf("libc_freertos: ramfs test success!\r\n");
    }
    close(fd);
    ramfs_unregister("/test");
    #endif

    printf("libc_freertos comp test success!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(libc_freertos_example, libc_example, lib stub component base example)
#endif
