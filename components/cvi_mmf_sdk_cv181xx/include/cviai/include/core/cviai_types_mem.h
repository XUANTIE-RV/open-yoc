#ifndef _CVIAI_TYPES_FREE_H_
#define _CVIAI_TYPES_FREE_H_
#include "core/core/cvai_core_types.h"
#include "core/face/cvai_face_types.h"
#include "core/object/cvai_object_types.h"

#ifdef __cplusplus
DLL_EXPORT void CVI_AI_FreeCpp(cvai_feature_t *feature);
DLL_EXPORT void CVI_AI_FreeCpp(cvai_pts_t *pts);
DLL_EXPORT void CVI_AI_FreeCpp(cvai_face_info_t *face_info);
DLL_EXPORT void CVI_AI_FreeCpp(cvai_face_t *face);
DLL_EXPORT void CVI_AI_FreeCpp(cvai_object_info_t *obj_info);
DLL_EXPORT void CVI_AI_FreeCpp(cvai_object_t *obj);
DLL_EXPORT void CVI_AI_FreeCpp(cvai_image_t *image);

DLL_EXPORT void CVI_AI_CopyInfoCpp(const cvai_face_info_t *info, cvai_face_info_t *infoNew);
DLL_EXPORT void CVI_AI_CopyInfoCpp(const cvai_object_info_t *info, cvai_object_info_t *infoNew);
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPORT void CVI_AI_FreeFeature(cvai_feature_t *feature);
DLL_EXPORT void CVI_AI_FreePts(cvai_pts_t *pts);
DLL_EXPORT void CVI_AI_FreeFaceInfo(cvai_face_info_t *face_info);
DLL_EXPORT void CVI_AI_FreeFace(cvai_face_t *face);
DLL_EXPORT void CVI_AI_FreeObjectInfo(cvai_object_info_t *obj_info);
DLL_EXPORT void CVI_AI_FreeObject(cvai_object_t *obj);
DLL_EXPORT void CVI_AI_FreeImage(cvai_image_t *image);

DLL_EXPORT void CVI_AI_CopyFaceInfo(const cvai_face_info_t *info, cvai_face_info_t *infoNew);
DLL_EXPORT void CVI_AI_CopyFaceMeta(const cvai_face_t *src, cvai_face_t *dst);
DLL_EXPORT void CVI_AI_CopyObjectInfo(const cvai_object_info_t *src, cvai_object_info_t *dst);
DLL_EXPORT void CVI_AI_CopyObjectMeta(const cvai_object_t *src, cvai_object_t *dst);

DLL_EXPORT void CVI_AI_CopyImage(const cvai_image_t *src_image, cvai_image_t *dst_image);

#ifdef __cplusplus
}
#endif
#endif
