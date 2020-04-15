/*
 * Copyright (C) 2018 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MDM_AT_H__
#define __MDM_AT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mdm_at_send_cb)(char *at_cmd);

int mdm_send_from_user(const char *atcmd, size_t size);

int mdm_send_from_api(const char *atcmd, size_t size, mdm_at_send_cb cb);

#ifdef __cplusplus
}
#endif

#endif /* __MDM_AT_H__ */
