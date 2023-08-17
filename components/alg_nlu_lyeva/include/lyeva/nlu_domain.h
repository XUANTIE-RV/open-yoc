/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#ifndef _NLU_DOMAIN_H_
#define _NLU_DOMAIN_H_

#include "lyeva/nlu.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MIN_SCORE 58.8357
#define MIN_SCORE_C 10

  typedef enum
  {
    CTL_ACC_POWER_CTRL = 0,
    CTL_ACC_MODE_PMV = 1,
    CTL_ACC_MODE_COLD = 2,
    CTL_ACC_MODE_HOT = 3,
    CTL_ACC_MODE_DEHUM = 4,
    CTL_ACC_MODE_WIND = 5,
    CTL_ACC_TEMP_SET_INCREASE = 6,
    CTL_ACC_TEMP_SET_DECREASE = 7,
    CTL_ACC_TEMP_SET_MAX_MIN = 8,
    CTL_ACC_TEMP_SET = 9,
    CTL_ACC_WIND_SPEED_INCREASE = 10,
    CTL_ACC_WIND_SPEED_DECREASE = 11,
    CTL_ACC_WIND_SPEED_MAX_MIN = 12,
    CTL_ACC_WIND_SPEED_HIGH = 13,
    CTL_ACC_WIND_SPEED_MID = 14,
    CTL_ACC_WIND_SPEED_LOW = 15,
    CTL_ACC_WIND_SPEED_AUTO = 16,
    CTL_ACC_SWING_LEFT_RIGHT = 17,
    CTL_ACC_SWING_UP_DOWN = 18,
    CTL_ACC_WIND_DIRECT_MIDDLE = 19,
    CTL_ACC_WIND_DIRECT_LEFT_RIGHT = 20,
    CTL_ACC_WIND_DIRECT_LEFT = 21,
    CTL_ACC_WIND_DIRECT_RIGHT = 22,
    CTL_ACC_WIND_DIRECT_UP = 23,
    CTL_ACC_WIND_DIRECT_DOWN = 24,
    CTL_ACC_WIND_AUXILIARY_HEATING = 25,
    CTL_ACC_CLEAN = 26,
    CTL_ACC_MUTE = 27,
    CTL_ACC_VOLUME_UP = 28,
    CTL_ACC_VOLUME_DOWN = 29,
    CTL_ACC_VOLUME_MAX_MIN = 30,
    CTL_ACC_VOICE = 31,
    CTL_ACC_FACTORY_RESET = 32,
    CTL_ACC_WIFI = 33,
    CTL_ACC_BRUTE_FORCE = 34,
    CTL_ACC_SLEEP = 35,
    CTL_ACC_SCREEN = 36,
    CTL_ACC_SOUND_SOURCE = 37,
    CTL_ACC_SOUND_SOURCE_CLOSE = 38,
    CTL_ACC_LOCK = 39,
    CTL_ACC_REACTION = 40,
    CTL_ACC_HEALTH = 41,
    CTL_ACC_WASH = 42,
    CTL_ACC_WASH_WIND_LOW = 43,
    CTL_ACC_WASH_WIND_HIGH = 44,
    CTL_ACC_SENSE_FOLLOW = 45,
    CTL_ACC_SENSE_ESCAPE = 46,
    CTL_ACC_SENSE = 47,
    CTL_ACC_XERANSIS = 48,
    CTL_ACC_METHANAL = 49,
    CTL_ACC_ANION = 50,
    CTL_ACC_PM25 = 51,
    CTL_ACC_HUMIDIFICATION = 52,
    CTL_ACC_FRESH = 53,
    CTL_ACC_BLUETOOTH = 54,
    CTL_ACC_OFFLINE = 55,
    CTL_ACC_SESSION = 56,
    OTHERS = 57,
  } acc_intention_en;

  const char *nlu_intention_str(int intention)
  {
    switch (intention)
    {
    case CTL_ACC_POWER_CTRL:
      return "CTL_ACC_POWER_CTRL";
    case CTL_ACC_MODE_PMV:
      return "CTL_ACC_MODE_PMV";
    case CTL_ACC_MODE_COLD:
      return "CTL_ACC_MODE_COLD";
    case CTL_ACC_MODE_HOT:
      return "CTL_ACC_MODE_HOT";
    case CTL_ACC_MODE_DEHUM:
      return "CTL_ACC_MODE_DEHUM";
    case CTL_ACC_MODE_WIND:
      return "CTL_ACC_MODE_WIND";
    case CTL_ACC_TEMP_SET_INCREASE:
      return "CTL_ACC_TEMP_SET_INCREASE";
    case CTL_ACC_TEMP_SET_DECREASE:
      return "CTL_ACC_TEMP_SET_DECREASE";
    case CTL_ACC_TEMP_SET_MAX_MIN:
      return "CTL_ACC_TEMP_SET_MAX_MIN";
    case CTL_ACC_TEMP_SET:
      return "CTL_ACC_TEMP_SET";
    case CTL_ACC_WIND_SPEED_INCREASE:
      return "CTL_ACC_WIND_SPEED_INCREASE";
    case CTL_ACC_WIND_SPEED_DECREASE:
      return "CTL_ACC_WIND_SPEED_DECREASE";
    case CTL_ACC_WIND_SPEED_MAX_MIN:
      return "CTL_ACC_WIND_SPEED_MAX_MIN";
    case CTL_ACC_WIND_SPEED_HIGH:
      return "CTL_ACC_WIND_SPEED_HIGH";
    case CTL_ACC_WIND_SPEED_MID:
      return "CTL_ACC_WIND_SPEED_MID";
    case CTL_ACC_WIND_SPEED_LOW:
      return "CTL_ACC_WIND_SPEED_LOW";
    case CTL_ACC_WIND_SPEED_AUTO:
      return "CTL_ACC_WIND_SPEED_AUTO";
    case CTL_ACC_SWING_LEFT_RIGHT:
      return "CTL_ACC_SWING_LEFT_RIGHT";
    case CTL_ACC_SWING_UP_DOWN:
      return "CTL_ACC_SWING_UP_DOWN";
    case CTL_ACC_WIND_DIRECT_MIDDLE:
      return "CTL_ACC_WIND_DIRECT_MIDDLE";
    case CTL_ACC_WIND_DIRECT_LEFT_RIGHT:
      return "CTL_ACC_WIND_DIRECT_LEFT_RIGHT";
    case CTL_ACC_WIND_DIRECT_LEFT:
      return "CTL_ACC_WIND_DIRECT_LEFT";
    case CTL_ACC_WIND_DIRECT_RIGHT:
      return "CTL_ACC_WIND_DIRECT_RIGHT";
    case CTL_ACC_WIND_DIRECT_UP:
      return "CTL_ACC_WIND_DIRECT_UP";
    case CTL_ACC_WIND_DIRECT_DOWN:
      return "CTL_ACC_WIND_DIRECT_DOWN";
    case CTL_ACC_WIND_AUXILIARY_HEATING:
      return "CTL_ACC_WIND_AUXILIARY_HEATING";
    case CTL_ACC_CLEAN:
      return "CTL_ACC_CLEAN";
    case CTL_ACC_MUTE:
      return "CTL_ACC_MUTE";
    case CTL_ACC_VOLUME_UP:
      return "CTL_ACC_VOLUME_UP";
    case CTL_ACC_VOLUME_DOWN:
      return "CTL_ACC_VOLUME_DOWN";
    case CTL_ACC_VOLUME_MAX_MIN:
      return "CTL_ACC_VOLUME_MAX_MIN";
    case CTL_ACC_VOICE:
      return "CTL_ACC_VOICE";
    case CTL_ACC_FACTORY_RESET:
      return "CTL_ACC_FACTORY_RESET";
    case CTL_ACC_WIFI:
      return "CTL_ACC_WIFI";
    case CTL_ACC_BRUTE_FORCE:
      return "CTL_ACC_BRUTE_FORCE";
    case CTL_ACC_SLEEP:
      return "CTL_ACC_SLEEP";
    case CTL_ACC_SCREEN:
      return "CTL_ACC_SCREEN";
    case CTL_ACC_SOUND_SOURCE:
      return "CTL_ACC_SOUND_SOURCE";
    case CTL_ACC_SOUND_SOURCE_CLOSE:
      return "CTL_ACC_SOUND_SOURCE_CLOSE";
    case CTL_ACC_LOCK:
      return "CTL_ACC_LOCK";
    case CTL_ACC_REACTION:
      return "CTL_ACC_REACTION";
    case CTL_ACC_HEALTH:
      return "CTL_ACC_HEALTH";
    case CTL_ACC_WASH:
      return "CTL_ACC_WASH";
    case CTL_ACC_WASH_WIND_LOW:
      return "CTL_ACC_WASH_WIND_LOW";
    case CTL_ACC_WASH_WIND_HIGH:
      return "CTL_ACC_WASH_WIND_HIGH";
    case CTL_ACC_SENSE_FOLLOW:
      return "CTL_ACC_SENSE_FOLLOW";
    case CTL_ACC_SENSE_ESCAPE:
      return "CTL_ACC_SENSE_ESCAPE";
    case CTL_ACC_SENSE:
      return "CTL_ACC_SENSE";
    case CTL_ACC_XERANSIS:
      return "CTL_ACC_XERANSIS";
    case CTL_ACC_METHANAL:
      return "CTL_ACC_METHANAL";
    case CTL_ACC_ANION:
      return "CTL_ACC_ANION";
    case CTL_ACC_PM25:
      return "CTL_ACC_PM25";
    case CTL_ACC_HUMIDIFICATION:
      return "CTL_ACC_HUMIDIFICATION";
    case CTL_ACC_FRESH:
      return "CTL_ACC_FRESH";
    case CTL_ACC_BLUETOOTH:
      return "CTL_ACC_BLUETOOTH";
    case CTL_ACC_OFFLINE:
      return "CTL_ACC_OFFLINE";
    case CTL_ACC_SESSION:
      return "CTL_ACC_SESSION";
    case OTHERS:
      return "OTHERS";
    default:
      return "NULL";
    }
  }

  typedef enum
  {
    SLOTS_ACTION_ON = 0,
    SLOTS_ACTION_OFF = 1,
    SLOTS_ACTION_MAX = 2,
    SLOTS_ACTION_MIN = 3,
    SLOTS_NUM = 4,
    SLOTS_TIME = 5,
  } acc_slots_en;

  typedef struct
  {
    acc_slots_en domain_slots;
    nlu_major_slots_en major_slots;
    char *syn_word;
  } nlu_acc_slots_info_t;

  nlu_acc_slots_info_t nlu_acc_slots_info[] = {
      {SLOTS_ACTION_ON, SLOT_ACTION, "开"},
      {SLOTS_ACTION_OFF, SLOT_ACTION, "关"},
      {SLOTS_ACTION_MAX, SLOT_ACTION, "最高"},
      {SLOTS_ACTION_MIN, SLOT_ACTION, "最低"},
      {SLOTS_NUM, SLOT_NUM, NULL},
  };

  typedef struct
  {
    int domain;
    int size;
    char **words;
  } nlu_domain_info_t;

  typedef enum
  {
    NLU_DOMAIN_ACC = 0,
    NLU_DOMAIN_OTHERS,
  } nlu_domain_en;

  const int g_domain_default = NLU_DOMAIN_ACC;

  char *nlu_domain_acc[] = {
      "空调",
  };

  nlu_domain_info_t g_domain_infos[] = {
      {
          NLU_DOMAIN_ACC,
          1,
          nlu_domain_acc,
      },
  };

  const char *nlu_domain_str(int domain)
  {
    switch (domain)
    {
    case NLU_DOMAIN_ACC:
      return "NLU_DOMAIN_ACC";
    case NLU_DOMAIN_OTHERS:
      return "NLU_DOMAIN_OTHERS";
    default:
      return "NULL";
    }
  }
#ifdef __cplusplus
}
#endif

#endif
