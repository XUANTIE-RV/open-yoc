#include <string.h>
#include "se_device.h"
#include "se_sha.h"
#include "yunit.h"

typedef struct{
se_sha_t          	sha;
uint8_t 			*msg;
uint32_t 			msg_len;
uint8_t 			*result;
uint32_t 			update_times;
uint32_t 			case_n;
uint32_t            mode;
}SHA_TEST_S;
#define MSG_INPUT_LEN 	1000
static uint8_t case_n = 0;
static uint8_t msg_a[MSG_INPUT_LEN];
// 1000*1000a
static const uint8_t sum_result_a[] = {
	0xcd,0xc7,0x6e,0x5c,0x99,0x14,0xfb,0x92,
	0x81,0xa1,0xc7,0xe2,0x84,0xd7,0x3e,0x67,
	0xf1,0x80,0x9a,0x48,0xa4,0x97,0x20,0xe,
	0x4,0x6d,0x39,0xcc,0xc7,0x11,0x2c,0xd0
};

static int se_sha_demo_single(SHA_TEST_S *sShaTestData)
{
	uint32_t sum[64];
	uint32_t len;
	int ret;

	memset(sum, 0x00, sizeof(sum));
	ret = se_sha_init(&sShaTestData->sha);
	CHECK_RET_WITH_RET(ret == 0, ret);

	ret = se_sha_start(&sShaTestData->sha, sShaTestData->mode);
	CHECK_RET_WITH_RET(ret == 0, ret);

	for (int i = 0; i < sShaTestData->update_times; i++) {
		ret = se_sha_update(&sShaTestData->sha,  sShaTestData->msg, sShaTestData->msg_len);
		CHECK_RET_WITH_RET(ret == 0, ret);
	}
	ret = se_sha_finish(&sShaTestData->sha,  sum, &len);
	CHECK_RET_WITH_RET(ret == 0, ret);
	se_sha_uninit(&sShaTestData->sha);
	ret = memcmp(sum, sShaTestData->result, len);
	if (ret != 0) {
		printf("case%d, hash_len = %d, update %dtime, failed!\n", sShaTestData->case_n,  sShaTestData->msg_len, sShaTestData->update_times);
	}

	return ret;
}

static void se_sha256_singlethread_demo(void)
{
    int ret;
	SHA_TEST_S sSha256TestData;

	//110*5len result
	const uint8_t sum_result[] = {
		0xdc, 0x9a, 0xb2, 0x18, 0x7b, 0xf4, 0x2a, 0xf7, 
		0x56, 0xba, 0x2e, 0xe7, 0xc1, 0x73, 0x69, 0x02, 
		0xb9, 0x7f, 0x66, 0xa6, 0x3f, 0xb6, 0x52, 0xed, 
		0x39, 0xba, 0xba, 0x35, 0x58, 0xa5, 0x3e, 0x88, 
	};
	//8*64len result
	const uint8_t sum_result_1[] = {
		0x47, 0x1b, 0xe6, 0x55, 0x8b, 0x66, 0x5e, 0x4f, 
		0x6d, 0xd4, 0x9f, 0x11, 0x84, 0x81, 0x4d, 0x14, 
		0x91, 0xb0, 0x31, 0x5d, 0x46, 0x6b, 0xee, 0xa7, 
		0x68, 0xc1, 0x53, 0xcc, 0x55, 0x00, 0xc8, 0x36, 
	};
	//100*75len result
	const uint8_t sum_result_2[] = {
		0xa8, 0x3d, 0xca, 0xef, 0x7e, 0x05, 0x83, 0xa2, 
		0xb7, 0x0e, 0xaa, 0xca, 0x62, 0x69, 0x94, 0x79, 
		0x59, 0xac, 0x54, 0xa2, 0x4e, 0x9b, 0xbb, 0x54, 
		0x4c, 0x4d, 0x15, 0xd7, 0xc0, 0x1b, 0xed, 0x9f,
	};
	//10*128len result
	const uint8_t sum_result_3[] = {
		0x31, 0x2d, 0xe4, 0x6d, 0x40, 0xe6, 0x4f, 0xda, 
		0x53, 0x92, 0xd6, 0x7c, 0x5e, 0x2a, 0x3d, 0x0b, 
		0x2d, 0x9b, 0xaa, 0x74, 0xa2, 0x28, 0xcf, 0x8b, 
		0x0a, 0x69, 0x56, 0x6f, 0x7b, 0xb8, 0x94, 0x5a,
	};

    case_n = 0;
	sSha256TestData.case_n = (++case_n);
	sSha256TestData.mode = SE_SHA_MODE_256;
	sSha256TestData.msg = (uint8_t *)msg_a;
	sSha256TestData.msg_len = 5;
	sSha256TestData.result = (uint8_t * )sum_result;
	sSha256TestData.update_times = 110;
	ret = se_sha_demo_single(&sSha256TestData);
    YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha256_singlethread_demo test case 5*110update is %d", ret);

	sSha256TestData.case_n = (++case_n);
	sSha256TestData.mode = SE_SHA_MODE_256;
	sSha256TestData.msg = (uint8_t *)msg_a;
	sSha256TestData.msg_len = 64;
	sSha256TestData.result = (uint8_t * )sum_result_1;
	sSha256TestData.update_times = 8;
	ret = se_sha_demo_single(&sSha256TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha256_singlethread_demo test case 64*8update is %d", ret);

	sSha256TestData.case_n = (++case_n);
	sSha256TestData.mode = SE_SHA_MODE_256;
	sSha256TestData.msg = (uint8_t *)msg_a;
	sSha256TestData.msg_len = 75;
	sSha256TestData.result = (uint8_t * )sum_result_2;
	sSha256TestData.update_times = 100;
	ret = se_sha_demo_single(&sSha256TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha256_singlethread_demo test case 75*100update is %d", ret);

	sSha256TestData.case_n = (++case_n);
	sSha256TestData.mode = SE_SHA_MODE_256;
	sSha256TestData.msg = (uint8_t *)msg_a;
	sSha256TestData.msg_len = 128;
	sSha256TestData.result = (uint8_t * )sum_result_3;
	sSha256TestData.update_times = 10;
	ret = se_sha_demo_single(&sSha256TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha256_singlethread_demo test case 128*10update is %d", ret);

	sSha256TestData.case_n = (++case_n);
	sSha256TestData.mode = SE_SHA_MODE_256;
	sSha256TestData.msg = (uint8_t *)msg_a;
	sSha256TestData.msg_len = 1000;
	sSha256TestData.result = (uint8_t * )sum_result_a;
	sSha256TestData.update_times = 1000;
	ret = se_sha_demo_single(&sSha256TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha256_singlethread_demo test case 1000*1000update is %d", ret);

	sSha256TestData.case_n = (++case_n);
	sSha256TestData.mode = SE_SHA_MODE_256;
	sSha256TestData.msg = (uint8_t *)msg_a;
	sSha256TestData.msg_len = 100;
	sSha256TestData.result = (uint8_t * )sum_result_a;
	sSha256TestData.update_times = 10000;
	ret = se_sha_demo_single(&sSha256TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha256_singlethread_demo test case 100*10000update is %d", ret);

}


static void se_sha1_singlethread_demo(void)
{
    int ret;
	SHA_TEST_S sSha1TestData;

	//11*64len
	const uint8_t sum_result[] = {
		0xef, 0xb8, 0xf8, 0xbb, 0x73, 0x13, 0xe6, 0x7a, 
		0x60, 0x63, 0x26, 0x49, 0x54, 0xca, 0x09, 0x43, 
		0x18, 0xec, 0x12, 0x9e,
	};
	//1000*1len
	const uint8_t sum_result_1[] = {
		0x29, 0x1e, 0x9a, 0x6c, 0x66, 0x99, 0x49, 0x49, 
		0xb5, 0x7b, 0xa5, 0xe6, 0x50, 0x36, 0x1e, 0x98, 
		0xfc, 0x36, 0xb1, 0xba,
	};

	//10000*100len
	const uint8_t sum_result_2[] = {
		0x34, 0xaa, 0x97, 0x3c, 0xd4, 0xc4, 0xda, 0xa4, 
		0xf6, 0x1e, 0xeb, 0x2b, 0xdb, 0xad, 0x27, 0x31, 
		0x65, 0x34, 0x01, 0x6f,
	};

    case_n = 0;
	sSha1TestData.case_n = (++case_n);
	sSha1TestData.mode = SE_SHA_MODE_1;
	sSha1TestData.msg = (uint8_t *)msg_a;
	sSha1TestData.msg_len = 64;
	sSha1TestData.result = (uint8_t * )sum_result;
	sSha1TestData.update_times = 11;
	ret = se_sha_demo_single(&sSha1TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha1_singlethread_demo test case 64*11update is %d", ret);

	sSha1TestData.case_n = (++case_n);
	sSha1TestData.mode = SE_SHA_MODE_1;
	sSha1TestData.msg = (uint8_t *)msg_a;
	sSha1TestData.msg_len = 1;
	sSha1TestData.result = (uint8_t * )sum_result_1;
	sSha1TestData.update_times = 1000;
	ret = se_sha_demo_single(&sSha1TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha1_singlethread_demo test case 1*1000update is %d", ret);

	sSha1TestData.case_n = (++case_n);
	sSha1TestData.mode = SE_SHA_MODE_1;
	sSha1TestData.msg = (uint8_t *)msg_a;
	sSha1TestData.msg_len = 100;
	sSha1TestData.result = (uint8_t * )sum_result_1;
	sSha1TestData.update_times = 10;
	ret = se_sha_demo_single(&sSha1TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha1_singlethread_demo test case 100*10update is %d", ret);

	sSha1TestData.case_n = (++case_n);
	sSha1TestData.mode = SE_SHA_MODE_1;
	sSha1TestData.msg = (uint8_t *)msg_a;
	sSha1TestData.msg_len = 100;
	sSha1TestData.result = (uint8_t * )sum_result_2;
	sSha1TestData.update_times = 10000;
	ret = se_sha_demo_single(&sSha1TestData);
	YUNIT_ASSERT_MSG_QA(ret == 0, "the se_sha1_singlethread_demo test case 100*10000update is %d", ret);

}


void se_test_sha(void)
{

    memset(msg_a, 'a', MSG_INPUT_LEN);

    /***************SHA TEST START****************/
    
    add_yunit_test("sha256", &se_sha256_singlethread_demo);

    add_yunit_test("sha1", &se_sha1_singlethread_demo);

    /***************SHA TEST END****************/

}