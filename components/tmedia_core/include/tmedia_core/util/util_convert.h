/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_CONVERT_H
#define TM_UTIL_CONVERT_H

using namespace std;

template<typename E>
inline constexpr auto SafeEnumVal(E e) -> typename std::underlying_type<E>::type
{
   return static_cast<typename std::underlying_type<E>::type>(e);
}

#endif  /* TM_UTIL_CONVERT_H */
