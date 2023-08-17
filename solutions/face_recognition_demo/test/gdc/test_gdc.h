#ifndef __TEST_GDC_H__
#define __TEST_GDC_H__
#if (CONFIG_APP_TEST == 1)
void test_gdc_begain_job(int32_t argc, char **argv);
void test_gdc_enable_rot(int32_t argc, char **argv);
void test_gdc_enable_ldc(int32_t argc, char **argv);
void gdc_gen_mesh(int32_t argc, char **argv);
#endif
#endif      /* __TEST_GDC_H__ */
