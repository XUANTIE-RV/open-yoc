/*
 * bl_misc.h
 *
 */

#ifndef COMPONENTS_CHIP_BL606_BL_DRV_COMMON_BL_MISC_H_
#define COMPONENTS_CHIP_BL606_BL_DRV_COMMON_BL_MISC_H_
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

/* DEBUG */
#ifdef DEBUG
void bl_check_failed(uint8_t *file, uint32_t line);
#define BL_CHECK_PARAMETE(expr) ((expr) ? (void)0 : bl_check_failed((uint8_t *)__FILE__, __LINE__))
#else
#define BL_CHECK_PARAMETE(expr) ((void)0)
#endif /* DEBUG */

/* function */
#ifdef BIT
#undef BIT
#define BIT(n) (1UL << (n))
#else
#ifndef BIT
#define BIT(n) (1UL << (n))
#endif
#endif

/* enum */
/**
 * @brief Error type definition
 */
typedef enum {
    bl_success   = 0,
    bl_error     = 1,
    bl_timeout   = 2,
    bl_invalid   = 3, /* invalid arguments */
    bl_noresc    = 4   /* no resource or resource temperary unavailable */
} bl_err_flg_type;

/**
 * @brief Functional type definition
 */
typedef enum {
    bl_disable = 0,
    bl_enable  = 1,
} bl_fun_flg_type;

/**
 * @brief Status type definition
 */
typedef enum {
    bl_reset = 0,
    bl_set = 1,
} bl_set_flg_type;

/**
 * @brief Mask type definition
 */
typedef enum {
    bl_unmask = 0,
    bl_mask   = 1
} bl_mask_flg_type;

/**
 * @brief Logical status Type definition
 */
typedef enum {
    bl_logic_lo = 0,
    bl_logic_hi = !bl_logic_lo
} bl_logical_flg_type;

/**
 * @brief Active status Type definition
 */
typedef enum {
    bl_deactive = 0,
    bl_active   = !bl_deactive
} bl_active_flg_type;


/* Std driver attribute macro*/
#define ATTR_CLOCK_SECTION         __attribute__((section(".sclock_rlt_code")))
#define ATTR_CLOCK_CONST_SECTION   __attribute__((section(".sclock_rlt_const")))
#define ATTR_TCM_SECTION           __attribute__((section(".tcm_code")))
#define ATTR_TCM_CONST_SECTION     __attribute__((section(".tcm_const")))
#define ATTR_DTCM_SECTION          __attribute__((section(".tcm_data")))
#define ATTR_HSRAM_SECTION         __attribute__((section(".hsram_code")))
#define ATTR_DMA_RAM_SECTION       __attribute__((section(".system_ram")))
#define ATTR_HBN_RAM_SECTION       __attribute__((section(".hbn_ram_code")))
#define ATTR_HBN_RAM_CONST_SECTION __attribute__((section(".hbn_ram_data")))
#define ATTR_EALIGN(x)             __attribute((aligned(x)))
#define ATTR_FALLTHROUGH()         __attribute__((fallthrough))
#define ATTR_USED                  __attribute__((__used__))

/** @defgroup  Memory common function
 *  @{
 */
void *bl_arch_memcpy(void *dst, const void *src, uint32_t n);
uint32_t *bl_arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n);
void *bl_arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n);
void *bl_arch_memset(void *s, uint8_t c, uint32_t n);
uint32_t *bl_arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n);
int bl_arch_memcmp(const void *s1, const void *s2, uint32_t n);
void bl_memcopy_to_fifo(void *fifo_addr, uint8_t *data, uint32_t length);
void bl_fifocopy_to_mem(void *fifo_addr, uint8_t *data, uint32_t length);

#ifdef DEBUG
#define check_failed       bl_check_failed
#endif
#define arch_memcpy        bl_arch_memcpy
#define arch_memcpy4       bl_arch_memcpy4
#define arch_memcpy_fast   bl_arch_memcpy_fast
#define arch_memset        bl_arch_memset
#define arch_memset4       bl_arch_memset4
#define arch_memcmp        bl_arch_memcmp
#define memcopy_to_fifo    bl_memcopy_to_fifo
#define fifocopy_to_mem    bl_fifocopy_to_mem
/*@} end of  Memory common function */

#endif /* COMPONENTS_CHIP_BL606_BL_DRV_COMMON_BL_MISC_H_ */
