/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JQUICK_JENKINSHASH_H_
#define JQUICK_JENKINSHASH_H_

#include <stdint.h>

namespace JQuick {

uint32_t JenkinsHashMix(uint32_t hash, uint32_t data);

uint32_t JenkinsHashWhiten(uint32_t hash);

uint32_t JenkinsHashMixBytes(uint32_t hash, const uint8_t* bytes, uint32_t size);

uint32_t JenkinsHashMixShorts(uint32_t hash, const uint16_t* shorts, uint32_t size);

}

#endif // JQUICK_JENKINSHASH_H_
