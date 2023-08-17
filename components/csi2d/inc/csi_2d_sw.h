/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _CSI_2D_SW_H_
#define _CSI_2D_SW_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

void csi_2d_rgb32_alpha_blending_sw(uint32_t *src, int src_rb, uint32_t *dst, int dst_rb, int width, int height);

void csi_2d_rgb32_blending_row_sw(uint32_t *dst, uint32_t *src, int count);

void csi_2d_rgb32_alpha_blending_row_sw(uint32_t *dst, uint32_t *src, int count, uint8_t alpha);

void csi_2d_rgb32_transform_bilinear_row_sw(const uint8_t *src, int rb, const uint32_t *xy, int count, uint32_t *dst,
                                            unsigned alpha);

void csi_2d_rgb32_transform_bilinear_row_xy_sw(const uint8_t *src, int rb, const uint32_t *xy, int count, uint32_t *dst,
                               unsigned alpha);

void csi_2d_rgb32_mask_blending_a8_sw(uint32_t *dst, int dstRB, const uint8_t *mask, int maskRB, uint32_t color,
                                    int width, int height);

void csi_2d_rgb32_fill_color_row_sw(uint32_t *dst, uint32_t *src, uint32_t color, int count);

void csi_2d_rgb32_gen_xy_row_sw(int fx, int fy, int dx, int dy, int maxX, int maxY, int oneX, int oneY, uint32_t *xy, int count);

void csi_2d_rgb32_gen_x_row_sw(int fx, int dx, int maxX, int oneX, uint32_t *xy, int count);

void csi_2d_rgb32_transform_nofliter_row_sw(const uint8_t *s, const uint32_t *xy, int count, uint32_t *dst,
                               unsigned alpha);

void csi_2d_rgb32_memset32_sw(uint32_t *dst, uint32_t color, int count);

void filter_and_scale_by_alpha(unsigned x, unsigned y, uint32_t a00, uint32_t a01, uint32_t a10, uint32_t a11,
                                      uint32_t *dstColor, unsigned alphaScale);

#ifdef __cplusplus
}
#endif

#endif