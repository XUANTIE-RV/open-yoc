 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

static void libc_stub_example(int argc, char **argv)
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
		printf("libc_stub: malloc fail!\r\n");
        return;
	}else{
		free(paddr);
		printf("libc_stub: malloc OK!\r\n");
	}

	#ifdef AOS_COMP_RAMFS
    ramfs_register("/test");
    fd = open("/test/file1", O_RDWR);
    if(fd < 0){
        printf("libc_stub: ramfs open fail!\r\n");
        return;
    }
    ret = write(fd, teststring, 5);
    if(ret < 0){
        printf("libc_stub: ramfs write fail!\r\n");
        close(fd);
        return;
    }
    lseek(fd, 0, SEEK_SET);
    ret = read(fd, readbuf, 5);
    if(ret < 0){
        printf("libc_stub: ramfs read fail!\r\n");
        close(fd);
        return;
    }
    if(strncmp(readbuf, teststring, 5)){
        printf("libc_stub: ramfs test fail! readbuf:%s\r\n",readbuf);
    }else{
        printf("libc_stub: ramfs test success!\r\n");
    }
    close(fd);
    ramfs_unregister("/test");
    #endif

    printf("libc_stub comp test success!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(libc_stub_example, libc_example, lib stub component base example)
#endif
