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

#ifndef _DEVICE_PWM_H_
#define _DEVICE_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <devices/device.h>

/**
 * enum rvm_hal_pwm_polarity_t - polarity of a PWM signal
 * @RVM_HAL_PWM_POLARITY_NORMAL: a high signal for the duration of the duty-
 * cycle, followed by a low signal for the remainder of the pulse
 * period
 * @RVM_HAL_PWM_POLARITY_INVERSED: a low signal for the duration of the duty-
 * cycle, followed by a high signal for the remainder of the pulse
 * period
 */
typedef enum {
	RVM_HAL_PWM_POLARITY_NORMAL = 0,
	RVM_HAL_PWM_POLARITY_INVERSED,
} rvm_hal_pwm_polarity_t;

typedef struct {
    float    duty_cycle; /**< the pwm duty_cycle */
    uint32_t freq;       /**< the pwm freq */
    rvm_hal_pwm_polarity_t polarity; /**< the pwm polarity */
} rvm_hal_pwm_config_t;

#define rvm_hal_pwm_open(name) rvm_hal_device_open(name)
#define rvm_hal_pwm_close(dev) rvm_hal_device_close(dev)

/**
  \brief       Set config for pwm device
  \param[in]   dev    Pointer to device object.
  \param[in]   config Pointer to the configuration
  \param[in]   channel The channel number
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_pwm_config(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel);

/**
  \brief       Get config for pwm device
  \param[in]   dev    Pointer to device object.
  \param[out]  config Pointer to the configuration
  \param[in]   channel The channel number
  \return      0 : on success,  otherwise is error
*/
int rvm_hal_pwm_config_get(rvm_dev_t *dev, rvm_hal_pwm_config_t *config, uint8_t channel);

/**
 \brief      Starts Pulse-Width Modulation signal output on a PWM pin
 \param[in]  dev      Pointer to device object.
 \param[in]  channel  The channel number
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_pwm_start(rvm_dev_t *dev, uint8_t channel);

/**
 \brief      Stops output on a PWM pin
 \param[in]  dev      Pointer to device object.
 \param[in]  channel  The channel number
 \return     0 : on success,  otherwise is error
 */
int rvm_hal_pwm_stop(rvm_dev_t *dev, uint8_t channel);


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <devices/vfs_pwm.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
