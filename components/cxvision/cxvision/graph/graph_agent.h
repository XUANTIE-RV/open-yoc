/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef CXVISION_GRAPH_AGENT_H_
#define CXVISION_GRAPH_AGENT_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#ifdef __YOC__
#include <aos/kernel.h>
#elif defined(__linux__)
#include <semaphore.h>
#endif

#include <posto/base/mutex.h>
#include <posto/common/macros.h>
#include <posto/posto.h>

#include "cxvision/byteorder.h"
#include "cxvision/plugin/plugin.h"

namespace cx {
namespace internal {

enum AgentProc : uint32_t {
  AGENT_PROC_START = 1,
  AGENT_PROC_STOP,
  AGENT_PROC_GET_STATISTICS,
};

class AgentRequest final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (4 * sizeof(uint32_t) + manager_id_.size() + config_.size());
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr = CX_HTOLE32(ctx_id_);
    *(ptr + 1) = CX_HTOLE32(proc_id_);
    *(ptr + 2) = CX_HTOLE32(manager_id_.size());
    *(ptr + 3) = CX_HTOLE32(config_.size());
    char* pc = (char*)(ptr + 4);
    std::memcpy(pc, manager_id_.data(), manager_id_.size());
    std::memcpy(pc + manager_id_.size(), config_.data(), config_.size());
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    ctx_id_ = CX_LE32TOH(*ptr);
    proc_id_ = (AgentProc)CX_LE32TOH(*(ptr + 1));
    size_t manager_id_size = CX_LE32TOH(*(ptr + 2));
    size_t config_size = CX_LE32TOH(*(ptr + 3));
    char* pc = (char*)(ptr + 4);
    manager_id_.resize(manager_id_size);
    config_.resize(config_size);
    std::memcpy(&manager_id_[0], pc, manager_id_size);
    std::memcpy(&config_[0], pc + manager_id_size, config_size);
    return true;
  }

  uint32_t ctx_id() const {
    return ctx_id_;
  }

  AgentProc proc_id() const {
    return proc_id_;
  }

  const std::string& manager_id() const {
    return manager_id_;
  }

  const std::string& config() const {
    return config_;
  }

  void set_ctx_id(uint32_t value) {
    ctx_id_ = value;
  }

  void set_proc_id(AgentProc value) {
    proc_id_ = value;
  }

  void set_manager_id(const std::string& value) {
    manager_id_ = value;
  }

  void set_config(const std::string& value) {
    config_ = value;
  }

private:
  uint32_t ctx_id_;
  AgentProc proc_id_;
  std::string manager_id_;
  std::string config_;
};

class AgentResponse final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (3 * sizeof(uint32_t) + statistics_.size());
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr = CX_HTOLE32(ctx_id_);
    *(ptr + 1) = CX_HTOLE32(error_code_);
    *(ptr + 2) = CX_HTOLE32(statistics_.size());
    std::memcpy(ptr + 3, statistics_.data(), statistics_.size());
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    ctx_id_ = CX_LE32TOH(*ptr);
    error_code_ = (int32_t)CX_LE32TOH(*(ptr + 1));
    size_t str_size = CX_LE32TOH(*(ptr + 2));
    statistics_.resize(str_size);
    std::memcpy(&statistics_[0], ptr + 3, str_size);
    return true;
  }

  uint32_t ctx_id() const {
    return ctx_id_;
  }

  int32_t error_code() const {
    return error_code_;
  }

  const std::string& statistics() const {
    return statistics_;
  }

  void set_ctx_id(uint32_t value) {
    ctx_id_ = value;
  }

  void set_error_code(int32_t value) {
    error_code_ = value;
  }

  void set_statistics(const std::string& value) {
    statistics_ = value;
  }

private:
  uint32_t ctx_id_;
  int32_t error_code_;
  std::string statistics_;
};

using RequestT = posto::Message<AgentRequest>;
using RequestTPtr = std::shared_ptr<RequestT>;
using ResponseT = posto::Message<AgentResponse>;
using ResponseTPtr = std::shared_ptr<ResponseT>;

// Must run in a device wide daemon
class GraphAgent final {
 public:
  bool Init();

 private:
  int Start(const std::string& manager_id, const std::string& conf);
  int Stop(const std::string& manager_id);
  int GetStatistics(const std::string& manager_id, std::string& out_str);

 private:
  struct VertexInfo {
    std::string name;
    PluginBase* plugins;
  };

  posto::base::mutex mutex_;
  std::shared_ptr<posto::Participant> participant_;
  std::shared_ptr<posto::Reader<RequestT>> req_channel_;
  // <response topic, writers> pair
  std::map<std::string, std::shared_ptr<posto::Writer<ResponseT>>> res_channels_;
  // <manager_id, <graph name, plugins>> pair
  std::map<std::string, std::map<std::string, std::vector<VertexInfo>>> managers_;

  POSTO_DECLARE_SINGLETON(GraphAgent)
};

class GraphAgentClient final {
 public:
  GraphAgentClient(const std::string& manager_id,
                   const std::string& dest_device_id);

  bool Init();
  bool DeInit();

  int Start(const std::string& conf);
  int Stop();
  int GetStatistics(std::string& out_str);

 private:
  struct PendingRequest {
#ifdef __YOC__
    aos_sem_t notifier;
#elif defined(__linux__)
    sem_t notifier;
#endif
    ResponseTPtr response;
  };

  posto::base::mutex mutex_;
  uint32_t next_ctx_id_;
  // <context id, request> pair
  std::map<uint32_t, PendingRequest*> pending_requests_;
  std::string manager_id_;
  std::string dest_device_id_;
  std::shared_ptr<posto::Participant> participant_;
  std::shared_ptr<posto::Writer<RequestT>> req_channel_;
  std::shared_ptr<posto::Reader<ResponseT>> res_channel_;
};

}  // namespace internal
}  // namespace cx

#endif  // CXVISION_GRAPH_AGENT_H_
