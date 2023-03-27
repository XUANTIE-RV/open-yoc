/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef POSTO_TRANSPORT_WRITER_WRITER_H_
#define POSTO_TRANSPORT_WRITER_WRITER_H_

#include <memory>

#include "posto/discovery/endpoint_manager.h"
#include "posto/profile/statistics.h"
#include "posto/transport/reader/intra_dispatcher.h"
#include "posto/transport/writer/ipc_writer.h"

namespace posto {
namespace transport {

struct WriterStatistics : public profile::Statistics {
  unsigned sent = 0;

  void Print(int level) override {
    TAB_PRINT(level, "sent: %u\r\n", sent);
  };
};

template <typename MessageT>
class Writer {
 public:
  using MessagePtr = std::shared_ptr<MessageT>;
  using IpcWriterPtr = std::shared_ptr<IpcWriter<MessageT>>;

  Writer(const entity::Attributes& attr);
  ~Writer();

  bool Write(const MessagePtr& msg);

  profile::Statistics* GetStatistics();

 private:
  void Init();

  entity::Attributes attr_;
  IntraDispatcherPtr intra_dispatcher_;
  IpcWriterPtr ipc_writer_;
  WriterStatistics statistics_;
};

template <typename MessageT>
Writer<MessageT>::Writer(const entity::Attributes& attr)
    : attr_(attr), intra_dispatcher_(IntraDispatcher::Instance()) {
  Init();
}

template <typename MessageT>
Writer<MessageT>::~Writer() {}

template <typename MessageT>
bool Writer<MessageT>::Write(const MessagePtr& msg) {
  auto endpoint_manager = discovery::EndpointManager::Instance();
  discovery::EndpointManager::EntityAttrVec readers;
  endpoint_manager->GetReadersByTopic(attr_.topic, readers);
  if (!readers.size()) {
    return false;
  }
  for (const auto& reader : readers) {
    if (reader->host_id != attr_.host_id) {
      ipc_writer_->Write(msg, *reader);
    } else {
      intra_dispatcher_->OnMessage(reader->guid, msg);
    }
  }

  ++statistics_.sent;
  return true;
}

template <typename MessageT>
void Writer<MessageT>::Init() {
  ipc_writer_ = std::make_shared<IpcWriter<MessageT>>(attr_);
}

template <typename MessageT>
profile::Statistics* Writer<MessageT>::GetStatistics() {
  return &statistics_;
}

}  // namespace transport
}  // namespace posto

#endif  // POSTO_TRANSPORT_WRITER_WRITER_H_
