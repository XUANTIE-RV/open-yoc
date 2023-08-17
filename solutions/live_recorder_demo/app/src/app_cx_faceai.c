/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <aos/cli.h>
#include <aos/kernel.h>
#include <stdio.h>
#include <string.h>
#include <ulog/ulog.h>
#include <app_cx_faceai.h>

#define TAG  "faceai"
#define IMG_PATH_SIZE (128)

static cx_srv_hdl_t g_faceai_handler;

static void face_enroll_cb(cx_srv_hdl_t face_hdl, cx_face_enroll_result_t *result)
{
    if(result->success == 0) {
        LOGD(TAG, "face_enroll_cb userid: %d\n", result->user_info.user_id);
    }
}

static void face_verify_cb(cx_srv_hdl_t face_hdl, cx_face_verify_result_t *result)
{
    LOGD(TAG, "face_verify_cb result: %d, ID: %d\n", result->result, result->user_id);
}

int app_faceai_start(void)
{
    // faceai start facedet faceldmk quality living2d living3d facerecog
    if (g_faceai_handler != NULL) {
        LOGE(TAG, "face ai already start");
        return 0;
    }

    g_faceai_handler = cx_face_start("faceai", face_enroll_cb, face_verify_cb);

    LOGD(TAG, "start face ai");
    return 0;
}

int app_faceai_stop(void)
{
    int ret = -1;
    // faceai stop
    LOGD(TAG, "Stop FaceAI service.");
    if (g_faceai_handler == NULL) {
        LOGE(TAG, "faceai not ready");
        return 0;
    }

    ret = cx_face_stop(g_faceai_handler);
    g_faceai_handler = NULL;
    LOGD(TAG, "Stop FaceAI service finish. %d", ret);
    return ret ;
}

int app_faceai_register(cx_face_user_info_t *user_info, cx_face_dir_e face_dir, int timeout)
{
    int ret = -1;
    // faceai register username admin(-1/0/1) direction(1-middle) left_img_path right_img_path
    LOGD(TAG, "Register face.");
    if (g_faceai_handler == NULL || user_info == NULL) {
        LOGE(TAG, "Please start service.\r\n");
        return ret;
    }

    face_dir = FACE_DIRECTION_MIDDLE;

    ret = cx_face_enroll(g_faceai_handler, user_info, face_dir, timeout);
    LOGE(TAG, "FaceAI app cx_face_enroll result: %d\r\n", ret);
    return ret;
}

int app_faceai_verify(int timeout)
{
    int ret = -1;
    // faceai verify left_img_path right_img_path
    LOGD(TAG, "Verify face.\r\n");
    if (g_faceai_handler == NULL) {
        LOGE(TAG, "Please start service.\r\n");
        return ret;
    }

    ret = cx_face_verify(g_faceai_handler, timeout);
    LOGE(TAG, "FaceAI app cx_face_verify result: %d\r\n", ret);

    return ret;
}

int app_faceai_get_user_info(int user_id, cx_face_user_info_t *user_info)
{
    int ret = -1;
    // faceai get-user userid
    LOGD(TAG, "Get userinfo.\r\n");
    if (g_faceai_handler == NULL || user_info == NULL) {
        LOGE(TAG, "Please start service.\r\n");
        return ret;
    }

    //cx_face_user_info_t user_info;
    ret = cx_face_get_user_info(g_faceai_handler, user_id, user_info);
    LOGD(TAG, "get_user_info result: %d", ret);
    return ret;
}

int app_faceai_delete_user(int user_id)
{
    int ret = -1;
    // faceai delete-user userid
    LOGD(TAG, "Delete user.");
    if (g_faceai_handler == NULL) {
        LOGE(TAG, "Please start service.");
        return ret;
    }

    ret = cx_face_delete_user(g_faceai_handler, user_id);
    LOGD(TAG, "Delete user_id: %d, ret %d.", user_id, ret);
    return ret;
}

int app_faceai_get_all_user_id(uint32_t *user_id_list, uint32_t size)
{
    int ret = -1;
    // faceai get-id-list
    LOGD(TAG, "Get all user.\r\n");
    if (g_faceai_handler == NULL || user_id_list == NULL) {
        LOGE(TAG, "Please start service.\r\n");
        return ret;
    }

    //uint32_t user_id_list[50];
    int id_count = cx_face_get_all_user_id(g_faceai_handler, user_id_list, size);
    return id_count;
}

int app_faceai_set_threshold(cx_face_ai_threshold_t *threshold)
{
    int ret = -1;
    // faceai get-id-list
    LOGD(TAG, "Set threshold.\r\n");
    if (g_faceai_handler == NULL || threshold == NULL) {
        LOGE(TAG, "Please start service.\r\n");
        return ret;
    }
#if 0
    cx_face_ai_threshold_t threshold;
    threshold.facedetect_thres = atoi(argv[2]);
    threshold.ptscore_thres = atoi(argv[3]);
    threshold.facequality_thres = atoi(argv[4]);
    threshold.living2d_thres = atoi(argv[5]);
    threshold.living3d_thres = atoi(argv[6]);
    threshold.face_reco_thres = atoi(argv[7]);
#endif
    ret = cx_face_set_threshold(g_faceai_handler, threshold);
    LOGD(TAG, "faceai_set_threshold result is %d.\r\n", ret);
    return ret;
}


int app_faceai_set_input_path(const char* left_img_path, const char* right_img_path)
{
    int ret = -1;
    LOGD(TAG, "Set input path.\r\n");

    if (g_faceai_handler == NULL || left_img_path == NULL || right_img_path == NULL) {
        LOGE(TAG, "Please start service, and check your input.\r\n");
        return ret;
    }

    ret = cx_face_set_input_path(g_faceai_handler, left_img_path, right_img_path);
    LOGD(TAG, "app_faceai_set_input_path result is %d.\r\n", ret);
    return ret;
}
