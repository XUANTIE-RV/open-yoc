/**
 * @file mutex.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_MUTEX_H
#define CX_COMMON_MUTEX_H


namespace cx {

template <typename MutexT>
class lock_guard {
public:
  explicit lock_guard(MutexT& m) : m_(m) {
    m_.lock();
  }

  ~lock_guard() {
    m_.unlock();
  }

private:
  lock_guard(const lock_guard&) = delete;

  MutexT& m_;
};

}


#endif /* CX_COMMON_MUTEX_H */

