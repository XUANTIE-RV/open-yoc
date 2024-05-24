//
// Copyright (c) 2022 Alibaba.inc,  All rights reserved.
//
#ifndef _FACENET_TYPES_H_
#define _FACENET_TYPES_H_

#if defined(__cplusplus)
#include <cstddef>
#endif
#include <stdint.h>

#ifndef FACE_NAME_MAX_LEN
#define FACE_NAME_MAX_LEN 31
#endif

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/**
 * The enumeration of pixel format
 */
typedef enum FaceNet_PixelFormat {
  FACENET_PIXFMT_RGB = 0, /**< RGB888 from low address to high */
  FACENET_PIXFMT_BGR,     /**< BGR888 from low address to high */
  FACENET_PIXFMT_GRAY,    /**< Gray8 */
  FACENET_PIXFMT_GRAY888, /**< Gray888 */
  FACENET_PIXFMT_GRAY16H, /**< 16 bits with high 8 bits gray data */
  FACENET_PIXFMT_GRAY16L, /**< 16 bits with low 8 bits gray data */
  FACENET_PIXFMT_GRAY16C, /**< 16 bits with customized 8 bits gray
                             data(bit2-bit9) */
  FACENET_PIXFMT_FLOAT,   /**< Float */
} FaceNet_PixelFormat;

/**
 * The enumeration of error code
 */
typedef enum {
  FACENET_ERR_OK = 0,         /**< No error */
  FACENET_ERR_DETECT_ERR,     /**< detect model inference error */
  FACENET_ERR_LANDMARK_ERR,   /**< landmark model inference error */
  FACENET_ERR_QUALITY_ERR,    /**< quality model inference error */
  FACENET_ERR_ANTILOCAL_ERR,  /**< local liveness model inference error */
  FACENET_ERR_ANTIGLOBAL_ERR, /**< global liveness model inference error */
  FACENET_ERR_RECOG_ERR,      /**< recognition model inference error */

  FACENET_ERR_NOT_DETECTED,      /**< no face detected */
  FACENET_ERR_BAD_BRIGHTNESS,    /**< image too dark or too bright */
  FACENET_ERR_BAD_QUALITY,       /**< bad image quality */
  FACENET_ERR_ANTILOCAL_FAILED,  /**< non-living (local) */
  FACENET_ERR_ANTIGLOBAL_FAILED, /**< non-living (global) */
  FACENET_ERR_ANTIDEPTH_FAILED,  /**< non-living (depth) */
  FACENET_ERR_NO_MATCH_FACE,     /**< no match face in the database */
  FACENET_ERR_ALREADY_EXIST,     /**< the user already exists */
  FACENET_ERR_DATABASE_FULL,     /**< face database is full */
  FACENET_ERR_CALIB_ERR,         /**< stereo ir calib file md5 check err */
  FACENET_ERR_BAD_LANDMARK,      /**< bad landmark pt score */

  FACENET_ERR_MEM_ALLOC,      /**< memory allocate error */
  FACENET_ERR_MEM_NOT_ENOUGH, /**< request memory is larger than the buffer */
  FACENET_ERR_INVALID_INPUT,  /**< invalid input */

  FACENET_ERR_AUTHENTICATE_FAILED,   /**< authenticate failed */
  FACENET_ERR_MODEL_INCOMPATIBLE,    /**< incompatible model with the engine */
  FACENET_ERR_MODEL_CORRUPT,         /**< corrupted model */
  FACENET_ERR_MODEL_CORRUPT_WEIGHTS, /**< corrupted weights */
  FACENET_ERR_MODEL_CORRUPT_BIAS,    /**< corrupted bias */
  FACENET_ERR_INVALID_PARAM,         /**< invalid parameters */
  FACENET_ERR_INTERNAL,              /**< engine internal error */
  FACENET_ERR_MODEL_PARSE_FAILED,    /**< model parse error */
  FACENET_ERR_UNKNOWN_ERR = 99, /**< generic/unknown error */
} FaceNetError;

typedef enum {
  FACENET_INT_FRAME_IDX_RGB, /**< RGB frame ID or LEFT IR frame ID for stereo IR*/
  FACENET_INT_FRAME_IDX_IR,  /**< IR frame ID or RIGHT IR frame ID for stereo IR */
  FACENET_INT_FRAME_IDX_3D,  /**< Depth frame ID */
  FACENET_INT_FRAME_IDX_NUM
} FaceNet_FrameID;

typedef enum {
  FACE_MODEL_ID_ALL_MODELS = 0x00,
  FACE_MODEL_ID_DETECTION = 0x01,
  FACE_MODEL_ID_RGB_IR_LIVENESS,  // RGB liveness model or single IR liveness model
  FACE_MODEL_ID_LANDMARK,
  FACE_MODEL_ID_POSE,
  FACE_MODEL_ID_QUALITY,
  FACE_MODEL_ID_RECO,
  FACE_MODEL_ID_DEPTH_IR_LIVENESS, // depth IR liveness model
  FACE_MODEL_ID_GLOBAL_LIVENESS, // NO USE
  //more models added here
  FACE_MODEL_ID_MAX_NUMS = 0x0A,
} FaceNet_Model_ID_t;

typedef struct {
  FaceNet_Model_ID_t id;  // model id
  void *weight;           // model weight, set NULL if graph has build inside
  uint32_t weight_size;   // weight size
  void *graph;            // model graph, set NULL if graph has build inside
  uint32_t graph_size;    // graph size, set 0 if graph has build inside
  void *reserved;            // reserved, for dual ir calibration, set remap matrix here
  uint32_t reserved_size;    // reserved size, set 0 if reserved is NULL
}FaceNet_Model_t;

typedef struct {
  int numbers;
  FaceNet_Model_t model_params[FACE_MODEL_ID_MAX_NUMS];
}FaceNet_Model_Params_t;

/*********************************************/

typedef struct FaceNet_Data_s {
  int w;        /**< width of data */
  int h;        /**< height of data */
  int c;        /**< channel number of data */
  int elemSize; /**< element size of data */
  void *data;   /**< pointer to data */
} FaceNet_Data;

typedef struct FaceNet_ImageFrame_s {
  int w;                   /**< width of data */
  int h;                   /**< height of data */
  int c;                   /**< channel number of data */
  FaceNet_PixelFormat fmt; /**< pixel format */
  uint8_t *data;           /**< pointer to data, can be null in init*/
} FaceNet_ImageFrame;

typedef struct FaceNet_ObjBox_s {
  int x1;
  int y1;
  int x2;
  int y2;
  float score;
} FaceNet_ObjBox;

typedef struct FaceNet_Threshold_s {    
  float   facedetect_thres;       /* face detect */
  float   ptscore_thres;          /* keypoint score */
  float   occusion_thres;         /* occusion, no use */
  float   maskclassify_thres;     /* maskclassify, no use */
  float   living2d_thres;         /* 2d, ir liveness threshold */
  float   livingRGB_thres;         /* RGB, rgb liveness threshold */
  float   living3d_thres;         /* 3d, depth liveness threshold */
  float   facequality_thres;      /* face quality*/
  float   headpose_thres[3];       /* head pose, no use */
  float   face_reco_thres;        /* face recognize threshold*/
}FaceNet_Threshold;

typedef struct FaceNet_License_s {
  uint32_t size; // size of license data
  unsigned char* data; // point of license data
}FaceNet_License;

typedef struct FaceNet_Nano_Result_s {
  FaceNetError err_code;
  /**< detect result, only one face support */
  uint8_t face_count;
  FaceNet_ObjBox face_box;
  /**< feature data */
  FaceNet_Data feature;
  /**< face pose: roll, pitch, yaw  */
  float pose[3];
  /**< quality score: larger is better  */
  uint32_t qualityScore;
  /**< NO USE. global liveness score: larger means much like real  */
  uint32_t antiglScore;
  /**< ir/rgb liveness score: larger means much like real  */
  uint32_t antilcScore;
  /**< depth liveness score: larger means much like real */
  uint32_t antidpScore;
  /**< landmark score: lower is better*/
  float    landmarkScore;
  /**< [0~9]: [x0,y0,...,x4,y4], landmark of original input image(left)*/
  float    lamdmarkPoints[10];
}FaceNet_Nano_Result;

#if defined(__cplusplus)
}
#endif // __cplusplus
#endif /* _FACENET_TYPES_H_ */
