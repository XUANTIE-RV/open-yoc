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
#include <errno.h>

#include "btc/btc_task.h"
#include "btc/btc_manage.h"

#include "bt_err.h"
#include "yoc_bt_defs.h"
#include "yoc_bt_main.h"

#include "btc_ble_mesh_prov.h"
#include "yoc_ble_mesh_defs.h"

int32_t ble_mesh_get_model_publish_period(ble_mesh_model_t *model)
{
    if (model == NULL) {
        return 0;
    }
    return btc_ble_mesh_model_pub_period_get(model);
}

uint16_t ble_mesh_get_primary_element_address(void)
{
    return btc_ble_mesh_get_primary_addr();
}

uint16_t *ble_mesh_is_model_subscribed_to_group(ble_mesh_model_t *model, uint16_t group_addr)
{
    if (model == NULL) {
        return NULL;
    }
    return btc_ble_mesh_model_find_group(model, group_addr);
}

ble_mesh_elem_t *ble_mesh_find_element(uint16_t element_addr)
{
    return btc_ble_mesh_elem_find(element_addr);
}

uint8_t ble_mesh_get_element_count(void)
{
    return btc_ble_mesh_elem_count();
}

ble_mesh_model_t *ble_mesh_find_vendor_model(const ble_mesh_elem_t *element,
        uint16_t company_id, uint16_t model_id)
{
    if (element == NULL) {
        return NULL;
    }
    return btc_ble_mesh_model_find_vnd(element, company_id, model_id);
}

ble_mesh_model_t *ble_mesh_find_sig_model(const ble_mesh_elem_t *element, uint16_t model_id)
{
    if (element == NULL) {
        return NULL;
    }
    return btc_ble_mesh_model_find(element, model_id);
}

const ble_mesh_comp_t *ble_mesh_get_composition_data(void)
{
    return btc_ble_mesh_comp_get();
}

