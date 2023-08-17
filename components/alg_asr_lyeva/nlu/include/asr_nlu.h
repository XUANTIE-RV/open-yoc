#ifndef __NLU_TEST_H_
#define __NLU_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

int asr_nlu_init();
int asr_nlu_deinit();
int asr_nlu_get(const char *asr_result, char *nlu_reslut, size_t nlu_reslut_size);

#ifdef __cplusplus
}
#endif

#endif