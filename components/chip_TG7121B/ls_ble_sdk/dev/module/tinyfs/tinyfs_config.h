#ifndef TINYFS_CONFIG_H_
#define TINYFS_CONFIG_H_
#include "sdk_config.h"

#define TINYFS_NODE_MAX ((SDK_BLE_STORAGE_PEER_MAX)*3 + 4 + SDK_USER_TINYFS_NODE_MAX)
// 4 : ecc_priv_key + static_rand_addr + local_irk + folder
#define TINYFS_SECTION_NUM 3

#if (TINYFS_SECTION_NUM<3)
#error TINYFS_SECTION_NUM must be greater than or equal to 3
#endif

#define TINYFS_SECTION_SIZE (0x1000)
#if (TINYFS_SECTION_SIZE % (0x1000))
#error TINYFS_SECTION_SIZE must be multiple of 4KBytes
#endif

#endif
