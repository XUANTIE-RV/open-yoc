/*
 * Copyright (C) 2018 C-SKY Microsystems Co., Ltd. All rights reserved.
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
 /******************************************************************************
 * @file     vector.c
 * @brief    vector table for bootloader
 * @version  V1.0
 * @date     23. may 2018
 ******************************************************************************/


extern void Reset_Handler(void);
extern void drv_reboot(void);
#define reboot_system drv_reboot


__attribute__((aligned(1024))) void (*BootVectors[])(void) = {
    Reset_Handler,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
    reboot_system,
};

