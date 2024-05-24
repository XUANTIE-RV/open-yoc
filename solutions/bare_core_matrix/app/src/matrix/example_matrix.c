/*
 * Copyright (C) 2019-2024 Alibaba Group Holding Limited
 */

#ifdef __riscv_matrix
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

extern int matrix_main(int argc, char **argv);
int example_core_matrix()
{
    int rc;

    rc = matrix_main(0, NULL);
    if (rc == 0)
        printf("matrix runs successfully!\n");
    else
        printf("matrix runs fail!\n");

    return rc;
}
#endif


