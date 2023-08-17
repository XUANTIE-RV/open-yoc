/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef _APP_CX_FACEAI_H_
#define _APP_CX_FACEAI_H_

#include <stdint.h>
#include <cx/faceai/face_ai_service.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int app_faceai_start(void);
int app_faceai_stop(void);
int app_faceai_register(cx_face_user_info_t *user_info, cx_face_dir_e face_dir, int timeout);
int app_faceai_verify(int timeout);
int app_faceai_get_user_info(int user_id, cx_face_user_info_t *user_info);
int app_faceai_delete_user(int user_id);
int app_faceai_get_all_user_id(uint32_t *user_id_list, uint32_t size);
int app_faceai_set_threshold(cx_face_ai_threshold_t *threshold);
int app_faceai_set_input_path(const char* left_img_path, const char* right_img_path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif