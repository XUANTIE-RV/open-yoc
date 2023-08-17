/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <aos/cli.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <string.h>
#include <app_cx_faceai.h>
#include <app_cx_record.h>
#include <app_gui.h>
#include <media_config.h>
#include <cx/preview/preview_service.h>
#include "pthread.h"

#define IMG_PATH_SIZE (128)
extern void set_faceai_dump_switch(int sw);
extern pthread_mutex_t sensor_toggle_lock;
void print_user_info(cx_face_user_info_t *user_info)
{
    aos_cli_printf("user_info: user_id: %d, user_name: %s, admin: %d\r\n", user_info->user_id, user_info->user_name, user_info->admin);
}

void faceai_start(char *buf, int32_t len, int32_t argc, char **argv)
{
    int ret = -1;
    // faceai start facedet faceldmk quality living2d living3d facerecog
    aos_cli_printf("Start FaceAI service.\r\n");

    if (argc == 2) {
        SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
        MediaVideoIRInit();
#endif
        ret = app_faceai_start();
        SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
        aos_cli_printf("FaceAI start %d", ret);
    }
    else {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }
}

void faceai_stop(char *buf, int32_t len, int32_t argc, char **argv)
{
    int ret = -1;
    // faceai stop
    aos_cli_printf("Stop FaceAI service.\r\n");
    if (argc > 2) {
        aos_cli_printf("Unsupport command\r\n");
        return;
    }
    SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRDeInit();
#endif
    ret = app_faceai_stop();
    SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
    aos_cli_printf("FaceAI stop %d.\r\n", ret);
}

void faceai_register(char *buf, int32_t len, int32_t argc, char **argv)
{
    if (app_linkvisual_isstreaming()) {
        aos_cli_printf("Error: linkvisual is streaming!\r\n");
        return;
    }

    // faceai register username admin(-1/0/1) direction(1-middle)
    aos_cli_printf("Register face.\r\n");

    cx_face_user_info_t user_info;
    cx_face_dir_e face_dir = FACE_DIRECTION_MIDDLE;
    char left_img_path[IMG_PATH_SIZE];
    char right_img_path[IMG_PATH_SIZE];
    if (argc == 2) {
        aos_cli_printf("Register face in default param.\r\n");
    } else if (argc == 5) {
        // get user name 
        snprintf(user_info.user_name, 32, argv[2]);
        user_info.admin = atoi(argv[3]);
        face_dir = (cx_face_dir_e)atoi(argv[4]);
    } else if (argc == 7) {
        // get user name 
        snprintf(user_info.user_name, 32, argv[2]);
        user_info.admin = atoi(argv[3]);
        face_dir = (cx_face_dir_e)atoi(argv[4]);

        // get input path
        snprintf(left_img_path, IMG_PATH_SIZE, argv[5]);
        snprintf(right_img_path, IMG_PATH_SIZE, argv[6]);
        app_faceai_set_input_path(left_img_path, right_img_path);
        aos_cli_printf("Set image path\r\n");
    } else {
        aos_cli_printf("Unsupport command.\r\n");
        return;
    }
    SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#endif
    int result = app_faceai_register(&user_info, face_dir, -1);
#if defined(CONFIG_IR_GUI_EN) && CONFIG_IR_GUI_EN
    extern cx_preview_hdl_t preview_handle;
    preview_handle = cx_preview_start("preview", PREVIEW_MODE_IR);
    char show_test[128];
    if(result == 0) {
        snprintf(show_test, sizeof(show_test), "Register Success: %d", result);
        GUI_Show_Text(show_test);
    } else {
        snprintf(show_test, sizeof(show_test), "Register Failed: %d", result);
        GUI_Show_Text(show_test);
    }
#endif
    SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
    aos_cli_printf("cx_face_enroll result: %d\r\n", result);
}

void faceai_verify(char *buf, int32_t len, int32_t argc, char **argv)
{
    if (app_linkvisual_isstreaming()) {
        aos_cli_printf("Error: linkvisual is streaming!\r\n");
        return;
    }

    // faceai verify left_img_path right_img_path
    aos_cli_printf("Verify face.\r\n");

    char left_img_path[IMG_PATH_SIZE];
    char right_img_path[IMG_PATH_SIZE];
    if (argc == 2) {
        aos_cli_printf("Verify face from camera input.\r\n");
    } else if (argc == 4) {
        // get input path
        snprintf(left_img_path, IMG_PATH_SIZE, argv[2]);
        snprintf(right_img_path, IMG_PATH_SIZE, argv[3]);
        app_faceai_set_input_path(left_img_path, right_img_path);
        aos_cli_printf("Set image path\r\n");
    } else {
        aos_cli_printf("Unsupport command.\r\n");
        return;
    }
    SENSOR_TOGGLE_LOCK(&sensor_toggle_lock);
#ifdef CONFIG_RGBIR_SENSOR_SWITCH
    MediaVideoIRInit();
#endif
    int result = app_faceai_verify(-1);
#if defined(CONFIG_IR_GUI_EN) && CONFIG_IR_GUI_EN
    extern cx_preview_hdl_t preview_handle;
    preview_handle = cx_preview_start("preview", PREVIEW_MODE_IR);
    char show_test[128];
    if(result >= 0) {
        snprintf(show_test, sizeof(show_test), "Recognition Success: %d", result);
        GUI_Show_Text(show_test);
    } else {
        snprintf(show_test, sizeof(show_test), "Recognition Failed: %d", result);
        GUI_Show_Text(show_test);
    }
#endif
    SENSOR_TOGGLE_UNLOCK(&sensor_toggle_lock);
    aos_cli_printf("cx_face_verify result: %d\r\n", result);
}

void faceai_get_user_info(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-user userid
    aos_cli_printf("Get userinfo.\r\n");

    if (argc != 3) {
        aos_cli_printf("Invalid param number.\r\n");
        return;
    }

    int user_id = atoi(argv[2]);
    cx_face_user_info_t user_info;
    int ret = app_faceai_get_user_info(user_id, &user_info);
    aos_cli_printf("cx_face_get_user_info result: %d\r\n", ret);
    if(ret != 0) {
        return;
    }
    print_user_info(&user_info);
}

void faceai_delete_user(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai delete-user userid
    aos_cli_printf("Delete user.\r\n");

    if (argc != 3) {
        aos_cli_printf("Invalid param number.\r\n");
        return;
    }

    int user_id = atoi(argv[2]);
    int ret = app_faceai_delete_user(user_id);
    aos_cli_printf("cx_face_delete_user result: %d\r\n", ret);
    if(ret != 0) {
        return;
    }

    aos_cli_printf("Delete user_id: %d.\r\n", user_id);
}

void faceai_get_all_user_id(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-id-list
    aos_cli_printf("Get all user.\r\n");

    if (argc != 2) {
        aos_cli_printf("Invalid param number.\r\n");
        return;
    }

    uint32_t user_id_list[50];
    int id_count = app_faceai_get_all_user_id(user_id_list, 50);
    aos_cli_printf("user id list size is %d.\r\n", id_count);
    aos_cli_printf("user id list is: \r\n");
    for (int i = 0; i < id_count; i++)
    {
         aos_cli_printf("user id %d: %d\r\n", i, user_id_list[i]);
    }
    aos_cli_printf("\r\n"); 
}

void faceai_set_threshold(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-id-list
    aos_cli_printf("Set threshold.\r\n");

    if (argc != 8) {
        aos_cli_printf("Invalid param number.\r\n");
        return;
    }
    cx_face_ai_threshold_t threshold;
    threshold.facedetect_thres = atoi(argv[2]);
    threshold.ptscore_thres = atoi(argv[3]);
    threshold.facequality_thres = atoi(argv[4]);
    threshold.living2d_thres = atoi(argv[5]);
    threshold.living3d_thres = atoi(argv[6]);
    threshold.face_reco_thres = atoi(argv[7]);
    int ret = app_faceai_set_threshold(&threshold);
    aos_cli_printf("faceai_set_threshol result is %d.\r\n", ret);
}

void faceai_set_dump(char *buf, int32_t len, int32_t argc, char **argv)
{
    // faceai get-id-list
    aos_cli_printf("Set dump switch.\r\n");

    if (argc != 3) {
        aos_cli_printf("Invalid param number.\r\n");
        return;
    }
    set_faceai_dump_switch(atoi(argv[2]));
    aos_cli_printf("faceai_set_dump now switch is %d.\r\n", atoi(argv[2]));
}

void faceai_help()
{
    aos_cli_printf("faceai commands help:\r\n");
    aos_cli_printf("start: start service\r\n");
    aos_cli_printf("stop: stop service\r\n");
}

void faceai_cmd(char *buf, int32_t len, int32_t argc, char **argv)
{
    aos_cli_printf("This is FaceAI test command begin.\r\n");
    
    if (argc < 2) {
        aos_cli_printf("use 'debug_api help' for test\r\n");
        return;
    }

    if (strcmp(argv[1], "start") == 0) {
        faceai_start(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "stop") == 0) {
        faceai_stop(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "register") == 0) {
        faceai_register(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "verify") == 0) {
        faceai_verify(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "get-user") == 0) {
        faceai_get_user_info(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "delete-user") == 0) {
        faceai_delete_user(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "get-id-list") == 0) {
        faceai_get_all_user_id(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "set-thres") == 0) {
        faceai_set_threshold(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "dump") == 0) {
        faceai_set_dump(buf, len, argc, argv);
    }
    else if (strcmp(argv[1], "help") == 0) {
        faceai_help();
    }
    else {
        aos_cli_printf("Unsupport command.\r\n");
    }

    aos_cli_printf("This is FaceAI test command end.\r\n");
}

const struct cli_command faceai_cli_cmds[] = {
    {"faceai", "FaceAI test", faceai_cmd},
};

void cli_reg_cmd_faceai(void)
{
    int32_t ret;
    
    ret = aos_cli_register_commands(faceai_cli_cmds, sizeof(faceai_cli_cmds) / sizeof(struct cli_command));
    if (ret) {
        aos_cli_printf("faceai cmds register fail\r\n");
    }
}