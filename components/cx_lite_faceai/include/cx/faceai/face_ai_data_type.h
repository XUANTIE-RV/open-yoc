/*
 * @Date: 2022-07-21
 * @LastEditTime: 2022-08-18
 * @FilePath: /cr182x_open/components/cx_faceai/include/cx/faceai/face_ai_data_type.h
 * @Description: 
 * 
 * Copyright (c) 2022-2022 by Alibaba Group Holding Limited, All Rights Reserved. 
 */
#ifndef __FACE_AI_DATA_TYPE_H__
#define __FACE_AI_DATA_TYPE_H__

#define CX_FACE_DELETE_ALL_USER_ID -1

typedef enum {
    CX_FACE_VERIFY_RESULT_SUCCESS = 0,
    CX_FACE_VERIFY_RESULT_NO_FACE,
    CX_FACE_VERIFY_RESULT_DETECT_FAILED,
    CX_FACE_VERIFY_RESULT_BAD_POSTURE,
    CX_FACE_VERIFY_RESULT_LIVING_2D_FAILED,
    CX_FACE_VERIFY_RESULT_LIVING_3D_FAILED,
    CX_FACE_VERIFY_RESULT_NOT_MATCH,
    CX_FACE_VERIFY_RESULT_REPEAT,
    CX_FACE_VERIFY_RESULT_TIMEOUT,
    CX_FACE_VERIFY_RESULT_FAILURE,
} cx_face_verify_result_e;

#define CX_FACE_MAX_USRNAME_LEN 32
typedef struct {
    char user_name[CX_FACE_MAX_USRNAME_LEN];
    int admin; //0: admin 1: non-admin -1: ignore
    int user_id;                   
} cx_face_user_info_t;

typedef enum {
    FACE_DIRECTION_ALL= 0x1F,       // face all
    FACE_DIRECTION_UP = 0x10,       // face up
    FACE_DIRECTION_DOWN = 0x08,       // face down
    FACE_DIRECTION_LEFT = 0x04,       // face left
    FACE_DIRECTION_RIGHT = 0x02,       // face right
    FACE_DIRECTION_MIDDLE = 0x01,       // face middle
    FACE_DIRECTION_UNDEFINE = 0x00,       // face undefine
} cx_face_dir_e;

typedef struct {
    cx_face_verify_result_e result;
    int user_id;
} cx_face_verify_result_t;

typedef struct {
    int success; //0:success, 1:fail
    cx_face_user_info_t user_info;
    cx_face_dir_e face_dir;
} cx_face_enroll_result_t;

typedef struct {
  int   facedetect_thres;       /* face detect */
  int   ptscore_thres;          /* keypoint score */
  int   living2d_thres;         /* 2d, ir liveness threshold */
  int   living3d_thres;         /* 3d, depth liveness threshold */
  int   facequality_thres;      /* face quality*/
  int   face_reco_thres;        /* face recognize threshold*/
} cx_face_ai_threshold_t;
#endif // __FACE_AI_DATA_TYPE_H__