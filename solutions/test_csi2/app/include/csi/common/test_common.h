/*
 * Copyright (C) 2020 C-SKY Microsystems Co., Ltd. All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions an
 * limitations under the License.
 */

#ifndef __TEST_COMMON__
#define __TEST_COMMON__

typedef int (*test_func)(char *args);

typedef struct {
    char *test_func_name;
    test_func test_func_p;
}test_func_map_t;

/**
 * description: 定义AT测试指令搭配的参数，以及调用的测试函数的映射关系
 *  name: AT测试指令的命令部分
 *  function: AT测试指令所调用的测试函数
 *  args_num: AT测试指令所需要的搭配的参数个数
 */
typedef struct {
    char *name;
    test_func function;
    uint8_t args_num;
}test_info_t;

extern volatile int8_t _dev_idx;

extern int args_parsing(void *args, uint32_t *value, uint8_t num);
extern void cmd_parsing(void *cml, void *mc, void *args);
extern int args_parsing_new(void *args, uint64_t *value, uint8_t num);
extern int testcase_jump(char *args, void *test_info);

extern void transfer_data(char *addr, uint32_t size);
extern void dataset(char *addr, uint32_t size, uint8_t pattern);

extern int ttimer_start(uint8_t timer_idx, uint32_t time_cell);
extern void ttimer_stop(void);
extern uint32_t ttimer_get_cell_count(void);

/**
Description: this function could be used to convert string(hex format) to array. 
             for example: "aaeeff55dd22" convert to  {0xaa, 0xee, 0xff, 0x55, 0xdd, 0x22}
usage:
    char *in = "aaeeff55dd";
    uint8_t out[6];
    hex_to_array(in, 6, out);
*/
extern int hex_to_array(const char *in, int in_size, uint8_t *out);
extern void generate_rand_array(char *buffer, uint8_t max_rand, uint32_t length);
extern void generate_rand_array2(uint8_t *buffer, uint8_t max_rand, uint32_t length);
#endif