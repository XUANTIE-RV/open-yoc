#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <csi_core.h>
#include "aos/cli.h"

#include "drv/common.h"
#include <drv/rng.h>

void cvi_trng_test(int32_t argc, char **argv)
{
    uint32_t data[4] = {0};

    csi_rng_get_single_word(data);

    printf("data : %x\n", data[0]);

    csi_rng_get_multi_word(data, 4);

    for (int i = 0; i < 4; i++) {
        printf("i : %d, data : %x\n", i, data[i]);
    }
}
ALIOS_CLI_CMD_REGISTER(cvi_trng_test, trng, trng test);
