/**
 * @file crc.h
 * @author wenjf
 * @brief
 * @version 0.1
 * @date 2018-12-06
 *
 * @copyright Copyright (c) 2018
 *
 */
#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

enum CRC_MODE
{
	CRC_CUSTOM     = 0xff,

	// poly = 1021
	CRC_CCIT_KERMIT		= 0,
	CRC_CCIT_XMODEM		= 1,
	CRC_CCIT_FALSE		= 2,
	CRC_CCIT_GENIBUS	= 3,
	CRC_CCIT_X_25		= 4,
	CRC_CCIT_MCRF4XX    = 5,
	CRC_CCIT_AUG_CCIT   = 6,
	CRC_CCIT_TMS37157   = 7,
	CRC_CCIT_RIELLO     = 8,
	CRC_CCIT_CRC_A      = 9,

	// ----------------------------
	// 2021 - 04 - 16 暂不支持
	// 
	// poly = 8005
	// CRC_16_BUYPASS      = 11,
	// CRC_16_ARC          = 12,
	// CRC_16_MODBUS       = 13,
	// CRC_16_MAXIM        = 14,
	// CRC_16_USB          = 15,
	// CRC_16_DDS_110      = 16,

	CRC_MODE_END,
};

// crc status
#define CRC_ST_SUCCESS			0x00
#define CRC_ST_ERR				0x01

typedef struct _crc_t
{
	/**
	 * @brief 0. 自定义参数
	 *
	 * @param init 初值
	 * @param xor 结果异或值
	 * @param reverse_in 输入数据是否需要按位翻转
	 * @param reverse_out 输出数据是否需要按位翻转
	 */
	void (*custom)(uint16_t init, uint16_t xor, uint8_t reverse_in, uint8_t reverse_out);

	/**
	 * @brief 1. 初始化
	 *     若自定义参数，则需要先执行**_custom函数
	 * @param mode 模式（宏定义CRC_CCIT_xx）
	 * @param endian 结果大小端
	 * @return uint8_t 状态（宏定义CRC_ST_xx）
	 */
	uint8_t(*init)(uint8_t mode, uint8_t endian);

	/**
	 * @brief 2. 更新数据（可连续）
	 *
	 * @param bytes 数据内容
	 * @param len 数据长度
	 * @return uint8_t 状态（宏定义CRC_ST_xx）
	 */
	uint8_t(*update)(uint8_t* data, uint32_t len);

	/**
	 * @brief 3. 取CRC结果
	 *
	 * @return uint16_t
	 */
	uint16_t(*get_result)(void);
}crc_t;

extern crc_t g_crc;

#endif
