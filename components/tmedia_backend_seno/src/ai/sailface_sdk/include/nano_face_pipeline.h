//
// Copyright (c) 2022 Alibaba.inc,  All rights reserved.
//
#ifndef _NANO_FACE_PIPELINE_H_
#define _NANO_FACE_PIPELINE_H_

#include "face_net_types.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*********************************************/
/****** for single model init/uninit *********/
/****** APP should manage the pipeline ********/
/*********************************************/
/**
 * @brief Create FaceNet session, call it before any model init
 * 
 * @param[in] p_frame, FaceNet_ImageFrame to indicate input image shape and format
 * @param[in] image_num, input image numbers
 * @param[in] license, license info for auth
 * @return FaceNetError 
 */
FaceNetError FaceNet_Nano_Create(FaceNet_ImageFrame* p_frame, int image_num, FaceNet_License* license);

/**
 * @brief model init,call individually if multi-models should be init
 * 
 * @param[in] module_params
 * @return FaceNetError 
 */
FaceNetError FaceNet_Nano_Init(FaceNet_Model_Params_t* module_params);

/**
 * @brief set input image info before calling first forward. 
 * NOTE: The image data pointer will be used in pipeline, DO NOT cleanup before FaceNet_Nano_Destroy      
 * 
 * @param[in] p_frame, FaceNet_ImageFrame to indicate input image shape and format
 * @param[in] idx, index of the image for detect
 * @return FaceNetError 
 */
FaceNetError FaceNet_Nano_Set_Input(FaceNet_ImageFrame* p_frame, int idx);

/**
 * @brief set input image info before calling first forward. 
 * NOTE: The image data pointer will be used in pipeline, DO NOT cleanup before FaceNet_Nano_Destroy      
 * 
 * @param[in] p_frame, FaceNet_ImageFrame to indicate input image shape and format
 * @param[in] image_num, input image numbers 
 * @return FaceNetError 
 */
FaceNetError FaceNet_Nano_Set_StereoDepth_Input(FaceNet_ImageFrame* p_frame, int image_num);

/**
 * @brief single model run, call individually if multi-models should be performed
 * 
 * @param[in] id, module id
 * @param[in] param, module param 
 * @param[out] result, module forward result
 * @return int, return FaceNetError
 */
FaceNetError FaceNet_Nano_Forward(FaceNet_Model_ID_t id, FaceNet_Nano_Result *result);

/**
 * @brief set pipeline threshold params, call it after FaceNet_Nano_Create.
 * It will use default threshold if did not call this function
 * @param[in] param, threshold param 
 */
void FaceNet_Nano_Set_Params(FaceNet_Threshold *param);

/**
 * @brief get pipeline threshold params
 * 
 * @return FaceNet_Threshold* 
 */
FaceNet_Threshold* FaceNet_Nano_Get_Params();

/**
 * @brief compare face feature
 * 
 * @param[in] feature, feature to be compared
 * @param[in] feature_list, feature list in DB
 * @param[in] id_list, feature id list
 * @param[in] id_num, feature list numbers
 * @param[out] score, similarity score
 * @return int, index of max similarity, return -1 if not pass threshold check
 */
int FaceNet_Nano_Compare_Feature(FaceNet_Data *feature, float **feature_list, uint16_t *id_list, int id_num, float* score);


/**
 * @brief model uninit, call individually
 * 
 * @param[in] id module id
 */
void FaceNet_Nano_Uninit(FaceNet_Model_ID_t id);

/**
 * @brief Destroy FaceNet session, call it after model uninit
 * 
 */
void FaceNet_Nano_Destroy();

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _NANO_FACE_PIPELINE_H_ */
