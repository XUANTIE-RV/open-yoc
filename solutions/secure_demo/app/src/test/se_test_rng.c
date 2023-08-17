#include <string.h>
#include "se_device.h"
#include "se_rng.h"
#include "yunit.h"


static void se_test_rng_demo(void)
{

    uint8_t data[32] = {0x00}; 
    int ret = 0;

    ret = se_rng_get_bytes(data, sizeof(data));
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_rng_get_bytes test get 32bytes is %d", ret);

    #ifdef SE_TEST_LOG
    printf("random:");
    for (i = 0; i < sizeof(data); i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("0x%x ",data[i]);
    }
    printf("\r\n");
    #endif
    
    ret = se_rng_get_bytes(NULL, sizeof(data));
    YUNIT_ASSERT_MSG_QA(ret != 0, "the se_rng_get_bytes test data is NULL is %d", ret);
}

void se_test_rng(void)
{
    /***************RNG TEST START****************/

    add_yunit_test("rng", &se_test_rng_demo);

    /***************RNG TEST END****************/
}

