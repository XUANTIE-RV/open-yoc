/*
 * Copyright (c) 2022-2022 Alibaba Group Holding Limited
 */

#ifndef __FACE_AI_SERVICE_H__
#define __FACE_AI_SERVICE_H__

#include <stdint.h>
#include <cx/faceai/face_ai_data_type.h>

typedef void * cx_srv_hdl_t;
typedef void (*cx_face_enroll_callback_t)(cx_srv_hdl_t face_hdl, cx_face_enroll_result_t *result);
typedef void (*cx_face_verify_callback_t)(cx_srv_hdl_t face_hdl, cx_face_verify_result_t *result);

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @description: start face ai service and get service handler
 * @param [in] *srv_name: user defined service name
 * @param [in] face_enroll_cb: face enroll callback
 * @param [in] face_verify_cb: face verify callback
 * @return [cx_srv_hdl_t*]: face ai service handler
 */
cx_srv_hdl_t cx_face_start(const char *srv_name, cx_face_enroll_callback_t face_enroll_cb, cx_face_verify_callback_t face_verify_cb);

/**
 * @description: stop face ai service
 * @param [in] *hdl: face ai service handler
 * @return [int]: result code, return 0 when success
 */
int cx_face_stop(const cx_srv_hdl_t hdl);

/**
 * @description: enroll face with specific direction
 * @param [in] *hdl: face ai handler
 * @param [in] enroll_user: user information with user name and administor flag
 * @param [in] face_dir: face direction
 * @param [in] timeout: timeout
 * @return [int] result code, return 0 when success
 */
int cx_face_enroll(const cx_srv_hdl_t hdl, const cx_face_user_info_t * enroll_user, cx_face_dir_e face_dir, int timeout);

/**
 * @description: verify face
 * @param [in] *hdl: face ai handler
 * @param [int] timeout: timeout
 * @return [int]: -1: failt >=0: user id
 */
int cx_face_verify(const cx_srv_hdl_t hdl, int timeout);

/**
 * @description: get all registered user id
 * @param [in] *hdl: face ai handler
 * @param [inout] user_ids: user id array
 * @param [in] id_count: size of the user id array //
 * @return [int]: the number of user id, return -1 when failed
 */
int cx_face_get_all_user_id(const cx_srv_hdl_t hdl, uint32_t user_ids[], int id_count);

/**
 * @description: get user info
 * @param [in] *hdl: face ai handler
 * @param [in] user_id: user id
 * @param [out] *info: 
 * @return [int]: result code, return 0 when success
 */
int cx_face_get_user_info(const cx_srv_hdl_t hdl, uint32_t user_id, cx_face_user_info_t *info);

/**
 * @description: delete user info
 * @param [in] *hdl: face ai handler
 * @param [in] user_id: user id, CX_FACE_DELETE_ALL_USER_ID: delete all users
 * @return [int]: result code, return 0 when success
 */
int cx_face_delete_user(const cx_srv_hdl_t hdl, uint32_t user_id);

int cx_face_set_input_path(const cx_srv_hdl_t hdl, const char* left_img_path, const char* right_img_path);

int cx_face_set_threshold(const cx_srv_hdl_t hdl, const cx_face_ai_threshold_t *threshold);

#ifdef __cplusplus
}
#endif
#endif // __FACE_AI_SERVICE_H__