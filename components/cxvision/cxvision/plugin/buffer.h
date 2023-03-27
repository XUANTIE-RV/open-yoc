/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef CXVISION_PLUGIN_BUFFER_H_
#define CXVISION_PLUGIN_BUFFER_H_

#include <map>
#include <memory>
#include <string>

#include <posto/posto.h>

#include "cxvision/base/memory.h"

namespace cx {

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;

class Buffer final : public posto::MessageBase {
public:
  bool AddMemory(const cx::MemoryPtr& memory) {
    return AppendBlock(memory);
  }

  cx::MemoryPtr GetMemory(unsigned int index) {
    if (index < io_blocks().size()) {
      return io_blocks().at(index);
    }
    return nullptr;
  }

  bool SetMetadata(const std::string& key, std::shared_ptr<void> meta) {
    metas_[key].object = meta;
    return true;
  }

  template <typename T>
  std::shared_ptr<T> GetMetadata(const std::string& key) {
    const auto& it = metas_.find(key);
    if (it != metas_.end()) {
      auto& meta = it->second;
      if (meta.object) {
        return std::static_pointer_cast<T>(meta.object);
      } else if (!meta.view.empty()) {
        auto obj = std::make_shared<T>();
        obj->ParseFromArray(meta.view.data(), meta.view.size());
        meta.object = obj;
        return obj;
      }
    }
    return nullptr;
  }

  size_t _ByteSizeLong() const override;
  bool _SerializeToArray(void* data, size_t size) const override;
  bool _ParseFromArray(const void* data, size_t size) override;

private:
  struct MetaValue {
    std::shared_ptr<void> object;
    std::string view;
  };

  std::map<std::string, MetaValue> metas_;
};

}  // namespace cx

#endif  // CXVISION_PLUGIN_BUFFER_H_
