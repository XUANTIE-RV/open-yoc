/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _NLU_H_
#define _NLU_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CONFIG_NLU_VERSION
#define CONFIG_NLU_VERSION "1.0.0"
#endif

#define MAX_SLOTS_TAG_LEN 20
#define MAX_SLOTS_NUM 20
#define FEATURE_SELECT_MIN_NUM 4
#define FEATURE_SELECT_RATE 0.75

   typedef enum
   {
      SLOT_TIME = 0,
      SLOT_NUM = 1,
      SLOT_MAJOR = 2,
      SLOT_ACTION = 3,
      SLOT_FUN = 4,
      SLOT_DIRECTION = 5,
      SLOT_ADVERBIAL = 6,
   } nlu_major_slots_en;

   typedef struct
   {
      nlu_major_slots_en major_slot;
      int offset;
      int length;
      char key_words[MAX_SLOTS_TAG_LEN];
   } nlu_sub_slots_result_t;

   typedef struct
   {
      int size;
      nlu_sub_slots_result_t sub_slots[MAX_SLOTS_NUM];
   } nlu_slots_result_t;

   int nlu_init();
   int nlu_deinit();
   int nlu_intention_get(const char *text, int *domain_lable, int *intention_lable, double *score);
   int nlu_intention_get_and_slots_fill(const char *text, int *domain_lable, int *intention_lable, double *score, nlu_slots_result_t *slots);
   int nlu_intention_slots_to_json_str(char *text, int domain_result, int intention_result, nlu_slots_result_t *slots_result, char *json_str, int json_str_buffer_size);
   const char *nlu_intention_str(int intention);
   const char *nlu_major_slots_str(nlu_major_slots_en slots);

#ifdef __cplusplus
}
#endif

#endif