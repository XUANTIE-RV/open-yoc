
//#include <yoc_config.h>
#include "../yunit.h"

static int init(void)
{
    return 0;
}
static int cleanup(void)
{
    return 0;
}
static void setup(void)
{
}
static void teardown(void)
{
}

extern void api_fota_test_entry(yunit_test_suite_t *suite);

void test_fotaapi(void)
{
    yunit_test_suite_t *suite;
    suite = yunit_add_test_suite("fotaapi", init, cleanup, setup, teardown);

    /* api */
    api_fota_test_entry(suite);

}

