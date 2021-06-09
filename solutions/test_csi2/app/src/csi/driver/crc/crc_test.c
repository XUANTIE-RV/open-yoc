/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "crc_test.h"

test_func_map_t crc_test_funcs_map[] = {
  {"CRC_FUNC",test_crc_func},
  {"CRC_PERFORMANCE",test_crc_performance},
  {"CRC_STABILITY",test_crc_stability},
};

int test_crc_main(char *args)
{
  uint8_t i;

  for(i = 0;i<sizeof(crc_test_funcs_map)/sizeof(test_func_map_t);i++){
    if(!strcmp((void *)_mc_name, crc_test_funcs_map[i].test_func_name)){
      (*(crc_test_funcs_map[i].test_func_p))(args);
      return 0;
    }
  }
  TEST_CASE_TIPS("CRC module don't support this command.");
  return -1;
}
