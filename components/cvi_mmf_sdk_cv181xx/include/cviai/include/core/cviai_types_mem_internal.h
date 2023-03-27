#ifndef _CVIAI_TYPES_MEM_INTERNAL_H_
#define _CVIAI_TYPES_MEM_INTERNAL_H_
#include "core/cviai_types_mem.h"
#include "core/face/cvai_face_types.h"
#include "cviai_log.hpp"

#include <string.h>

inline void CVI_AI_MemAlloc(const uint32_t unit_len, const uint32_t size, const feature_type_e type,
                            cvai_feature_t *feature) {
  if (feature->size != size || feature->type != type) {
    free(feature->ptr);
    feature->ptr = (int8_t *)malloc(unit_len * size);
    feature->size = size;
    feature->type = type;
  }
}

inline void CVI_AI_MemAlloc(const uint32_t size, cvai_pts_t *pts) {
  if (pts->size != size) {
    free(pts->x);
    free(pts->y);
    pts->x = (float *)malloc(sizeof(float) * size);
    pts->y = (float *)malloc(sizeof(float) * size);
    pts->size = size;
  }
}

inline void CVI_AI_MemAlloc(const uint32_t size, cvai_face_t *meta) {
  if (meta->size != size) {
    for (uint32_t i = 0; i < meta->size; i++) {
      CVI_AI_FreeCpp(&meta->info[i]);
      free(meta->info);
    }
    meta->size = size;
    meta->info = (cvai_face_info_t *)malloc(sizeof(cvai_face_info_t) * meta->size);
  }
}

inline void CVI_AI_MemAlloc(const uint32_t size, cvai_object_t *meta) {
  if (meta->size != size) {
    for (uint32_t i = 0; i < meta->size; i++) {
      CVI_AI_FreeCpp(&meta->info[i]);
      free(meta->info);
    }
    meta->size = size;
    meta->info = (cvai_object_info_t *)malloc(sizeof(cvai_object_info_t) * meta->size);
  }
}

inline void CVI_AI_MemAllocInit(const uint32_t size, cvai_object_t *meta) {
  CVI_AI_MemAlloc(size, meta);
  for (uint32_t i = 0; i < meta->size; ++i) {
    memset(&meta->info[i], 0, sizeof(cvai_object_info_t));
    meta->info[i].bbox.x1 = -1;
    meta->info[i].bbox.x2 = -1;
    meta->info[i].bbox.y1 = -1;
    meta->info[i].bbox.y2 = -1;

    meta->info[i].name[0] = '\0';
    meta->info[i].classes = -1;
  }
}

inline void CVI_AI_MemAllocInit(const uint32_t size, const uint32_t pts_num, cvai_face_t *meta) {
  CVI_AI_MemAlloc(size, meta);
  for (uint32_t i = 0; i < meta->size; ++i) {
    meta->info[i].bbox.x1 = -1;
    meta->info[i].bbox.x2 = -1;
    meta->info[i].bbox.y1 = -1;
    meta->info[i].bbox.y2 = -1;

    meta->info[i].name[0] = '\0';
    meta->info[i].head_pose.yaw = 0;
    meta->info[i].head_pose.pitch = 0;
    meta->info[i].head_pose.roll = 0;
    memset(&meta->info[i].head_pose.facialUnitNormalVector, 0, sizeof(float) * 3);

    memset(&meta->info[i].feature, 0, sizeof(cvai_feature_t));
    if (pts_num > 0) {
      meta->info[i].pts.x = (float *)malloc(sizeof(float) * pts_num);
      meta->info[i].pts.y = (float *)malloc(sizeof(float) * pts_num);
      meta->info[i].pts.size = pts_num;
      for (uint32_t j = 0; j < meta->info[i].pts.size; ++j) {
        meta->info[i].pts.x[j] = -1;
        meta->info[i].pts.y[j] = -1;
      }
    } else {
      memset(&meta->info[i].pts, 0, sizeof(meta->info[i].pts));
    }
  }
}

inline void __attribute__((always_inline)) floatToBF16(float *input, uint16_t *output) {
  uint16_t *p = reinterpret_cast<uint16_t *>(input);
  (*output) = p[1];
}

#endif  // End of _CVIAI_TYPES_MEM_INTERNAL_H_