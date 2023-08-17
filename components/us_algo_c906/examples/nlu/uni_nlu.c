/**
 * Copyright (C) 2023 Unisound.inc, All rights reserved.
 *
 * @file uni_nlu.c
 * @brief 本地nlu源文件
 * @version 0.1
 */

#include "uni_nlu.h"
#include "uni_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <ulog/ulog.h>

#include "uni_nlu_content.h"
#include "cJSON.h"

#define TAG "nlu"
#define NLU_FORMAT "{\"header\":{\"namespace\":\"NluModel\",\"version\":\"1.0.0\"}, \"payload\":{}}"


static void _replace_asr_recongize(cJSON *reslut, const char *word) {
  cJSON *rec_node = cJSON_CreateString(word);
  if (reslut) {
    if (cJSON_GetObjectItem(reslut, "asrresult")) {
      cJSON_ReplaceItemInObject(reslut, "asrresult", rec_node);
    } else {
      cJSON_AddItemToObject(reslut, "asrresult", rec_node);
    }
  }
}

static cJSON *_replace_nlu_payload(cJSON *content) {
  cJSON *nlu = cJSON_Parse(NLU_FORMAT);
  if (nlu) {
    cJSON_ReplaceItemInObject(nlu, "payload", content);
  }
  return nlu;
}

//TODO perf sort g_nlu_content_mapping by hashcode O(logN), now version O(N)
char* NluParseLasr(const char *asr_json, float *score) {
  cJSON *result = NULL;
  cJSON *nlu = NULL;
  char *nlu_str = NULL;
  cJSON *cmd_item = NULL;
  cJSON *score_item = NULL;
  cJSON *asr = cJSON_Parse(asr_json);
  unsigned int  hashCode;

  if (asr == NULL) {
    LOGE(TAG, "asr rst parse fail");
    return NULL;
  }

  score_item = cJSON_GetObjectItem(asr, "score");
  if(score_item != NULL) {
    *score = atoi(score_item->valuestring);
  }

  cmd_item = cJSON_GetObjectItem(asr, "cmd");
  if (cmd_item == NULL) {
    cJSON_Delete(asr);
    LOGE(TAG, "not get cmd item");
    return NULL;
  }

  hashCode = uni_hash(cmd_item->valuestring);


  int hashTableSize = sizeof(g_nlu_content_mapping) / sizeof(g_nlu_content_mapping[0]);
  int i;

  for (i = 0; i < hashTableSize; i++) {
    /* find same hashcode as keyword's */
    if (hashCode == g_nlu_content_mapping[i].key_word_hash_code) {
      LOGI(TAG, "found map %d", i);
      /* return immediately when no hash collision */
      if (NULL == g_nlu_content_mapping[i].hash_collision_orginal_str) {
        LOGI(TAG, "found result %s", g_nlu_content_str[g_nlu_content_mapping[i].nlu_content_str_index]);
        result = cJSON_Parse(g_nlu_content_str[g_nlu_content_mapping[i].nlu_content_str_index]);
        if (result == NULL) {
          LOGE(TAG, "json parse failed, memory may not enough, reboot now");
          return NULL;
        }
        _replace_asr_recongize(result, cmd_item->valuestring);
        nlu = _replace_nlu_payload(result);
        if (nlu) {
          nlu_str = cJSON_PrintUnformatted(nlu);
          cJSON_Delete(nlu);
        } else {
          cJSON_Delete(result);
        }
        cJSON_Delete(asr);
        return nlu_str;
      }
      /* return when key_word equals hash_collision_orginal_str */
      if (0 == strcmp(cmd_item->valuestring, g_nlu_content_mapping[i].hash_collision_orginal_str)) {
        LOGI(TAG, "found result %s", g_nlu_content_str[g_nlu_content_mapping[i].nlu_content_str_index]);
        result = cJSON_Parse(g_nlu_content_str[g_nlu_content_mapping[i].nlu_content_str_index]);
        if (result == NULL) {
          LOGE(TAG, "json parse failed, memory may not enough, reboot now");
          return NULL;
        }
        _replace_asr_recongize(result, cmd_item->valuestring);
        nlu = _replace_nlu_payload(result);
        if (nlu) {
          nlu_str = cJSON_PrintUnformatted(nlu);
          cJSON_Delete(nlu);
        } else {
          cJSON_Delete(result);
        }
        cJSON_Delete(asr);
        return nlu_str;
      }
    }
  }
  cJSON_Delete(asr);
  return NULL;
}

char* NluParseWkuRst(const char *asr_json, float *score, int *index) {
  cJSON *wake_item = NULL;
  cJSON *score_item = NULL;
  char *wakeword = NULL;
  (void)index;
  cJSON *asr = cJSON_Parse(asr_json);
  if (asr == NULL) {
    LOGE(TAG, "asr rst parse fail");
    return NULL;
  }

  do {
    wake_item = cJSON_GetObjectItem(asr, "wakeup_word");
    if (wake_item == NULL) {
      LOGE(TAG, "not get wakewords item");
      break;
    }

    score_item = cJSON_GetObjectItem(asr, "score");
    if (score_item == NULL) {
      LOGE(TAG, "not get score item");
      break;
    }

    wakeword = malloc(strlen(wake_item->valuestring) + 1);
    if (wakeword == NULL) {
      LOGE(TAG, "malloc wakeword buf");
      break;
    }

    strcpy(wakeword, wake_item->valuestring);
  } while (0);

  if (asr) {
    cJSON_Delete(asr);
  }
  return wakeword;
}

