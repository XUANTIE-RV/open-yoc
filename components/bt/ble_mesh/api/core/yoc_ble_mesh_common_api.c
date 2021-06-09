// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>

#include <aos/kernel.h>

#include "btc/btc_task.h"
#include "btc/btc_manage.h"

#include "bt_err.h"
#include "yoc_bt_defs.h"
#include "yoc_bt_main.h"

#include "btc_ble_mesh_prov.h"
#include "yoc_ble_mesh_defs.h"

bt_err_t ble_mesh_init(ble_mesh_prov_t *prov, ble_mesh_comp_t *comp)
{
    btc_ble_mesh_prov_args_t arg = {0};
    aos_sem_t semaphore;
    btc_msg_t msg = {0};

    if (prov == NULL || comp == NULL) {
        return BT_ERR_INVALID_ARG;
    }

    BT_BLUEDROID_STATUS_CHECK(BT_BLUEDROID_STATUS_ENABLED);

    // Create a semaphore
    if ((aos_sem_new(&semaphore, 0)) != 0) {
        BT_LOG_ERROR("%s, Failed to allocate memory for the semaphore", __func__);
        return BT_ERR_NO_MEM;
    }

    arg.mesh_init.prov = prov;
    arg.mesh_init.comp = comp;
    /* Transport semaphore pointer to BTC layer, and will give the semaphore in the BTC task */
    arg.mesh_init.semaphore = &semaphore;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PROV;
    msg.act = BTC_BLE_MESH_ACT_MESH_INIT;

    if (btc_transfer_context(&msg, &arg, sizeof(btc_ble_mesh_prov_args_t), NULL) != BT_STATUS_SUCCESS) {
        aos_sem_free(&semaphore);
        BT_LOG_ERROR("%s, BLE Mesh initialise failed", __func__);
        return BT_FAIL;
    }

    /* Take the Semaphore, wait BLE Mesh initialization to finish. */
    aos_sem_wait(&semaphore, AOS_WAIT_FOREVER);
    /* Don't forget to delete the semaphore at the end. */
    aos_sem_free(&semaphore);

    return BT_OK;
}

