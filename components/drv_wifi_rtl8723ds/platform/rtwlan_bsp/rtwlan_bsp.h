/******************************************************************************
 * Copyright (c) 2013-2016 Realtek Semiconductor Corp.
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
 *
 ******************************************************************************/

/**
  ******************************************************************************
  * @file    rtwlan_bsp.h
  * @author  Realtek software team
  * @version V0.1
  * @date    05-March-2013
  * @brief   Realtek WLAN hardware configuration.
  ******************************************************************************
  */

#ifndef __REALTEK_WLAN_BSP_H__
#define __REALTEK_WLAN_BSP_H__
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void Set_WLAN_Power_On(void);
void Set_WLAN_Power_Off(void);
int WLAN_BSP_Config(void);
void WLAN_BSP_UsLoop(int us);
unsigned long WLAN_BSP_Transfer(unsigned char* buf, unsigned int buf_len);

#ifdef __cplusplus
}
#endif

#endif// __REALTEK_WLAN_BSP_H__
