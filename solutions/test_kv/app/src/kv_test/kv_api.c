/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <app_config.h>

#include "../yunit.h"
#include "kvset.h"
#include "block.h"

#define TAG "kv_test"

kv_t kv;
char *test_key = "kv_key_test";
char *test_value = "kv_value_test";
int ret = -99;

static int kv_partition_erase(kv_t *kv, int pos, int size)
{
    return partition_erase(kv->handle, pos, 1);
}

static int kv_partition_write(kv_t *kv, int pos, void *data, int size)
{    
    return partition_write(kv->handle, pos, data, size);
}

static int kv_partition_read(kv_t *kv, int pos, void *data, int size)
{
    return partition_read(kv->handle, pos, data, size);
}

static flash_ops_t partition_ops = {
    .write = kv_partition_write,
    .erase = kv_partition_erase,
    .read =  kv_partition_read
};

static int _iter_list(kvnode_t *node, void *p)
{
    printf("%s: %s\n", \
            KVNODE_OFFSET2CACHE(node, head_offset),
            KVNODE_OFFSET2CACHE(node, value_offset)
    );
    return 0;
}

static void test_kv_init()
{ 
    LOGI(TAG, "kv_init api test");
    kv.handle = partition_open("kv");
    kv.ops = &partition_ops;

    partition_info_t *lp = hal_flash_get_info(kv.handle);
    aos_assert(lp);
    uint8_t *mem = (uint8_t *)(lp->start_addr + lp->base_addr);
    int block_size = lp->sector_size;
    int block_num  = lp->length / lp->sector_size;
    LOGI(TAG, "block_size = %d",block_size);
    LOGI(TAG, "block_num = %d",block_num);

    ret = kv_init(&kv, mem, 0, block_size);
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_init block_num1 ret=%d", ret, "YoC_KV_01");

    ret = kv_init(&kv, mem, 1, block_size);
    YUNIT_ASSERT_MSG_QA(ret==0, "kv_init block_num2 ret=%d", ret, "YoC_KV_02");

    ret = kv_init(&kv, mem, -1, block_size);
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_init block_num3 ret=%d", ret, "YoC_KV_03");

    ret = kv_init(&kv, mem, block_num, 1);
    YUNIT_ASSERT_MSG_QA(ret==0, "kv_init block_size1 ret=%d", ret, "YoC_KV_04");

    ret =  kv_init(&kv, mem, block_num, block_size);
    YUNIT_ASSERT_MSG_QA(ret==0, "kv_init success ret=%d", ret, "YoC_KV_05");
}
static void test_kv_set()
{
    LOGI(TAG, "kv_set api test");
    int len_max = 497-strlen(test_key);
    ret = kv_set(&kv, test_key, test_value, strlen(test_value));
    YUNIT_ASSERT_MSG_QA(ret == strlen(test_value), "kv_set success ret=%d", ret, "YoC_KV_06");

    ret = kv_set(&kv, NULL, test_value, strlen(test_value));
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_set key ret=%d", ret, "YoC_KV_07");

    ret = kv_set(&kv, test_key, NULL, strlen(test_value));
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_set value ret=%d", ret, "YoC_KV_08");

    //size != strlen(value)
    ret = kv_set(&kv, test_key, test_value, 0);
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_set size1 ret=%d", ret, "YoC_KV_09");

    ret = kv_set(&kv, test_key, test_value, -1);
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_set size2 ret=%d", ret, "YoC_KV_10");

    //testcase: size + strlen(key) > 497
    ret = kv_set(&kv, test_key, test_value, len_max+1);
    YUNIT_ASSERT_MSG_QA(ret==-1, "kv_set size3 ret=%d", ret, "YoC_KV_11");
}

static void test_kv_get()
{
    LOGI(TAG, "kv_get api test");
    int len_value;
    len_value = strlen(test_value);
    int data_len = len_value + 1;
    char out_data[data_len];

    memset(out_data, 0, sizeof(out_data));
    ret = kv_get(&kv, test_key, out_data, sizeof(out_data));
    YUNIT_ASSERT_MSG_QA(ret == len_value && (strcmp(test_value, out_data) == 0), "kv_set success ret=%d", ret, "YoC_KV_12");
    
    ret = kv_get(&kv, NULL, out_data, sizeof(out_data));
    YUNIT_ASSERT_MSG_QA(ret == -1, "kv_get key ret=%d", ret, "YoC_KV_13");

    ret = kv_get(&kv, "key_err", out_data, sizeof(out_data));
    YUNIT_ASSERT_MSG_QA(ret == -1, "kv_get key1 ret=%d", ret, "YoC_KV_14");

    memset(out_data, 0, sizeof(out_data));
    ret = kv_get(&kv, test_key, NULL, sizeof(out_data));
    YUNIT_ASSERT_MSG_QA(ret == 13, "kv_get value ret=%d", ret, "YoC_KV_15");

    ret = kv_get(&kv, test_key, out_data, 0);
    YUNIT_ASSERT_MSG_QA(ret == 13, "kv_set size2 ret=%d", ret, "YoC_KV_16");

    //size > value size, returan value size
    memset(out_data, 0, sizeof(out_data));
    ret = kv_get(&kv, test_key, out_data, len_value + 10);
    YUNIT_ASSERT_MSG_QA(ret == len_value && (strcmp(test_value, out_data) == 0), "kv_set size3 ret=%d", ret, "YoC_KV_17");

    //size < value size,return size
    memset(out_data, 0, sizeof(out_data));
    ret = kv_get(&kv, test_key, out_data, len_value);
    YUNIT_ASSERT_MSG_QA(ret == len_value, "kv_set size4 ret=%d", ret, "YoC_KV_18");
}

static void test_kv_find()
{
    LOGI(TAG, "kv_find api test");
    kvnode_t node;
    ret = kv_find(&kv, test_key, &node);
    YUNIT_ASSERT_MSG_QA(ret == 0, "kv_find success ret=%d", ret, "YoC_KV_19");
}

static void test_kv_iter()
{
    LOGI(TAG, "kv_iter api test");
    char out_data[40];
    ret = kv_iter(&kv, _iter_list, out_data);
    YUNIT_ASSERT_MSG_QA(ret == 0, "kv_iter data1 ret=%d", ret, "YoC_KV_23");
    
    ret = kv_iter(&kv, _iter_list, NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "kv_iter data2 ret=%d", ret, "YoC_KV_24");
}

static void test_kv_gc()
{
    LOGI(TAG, "kv_gc api test");
    ret = kv_gc(&kv);
    YUNIT_ASSERT_MSG_QA(ret == 0, "kv_gc success ret=%d", ret, "YoC_KV_25");
}

static void test_kv_rm()
{
    LOGI(TAG, "kv_rm api test");
    ret = kv_rm(&kv, test_key);
    YUNIT_ASSERT_MSG_QA(ret == 0, "kv_rm success ret=%d", ret, "YoC_KV_26");

    ret = kv_rm(&kv, "key_err");
    YUNIT_ASSERT_MSG_QA(ret == -1, "kv_rm key1 ret=%d", ret, "YoC_KV_27");

    ret = kv_rm(&kv, NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "kv_rm key2 ret=%d", ret, "YoC_KV_28");
}

static void test_kv_reset()
{
    LOGI(TAG, "kv_reset api test");
    ret = kv_reset(&kv);
    YUNIT_ASSERT_MSG_QA(ret == 0, "kv_reset success ret=%d", ret, "YoC_KV_29");
}

static void test_kv_api_param(void)
{
    test_kv_init();
    test_kv_set();
    test_kv_get();
    test_kv_find();
    test_kv_gc();
    test_kv_rm();
    test_kv_iter();
    test_kv_reset();

}

void api_kv_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "api_kv", test_kv_api_param);
}
