/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _CSI_2D_DIRECT_H_
#define _CSI_2D_DIRECT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void csi_2d_rgb32_alpha_blending(uint32_t *src, int src_rb, uint32_t *dst, int dst_rb, int width, int height);

void csi_2d_rgb32_blending_row(uint32_t *dst, uint32_t *src, int count);

void csi_2d_rgb32_alpha_blending_row(uint32_t *dst, uint32_t *src, int count, uint8_t alpha);

void csi_2d_rgb32_transform_bilinear_row(const uint8_t *src, int rb, const uint32_t *xy, int count, uint32_t *dst,
                                            unsigned alpha);

void csi_2d_rgb32_transform_bilinear_row_xy(const uint8_t *src, int rb, const uint32_t *xy, int count, uint32_t *dst,
                               unsigned alpha);

void csi_2d_rgb32_mask_blending_a8(uint32_t *dst, int dstRB, const uint8_t *mask, int maskRB, uint32_t color,
                                    int width, int height);

void csi_2d_rgb32_fill_color_row(uint32_t *dst, uint32_t *src, uint32_t color, int count);

void csi_2d_rgb32_gen_xy_row(int fx, int fy, int dx, int dy, int maxX, int maxY, int oneX, int oneY, uint32_t *xy, int count);

void csi_2d_rgb32_gen_x_row(int fx, int dx, int maxX, int oneX, uint32_t *xy, int count);

void csi_2d_rgb32_transform_nofliter_row(const uint8_t *s, const uint32_t *xy, int count, uint32_t *dst,
                               unsigned alpha);

void csi_2d_rgb32_memset32(uint32_t *dst, uint32_t color, int count);

#ifdef __cplusplus
}
#endif

#endif