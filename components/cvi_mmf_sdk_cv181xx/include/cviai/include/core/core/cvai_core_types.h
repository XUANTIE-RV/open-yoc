#ifndef _CVI_CORE_TYPES_H_
#define _CVI_CORE_TYPES_H_

#include <stdint.h>
#include <stdlib.h>
#include "cvi_comm_video.h"


/**
 * \defgroup core_cviaicore CVIAI Core Module
 */

/** @enum feature_type_e
 * @ingroup core_cviaicore
 * @brief A variable type enum to present the data type stored in cvai_feature_t.
 * @see cvai_feature_t
 */
typedef enum {
  TYPE_INT8 = 0, /**< Equals to int8_t. */
  TYPE_UINT8,    /**< Equals to uint8_t. */
  TYPE_INT16,    /**< Equals to int16_t. */
  TYPE_UINT16,   /**< Equals to uint16_t. */
  TYPE_INT32,    /**< Equals to int32_t. */
  TYPE_UINT32,   /**< Equals to uint32_t. */
  TYPE_BF16,     /**< Equals to bf17. */
  TYPE_FLOAT     /**< Equals to float. */
} feature_type_e;

/** @enum meta_rescale_type_e
 * @ingroup core_cviaicore
 * @brief A variable type enum that records the resize padding method.
 */
typedef enum { RESCALE_UNKNOWN, RESCALE_NOASPECT, RESCALE_CENTER, RESCALE_RB } meta_rescale_type_e;

/** @struct cvai_bbox_t
 * @ingroup core_cviaicore
 * @brief A structure to describe an area in a given image with confidence score.
 *
 * @var cvai_bbox_t::x1
 * The left-upper x coordinate.
 * @var cvai_bbox_t::y1
 * The left-upper y coordinate.
 * @var cvai_bbox_t::x2
 * The right-bottom x coordinate.
 * @var cvai_bbox_t::y2
 * The right-bottom y coordinate.
 * @var cvai_bbox_t::score
 * The confidence score.
 */

typedef struct {
  float x1;
  float y1;
  float x2;
  float y2;
  float score;
} cvai_bbox_t;

/** @struct cvai_feature_t
 * @ingroup core_cviaicore
 * @brief A structure to describe feature. Note that the length of the buffer is size *
 * getFeatureTypeSize(type)
 *
 * @var cvai_feature_t::ptr
 * The raw pointer of a feature. Need to convert to correct type with feature_type_e.
 * @var cvai_feature_t::size
 * The buffer size of ptr in unit of type.
 * @var cvai_feature_t::type
 * An enum to describe the type of ptr.
 * @see feature_type_e
 * @see getFeatureTypeSize()
 */
typedef struct {
  int8_t* ptr;
  uint32_t size;
  feature_type_e type;
} cvai_feature_t;

/** @struct cvai_pts_t
 * @ingroup core_cviaicore
 * @brief A structure to describe (x, y) array.
 *
 * @var cvai_pts_t::x
 * The raw pointer of the x coordinate.
 * @var cvai_pts_t::y
 * The raw pointer of the x coordinate.
 * @var cvai_pts_t::size
 * The buffer size of x and y in the unit of float.
 */
typedef struct {
  float* x;
  float* y;
  uint32_t size;
} cvai_pts_t;

/** @struct cvai_4_pts_t
 * @ingroup core_cviaicore
 * @brief A structure to describe 4 2d points.
 *
 * @var cvai_pts_t::x
 * The raw pointer of the x coordinate.
 * @var cvai_pts_t::y
 * The raw pointer of the x coordinate.
 */
typedef struct {
  float x[4];
  float y[4];
} cvai_4_pts_t;

/** @struct cvai_image_t
 * @ingroup core_cviaicore
 * @brief image stucture.
 *
 * @var cvai_image_t::height
 * The height of the image.
 * @var cvai_image_t::width
 * The width of the image.
 * @var cvai_image_t::stride
 * The stride of the image.
 * @var cvai_image_t::pix
 * The pixel data of the image.
 */
typedef struct {
  PIXEL_FORMAT_E pix_format;
  uint8_t* pix[3];
  uint32_t stride[3];
  uint32_t length[3];
  uint32_t height;
  uint32_t width;
} cvai_image_t;

typedef struct {
  float m[2][3];
} cvai_affine_matrix_t;


/**
 * @brief A helper function to get the unit size of feature_type_e.
 * @ingroup core_cviaicore
 *
 * @param type Input feature_type_e.
 * @return const int The unit size of a variable type.
 */

inline int __attribute__((always_inline)) getFeatureTypeSize(feature_type_e type) {
  uint32_t size = 1;
  switch (type) {
    case TYPE_INT8:
    case TYPE_UINT8:
      break;
    case TYPE_INT16:
    case TYPE_UINT16:
    case TYPE_BF16:
      size = 2;
      break;
    case TYPE_INT32:
    case TYPE_UINT32:
    case TYPE_FLOAT:
      size = 4;
      break;
  }
  return size;
}

#define DLL_EXPORT __attribute__((visibility("default")))
#endif