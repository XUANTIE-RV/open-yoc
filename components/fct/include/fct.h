/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _FCT_H_

#include <stdio.h>

/*测试结果最大长度*/
#define FTEST_MAX_RESULT_LEN        256

/* 测试返回结果 */
#define FTEST_RESULT_SUCCESS        0  
#define FTEST_RESULT_ERR_PARAM      -1
#define FTEST_RESULT_ERR_FAILED     -2

typedef int (*fct_test_case)(int isquery, int arg_num, char **args, char *test_result);

/**
 * 初始化产测程序
 *
 * @param[in]  uart_id 产测命令串口ID
 * @param[in]  baud_rate 串口波特率
 * @param[in]  task 产测程序运行线程
 * @return  0 成功, 其他失败.
 */
int fct_init(int uart_id, uint32_t baud_rate, utask_t *task);

/**
 * 添加按键配置
 *
 * @param[in]  name AT指令名
 * @param[in]  help AT指令帮助信息，上位机通过AT+XXX=?获取
 * @param[in]  test_function 产测程序
 * @return  0 成功, 其他失败.
 */
int fct_register_cmd(const char *name, char *help, fct_test_case test_function);

/**
 * 添加按键配置
 *
 * @param[in]  name AT指令名
 * @return  0 成功, 其他失败.
 */
int fct_unregister_cmd(const char *name);

#endif
