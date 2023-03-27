#ifndef _CVI_FACE_TYPES_H_
#define _CVI_FACE_TYPES_H_
#include "core/core/cvai_core_types.h"

/** @struct cvai_head_pose_t
 *  @ingroup core_cviaicore
 *  @brief The data structure for the head pose output.
 *
 *  @var cvai_head_pose_t::faacialUnitNormalVector
 *  The Normal vector for the face.
 *  @var cvai_head_pose_t::roll
 *  The roll angle of the head pose.
 *  @var cvai_head_pose_t::pitch
 *  The pitch angle of the head pose.
 *  @var cvai_head_pose_t::yaw
 *  The yaw angle of the head pose.
 */
typedef struct {
  float yaw;
  float pitch;
  float roll;

  // Facial normal means head direction.
  float facialUnitNormalVector[3];  // 0: x-axis, 1: y-axis, 2: z-axis
} cvai_head_pose_t;

/** @struct cvai_face_info_t
 *  @ingroup core_cviaicore
 *  @brief The data structure for storing a single face information.
 *
 *  @var cvai_face_info_t::name
 *  A human readable name.
 *  @var cvai_face_info_t::bbox
 *  The bounding box of a face. Refers to the width, height from cvai_face_t.
 *  @var cvai_face_info_t::pts
 *  The point to describe the point on the face.
 *  @var cvai_face_info_t::head_pose;
 *  The head pose.
 *
 *  @see cvai_face_t
 */

typedef struct {
  char name[128];
  cvai_bbox_t bbox;
  cvai_pts_t pts;
  cvai_feature_t feature;
  cvai_head_pose_t head_pose;
} cvai_face_info_t;

/** @struct cvai_face_t
 *  @ingroup core_cviaicore
 *  @brief The data structure for storing face meta.
 *
 *  @var cvai_face_t::size
 *  The size of the info.
 *  @var cvai_face_t::width
 *  The current width. Affects the coordinate recovery of bbox and pts.
 *  @var cvai_face_t::height
 *  The current height. Affects the coordinate recovery of bbox and pts.
 *  @var cvai_face_t::info
 *  The information of each face.
 *  @var cvai_face_t::dms
 *  The dms of face.
 *
 *  @see cvai_face_info_t
 */
typedef struct {
  uint32_t size;
  uint32_t width;
  uint32_t height;
  meta_rescale_type_e rescale_type;
  cvai_face_info_t* info;
} cvai_face_t;

#endif
