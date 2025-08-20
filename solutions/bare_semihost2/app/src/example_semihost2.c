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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semihost2/semihost.h>
#include "board.h"

#ifndef SEMI_CHECK_RET_WITH_GOTO
#define SEMI_CHECK_RET_WITH_GOTO(x, label) \
	do { \
		if (!(x)) { \
			printf("func[%s], line[%d] fail.\r\n", __FUNCTION__, __LINE__); \
			goto label; \
		}\
	} while (0)
#endif

int example_semihost2(void)
{
    long read, fd, rc;
    uint8_t r_buffer[16];
    uint8_t w_buffer[16] = { 1, 2, 3, 4, 5 };
    const char *test_file = "./semihost_test.txt";

    /* Redirect stdout and stderr to the debug host for semihost. */
    semihost_open(":tt", SEMIHOST_OPEN_W_PLUS);
    semihost_open(":tt", SEMIHOST_OPEN_A_PLUS);

    semihost_printf("output by semihost_printf\n");
    semihost_printf2("output by semihost_printf2\n");
    rc = semihost_write(STDOUT_FILENO, "output by semihost_write\n", 25);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
    rc = semihost_write(STDERR_FILENO, "output1 by semihost_write\n", 26);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    /* Open in write mode */
    fd = semihost_open(test_file, SEMIHOST_OPEN_WB);
    SEMI_CHECK_RET_WITH_GOTO(fd > 0, error);
    rc = semihost_flen(fd);
    if (rc) {
        printf("File may be not empty, rc = %d\r\n", (int)rc);
        goto error;
    }

    /* Write some data */
    rc = semihost_write(fd, w_buffer, sizeof(w_buffer));
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
    rc = semihost_flen(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == sizeof(w_buffer), error);
    rc = semihost_write(fd, w_buffer, sizeof(w_buffer));
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
    rc = semihost_flen(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == 2 * sizeof(w_buffer), error);

    /* Reading should fail in this mode */
    read = semihost_read(fd, r_buffer, sizeof(r_buffer));
    SEMI_CHECK_RET_WITH_GOTO(read < 0, error);

    /* Close the file */
    rc = semihost_close(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    /* Open the same file again for reading */
    fd = semihost_open(test_file, SEMIHOST_OPEN_RB);
    SEMI_CHECK_RET_WITH_GOTO(fd > 0, error);
    rc = semihost_flen(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == 2 * sizeof(w_buffer), error);

    /* Check reading data */
    read = semihost_read(fd, r_buffer, sizeof(r_buffer));
    SEMI_CHECK_RET_WITH_GOTO(read == sizeof(r_buffer), error);
    rc = memcmp(r_buffer, w_buffer, sizeof(r_buffer));
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
    read = semihost_read(fd, r_buffer, sizeof(r_buffer));
    SEMI_CHECK_RET_WITH_GOTO(read == sizeof(r_buffer), error);
    rc = memcmp(r_buffer, w_buffer, sizeof(r_buffer));
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    /* Read past end of file */
    read = semihost_read(fd, r_buffer, sizeof(r_buffer));
    SEMI_CHECK_RET_WITH_GOTO(read == -EIO, error);

    /* Seek to file offset */
    rc = semihost_seek(fd, 1);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    /* Read from offset */
    read = semihost_read(fd, r_buffer, sizeof(r_buffer) - 1);
    SEMI_CHECK_RET_WITH_GOTO(read == sizeof(r_buffer) - 1, error);
    rc = memcmp(r_buffer, w_buffer + 1, sizeof(r_buffer) - 1);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    /* Close the file */
    rc = semihost_close(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    /* Opening again in write mode should erase the file */
    fd = semihost_open(test_file, SEMIHOST_OPEN_WB);
    SEMI_CHECK_RET_WITH_GOTO(fd > 0, error);
    rc = semihost_flen(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);
    rc = semihost_close(fd);
    SEMI_CHECK_RET_WITH_GOTO(rc == 0, error);

    printf("bare_semihost2 demo runs successfully!\n");
    return 0;
error:
    printf("bare_semihost2 demo runs fail!\n");
    return -1;
}
