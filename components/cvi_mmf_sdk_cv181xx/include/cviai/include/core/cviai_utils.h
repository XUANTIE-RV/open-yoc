#ifndef _CVIAI_UTILS_H_
#define _CVIAI_UTILS_H_
#include "cviai_core.h"
#include "face/cvai_face_types.h"
// #include "object/cvai_object_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_REDUNDANT
/**
 * \addtogroup core_utils AI Utilities for Preprocessing and Post-processing
 * \ingroup core_cviaicore
 */
/**@{*/

/**
 * @brief Do quantize scale for a given VIDEO_FRAME_INFO_S, but the quantized_factor and
 *        quantized_mean have to calculate manually.
 *
 * @param handle An AI SDK handle.
 * @param frame Input frame.
 * @param output Output frame.
 * @param quantized_factor Quantized factor.
 * @param quantized_mean Quantized mean, must be positive.
 * @param thread Working thread id of vpss thread. A new thread will be created if thread doesn't
 * exist.
 * @param timeout Vpss wait timeout value.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_SQPreprocessRaw(cviai_handle_t handle, const VIDEO_FRAME_INFO_S *frame,
                                          VIDEO_FRAME_INFO_S *output, const float quantized_factor,
                                          const float quantized_mean, const uint32_t thread,
                                          uint32_t timeout);

/**
 * @brief Do Quantize scale for a given VIDEO_FRAME_INFO_S.
 *        The formula of scale is (factor * x - mean).
 *
 * @param handle An AI SDK handle.
 * @param frame Input frame.
 * @param output Output frame.
 * @param factor Factor.
 * @param mean Mean.
 * @param quantize_threshold Threshold for quantization.
 * @param thread Working thread id of vpss thread. A new thread will be created if thread doesn't
 * exist.
 * @param timeout Vpss wait timeout value.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_SQPreprocess(cviai_handle_t handle, const VIDEO_FRAME_INFO_S *frame,
                                       VIDEO_FRAME_INFO_S *output, const float factor,
                                       const float mean, const float quantize_threshold,
                                       const uint32_t thread, uint32_t timeout);

/**
 * @brief Dequantize an int8_t output result from NN.
 *
 * @param quantizedData Input quantized data.
 * @param data Output float data.
 * @param bufferSize Size of the buffer.
 * @param dequantizeThreshold Dequantize threshold.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_Dequantize(const int8_t *quantizedData, float *data,
                                     const uint32_t bufferSize, const float dequantizeThreshold);

/**
 * @brief Do softmax on buffer.
 *
 * @param inputBuffer Input float buffer.
 * @param outputBuffer Output result.
 * @param bufferSize Size of the buffer.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_SoftMax(const float *inputBuffer, float *outputBuffer,
                                  const uint32_t bufferSize);
#endif

/**
 * @brief Do non maximum suppression on cvai_face_t.
 *
 * @param face Input cvai_face_t.
 * @param faceNMS Output result.
 * @param threshold NMS threshold.
 * @param method Support 'u' and 'm'. (intersection over union and intersection over min area)
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_FaceNMS(const cvai_face_t *face, cvai_face_t *faceNMS,
                                  const float threshold, const char method);

#ifdef USE_REDUNDANT
/**
 * @brief Do non maximum suppression on cvai_object_t.
 *
 * @param obj Input cvai_object_t.
 * @param objNMS Output result.
 * @param threshold NMS threshold.
 * @param method Support 'u' and 'm'. (intersection over union and intersection over min area)
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_ObjectNMS(const cvai_object_t *obj, cvai_object_t *objNMS,
                                    const float threshold, const char method);

/**
 * @brief
 *
 * @param inFrame Input frame.
 * @param metaWidth The face meta width used for coordinate recovery.
 * @param metaHeight The face meta height used for coordinate recovery.
 * @param info The face info.
 * @param outFrame Output face align result. Frame must be preallocated.
 * @param enableGDC Enable GDC hardware support.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_FaceAlignment(VIDEO_FRAME_INFO_S *inFrame, const uint32_t metaWidth,
                                        const uint32_t metaHeight, const cvai_face_info_t *info,
                                        VIDEO_FRAME_INFO_S *outFrame, const bool enableGDC);
#endif

/**
 * @brief
 *
 * @param image Output image.
 * @param height The height of output image.
 * @param width The width of output image.
 * @param fmt The pixel format of output image.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_CreateImage(cvai_image_t *image, uint32_t height, uint32_t width,
                                      PIXEL_FORMAT_E fmt);

/**
 * @brief
 *
 * @param height The height of the image.
 * @param width The width of the image.
 * @param fmt The pixel format of the image.
 * @return int Return CVI_SUCCESS on success.
 */
DLL_EXPORT CVI_S32 CVI_AI_EstimateImageSize(uint64_t *size, uint32_t height, uint32_t width,
                                            PIXEL_FORMAT_E fmt);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif  // End of _CVIAI_UTILS_H_
