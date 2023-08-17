/**************************************************************************
 * Copyright (C) 2017-2017  Unisound
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **************************************************************************
 *
 * Description : uni_hash.h
 * Author      : chenxiaosong.unisound.com
 * Date        : 2018.06.19
 *
 **************************************************************************/
#ifndef UTILS_HASH_INC_UNI_HASH_H_
#define UTILS_HASH_INC_UNI_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif


unsigned int bkdr_hash(const char* key);

#define uni_hash(key) bkdr_hash(key)

#ifdef __cplusplus
}
#endif
#endif  //  UTILS_HASH_INC_UNI_HASH_H_
