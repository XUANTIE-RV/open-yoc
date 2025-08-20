 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AOS_VERSION_H
#define AOS_VERSION_H
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Get aos product model.
 *
 * @return  model success, 0 failure.
 */
const char *aos_get_product_model(void);

/**
 * Get aos os version.
 *
 * @return  os version success, 0 failure.
 */
char *aos_get_os_version(void);

/**
 * Get aos app version.
 *
 * @return  app version success, 0 failure.
 */
char *aos_get_app_version(void);

/**
 * Get aos kernel version.
 *
 * @return  kernel version success, 0 failure.
 */
const char *aos_get_kernel_version(void);

/**
 * Get aos device name.
 *
 * @return  device name success, 0 failure.
 */
const char *aos_get_device_name(void);


#ifdef __cplusplus
}
#endif

#endif /* AOS_VERSION_H */

