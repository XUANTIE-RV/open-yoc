#include "yunit.h"

extern void test_ble_sdk_mesh(void);

void yunit_test_api()
{
/*common api test*/
    yunit_test_init();
    test_ble_sdk_mesh();
}