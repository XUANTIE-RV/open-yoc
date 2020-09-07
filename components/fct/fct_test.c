/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <aos/log.h>
#include <aos/list.h>
#include <aos/debug.h>
#include <ctype.h>
#include <string.h>
#include <yoc/uservice.h>

#include "fct.h"
#include "fct_comm.h"

#define TAG "fct"


typedef struct fct_cmd_param {
    char            **args;
    int             arg_num;
    int             isquery;
    fct_test_case   test_func;
} fct_cmd_param_t;

typedef struct cmd_list {
    const char             *name;
    int                    cmd_id;
    fct_test_case          test_func;
    slist_t                next;
} cmd_list_t;

static slist_t cmd_lists;
static int test_case_num = 0;

static uservice_t  *fct_usrv = NULL;
static fct_test_stat_t test_stat;
static char *test_result;

static int fct_process_rpc(void *context, rpc_t *rpc)
{
    fct_cmd_param_t *param = rpc_get_buffer(rpc, NULL);
    int ret;
    
    test_result[0] = 0;
    ret = param->test_func(param->isquery, param->arg_num, param->args, test_result);
    if (ret == FTEST_RESULT_SUCCESS) {
        test_stat = TEST_END;
    } else {
        /* if user doesn't write err reason, give it one */
        if (test_result[0] == 0) {
            switch(ret) {
                case FTEST_RESULT_ERR_PARAM:
                    strcpy(test_result, "param error");
                    break;

                case FTEST_RESULT_ERR_FAILED:
                    strcpy(test_result, "test failed");
                    break;

                default:
                    break;
            }
        }
        test_stat = TEST_ERROR;
    }

    rpc_reply(rpc);

    return -1;
}

int fct_register_cmd(const char *name, char *help, fct_test_case test_function)
{
    int ret;

    if (name == NULL || test_function == NULL) {
        return -EINVAL;
    }

    cmd_list_t *temp_cmd_node;
    slist_for_each_entry(&cmd_lists, temp_cmd_node, cmd_list_t, next)
    {
        if (strcmp(temp_cmd_node->name, name) == 0) {
            return -EINVAL;
        }
    }

    ret = fct_comm_reg_cmd(name, help, test_case_num);
    CHECK_RET_WITH_RET(ret == 0, -1);

    cmd_list_t *cmd_node = (cmd_list_t *)aos_malloc_check(sizeof(cmd_list_t));
    cmd_node->cmd_id = test_case_num++;
    cmd_node->name = name;
    cmd_node->test_func = test_function;
    
    slist_add_tail(&cmd_node->next, &cmd_lists);

    return 0;
}

int fct_unregister_cmd(const char *name)
{
    if (NULL == name) {
        return -EINVAL;
    }

    cmd_list_t *cmd_node;
    slist_for_each_entry(&cmd_lists, cmd_node, cmd_list_t, next)
    {
        if (strcmp(cmd_node->name, name) == 0) {
            fct_comm_unreg_cmd(name);
            slist_del(&cmd_node->next, &cmd_lists);
            aos_free(cmd_node);
            break;
        }
    }

    return 0;
}

int fct_run_test(int cmd_id, int isquery, int argc, char **argv)
{
    int ret = -1;
    fct_cmd_param_t param;

    CHECK_PARAM(cmd_id < test_case_num && cmd_id >= 0, -1);

    if (test_stat == TEST_RUNNING) {
        return -1;
    }

    cmd_list_t *cmd_node;
    int found = 0;
    slist_for_each_entry(&cmd_lists, cmd_node, cmd_list_t, next)
    {
        if (cmd_node->cmd_id == cmd_id) {
            param.test_func = cmd_node->test_func;
            found = 1;
            break;
        }
    }

    if (!found) {
        LOGW(TAG, "fct test case not found %d", cmd_id);
        return -1;
    }

    test_stat = TEST_RUNNING;

    param.isquery = isquery;
    param.arg_num = argc;
    param.args = argv;

    ret = uservice_call_async(fct_usrv, cmd_id, &param, sizeof(param));
    return ret;
}

char *fct_get_result()
{
    return test_result;
}

int fct_state()
{
    return test_stat;
}

int fct_init(int uart_id, uint32_t baud_rate, utask_t *task)
{
    int ret;

    if (task == NULL) {
        return -EINVAL;
    }

    ret = fct_comm_init(uart_id, baud_rate);
    if (ret < 0) {
        return ret;
    }

    fct_usrv = uservice_new("fct", fct_process_rpc, NULL);

    if (fct_usrv == NULL) {
        return -ENOMEM;
    }

    utask_add(task, fct_usrv);

    test_result = (char *)aos_malloc_check(FTEST_MAX_RESULT_LEN + 1);
    if (test_result == NULL) {
        return -ENOMEM;
    }

    return 0;
}
