#ifndef _HAL_BT_H_
#define _HAL_BT_H_

#include <stdint.h>
#include "tg_bt_mesh.h"
#include "bluetooth.h"

#define HAL_BT_GATT_INVALID_IF 0
#define HAL_BT_ADDR_LEN 6

typedef enum
{
    HAL_BT_ERR_SUCCESS = 0,
    HAL_BT_ERR_INVALID_PARAM,
    HAL_BT_ERR_FAIL,
    HAL_BT_ERR_NO_CONN_FOUND,
    HAL_BT_ERR_GATT_IF_FULL,
    HAL_BT_ERR_GATT_IF_INVALID,
    HAL_BT_ERR_GATT_DB_FULL,
    HAL_BT_ERR_GATT_SVC_HANDLE_INVALID,
    HAL_BT_ERR_GATTC_BUSY,
    HAL_BT_ERR_NO_ENOUGH_MEM,
    /* Add more HAL BT error code hereafter */
} hal_bt_status_t;

typedef enum
{
    HAL_BT_GATTC_IDLE = 0,
    HAL_BT_GATTC_DISC
} hal_bt_gattc_state_t;
    /**
 * @brief Attribute permissions
 */
#define  HAL_YOC_GATT_PERM_READ                  (1 << 0)   /* bit 0 -  0x0001 */    /* relate to BTA_GATT_PERM_READ in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_READ_ENCRYPTED        (1 << 1)   /* bit 1 -  0x0002 */    /* relate to BTA_GATT_PERM_READ_ENCRYPTED in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_READ_ENC_MITM         (1 << 2)   /* bit 2 -  0x0004 */    /* relate to BTA_GATT_PERM_READ_ENC_MITM in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_WRITE                 (1 << 4)   /* bit 4 -  0x0010 */    /* relate to BTA_GATT_PERM_WRITE in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_WRITE_ENCRYPTED       (1 << 5)   /* bit 5 -  0x0020 */    /* relate to BTA_GATT_PERM_WRITE_ENCRYPTED in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_WRITE_ENC_MITM        (1 << 6)   /* bit 6 -  0x0040 */    /* relate to BTA_GATT_PERM_WRITE_ENC_MITM in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_WRITE_SIGNED          (1 << 7)   /* bit 7 -  0x0080 */    /* relate to BTA_GATT_PERM_WRITE_SIGNED in bta/bta_gatt_api.h */
#define  HAL_YOC_GATT_PERM_WRITE_SIGNED_MITM     (1 << 8)   /* bit 8 -  0x0100 */    /* relate to BTA_GATT_PERM_WRITE_SIGNED_MITM in bta/bta_gatt_api.h */
    
int tg_bt_scan_start_internal(struct bt_le_scan_param *param, tg_bt_mesh_le_scan_cb_t cb, bool from_mesh);
int tg_bt_scan_stop_internal(bool from_mesh);
#endif
