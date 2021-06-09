#include "aos/hal/rng.h"
#include "lstrng.h"
#include "platform.h"
#include <string.h>

int32_t hal_random_num_read(random_dev_t random, void *buf, int32_t bytes)
{
    uint32_t rng_t;
    if(buf==NULL) {
        return -1;
    }

    lstrng_init();
    rng_t = lstrng_random();
    memcpy(buf,&rng_t,bytes);    

    return 0;
}