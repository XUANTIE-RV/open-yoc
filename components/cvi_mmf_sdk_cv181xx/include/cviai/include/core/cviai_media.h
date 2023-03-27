#ifndef _CVIAI_MEDIA_H_
#define _CVIAI_MEDIA_H_
#include <cvi_comm_vb.h>
#include <cvi_sys.h>

#define DLL_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup core_media Convert Buffer or File to VIDEO_FRAME_INFO_S
 * \ingroup core_cviaicore
 */
/**@{*/

/**
 * @brief Convert given image buffer to VB frame.
 *
 * @param buffer The input image buffer.
 * @param width Input image width.
 * @param height Input image height.
 * @param stride Input image stride.
 * @param inFormat Input image buffer format.
 * @param blk VB block id.
 * @param frame Output read image.
 * @param outFormat Set output format, only supports RGB, BGR, planar.
 * @return int Return CVI_SUCCESS if read succeed.
 */
DLL_EXPORT CVI_S32 CVI_AI_Buffer2VBFrame(const uint8_t *buffer, uint32_t width, uint32_t height,
                                         uint32_t stride, const PIXEL_FORMAT_E inFormat,
                                         VB_BLK *blk, VIDEO_FRAME_INFO_S *frame,
                                         const PIXEL_FORMAT_E outFormat);

/**
 * @brief Read image from given path and return a VIDEO_FRAME_INFO_S allocated from VB block.
 *
 * @param filepath GIven image path.
 * @param blk VB block id.
 * @param frame Output read image.
 * @param format Set output format, only supports RGB, BGR, planar.
 * @return int Return CVI_SUCCESS if read succeed.
 */
DLL_EXPORT CVI_S32 CVI_AI_ReadImage(const char *filepath, VB_BLK *blk, VIDEO_FRAME_INFO_S *frame,
                                    const PIXEL_FORMAT_E format);

/**@}*/


DLL_EXPORT CVI_S32 CVI_AI_LoadBinImage(const char *filepath, VB_BLK *blk, VIDEO_FRAME_INFO_S *frame,
                                    PIXEL_FORMAT_E format);
#ifdef __cplusplus
}
#endif

#endif  // End of _CVIAI_MEDIA_H_
