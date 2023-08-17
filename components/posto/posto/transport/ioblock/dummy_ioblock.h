/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef POSTO_TRANSPORT_IOBLOCK_DUMMYIOBLOCK_H_
#define POSTO_TRANSPORT_IOBLOCK_DUMMYIOBLOCK_H_

#include <stdlib.h>
#include <string.h>

#include "posto/transport/ioblock/ioblock.h"

namespace posto {
namespace transport {

class DummyIoBlock : public IoBlock {
 public:
  DummyIoBlock() : size_(0), data_(nullptr) {}

  explicit DummyIoBlock(size_t size) : size_(size) {
    data_ = (char*)malloc(size);
  }

  DummyIoBlock(const void* data, size_t size) : DummyIoBlock(size) {
    memcpy(data_, data, size);
  }

  ~DummyIoBlock() {
    free(data_);
  }

  bool SerializeTo(IoBlockMeta& meta) {
    meta.data_size = size_;
    return true;
  }

  bool DeserializeFrom(const IoBlockMeta& meta) {
    size_ = meta.data_size;
    data_ = (char*)malloc(size_);
    return true;
  }

  void* data() override { return data_; }
  size_t size() const override { return size_; }
  size_t block_size() const override { return size_; }
  uint64_t id() const override { return (uint64_t)(uintptr_t)data_; }

 private:
  size_t size_;
  char* data_;
};

}  // namespace transport
}  // namespace posto

#endif  // POSTO_TRANSPORT_IOBLOCK_DUMMYIOBLOCK_H_
