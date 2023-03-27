/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef CXVISION_PROTO_VOICE_H_
#define CXVISION_PROTO_VOICE_H_

#include <cstdint>
#include <string>

#include <posto/posto.h>
#include "cxvision/byteorder.h"

namespace thead {
namespace voice {
namespace proto {

/*************************************
 * 消息:算法发送事件到MIC模块
 *************************************/
enum SessionCmd : int32_t {
  BEGIN = 0,
  END = 1,
  TIMEOUT = 2,
  WWV = 3,
  DOA = 4,
  ASR = 5,
  VAD_BEGIN = 6,
  VAD_END = 7,
};

class SessionMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (7 * sizeof(int32_t) + kws_word_.size());
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr = CX_HTOLE32(cmd_id_);
    *(ptr + 1) = CX_HTOLE32(wakeup_type_);
    *(ptr + 2) = CX_HTOLE32(kws_id_);
    *(ptr + 3) = CX_HTOLE32(kws_score_);
    *(ptr + 4) = CX_HTOLE32(kws_doa_);
    *(ptr + 5) = CX_HTOLE32(vad_send_count_);
    *(ptr + 6) = CX_HTOLE32(kws_word_.size());
    std::memcpy(ptr + 7, kws_word_.data(), kws_word_.size());
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr    = (uint32_t*)data;
    cmd_id_          =      (::thead::voice::proto::SessionCmd)CX_LE32TOH(*ptr);
    wakeup_type_     = (int32_t)CX_LE32TOH(*(ptr + 1));
    kws_id_          = (int32_t)CX_LE32TOH(*(ptr + 2));
    kws_score_       = (int32_t)CX_LE32TOH(*(ptr + 3));
    kws_doa_         = (int32_t)CX_LE32TOH(*(ptr + 4));
    vad_send_count_         = (int32_t)CX_LE32TOH(*(ptr + 5));
    size_t word_size = CX_LE32TOH(*(ptr + 6));
    kws_word_.resize(word_size);
    std::memcpy(&kws_word_[0], ptr + 7, word_size);
    return true;
  }

  ::thead::voice::proto::SessionCmd cmd_id() const {
    return cmd_id_;
  }

  int32_t wakeup_type() const {
    return wakeup_type_;
  }

  int32_t kws_id() const {
    return kws_id_;
  }

  int32_t kws_score() const {
    return kws_score_;
  }

  int32_t kws_doa() const {
    return kws_doa_;
  }

  int32_t vad_send_count() const {
    return vad_send_count_;
  }

  const std::string& kws_word() const {
    return kws_word_;
  }

  void set_wakeup_type(int32_t value) {
    wakeup_type_ = value;
  }

  void set_cmd_id(::thead::voice::proto::SessionCmd value) {
    cmd_id_ = value;
  }

  void set_kws_id(int32_t value) {
    kws_id_ = value;
  }

  void set_kws_score(int32_t value) {
    kws_score_ = value;
  }

  void set_kws_doa(int32_t value) {
    kws_doa_ = value;
  }

  void set_vad_send_count(int32_t value) {
    vad_send_count_ = value;
  }

  void set_kws_word(const std::string& value) {
    kws_word_ = value;
  }
private:
  ::thead::voice::proto::SessionCmd cmd_id_;
  int32_t wakeup_type_;
  int32_t kws_id_;
  int32_t kws_score_;
  int32_t kws_doa_;
  int32_t vad_send_count_;
  std::string kws_word_;
};

/* record command message */
enum RecordCmd : int32_t {
  START = 0,
  STOP = 1,
};

class RecordMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return 2 * sizeof(int32_t);
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr          = CX_HTOLE32(cmd_);
    *(ptr + 1)    = CX_HTOLE32(record_chn_count_);
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr     = (uint32_t*)data;
    cmd_              = (::thead::voice::proto::RecordCmd)CX_LE32TOH(*ptr);
    record_chn_count_ = (int32_t)CX_LE32TOH(*(ptr + 1));
    return true;
  }

  void set_cmd(::thead::voice::proto::RecordCmd value) {
    cmd_ = value;
  }

  ::thead::voice::proto::RecordCmd cmd() const {
    return cmd_;
  }

  void set_record_chn_count(int32_t value) {
    record_chn_count_ = value;
  }

  int32_t record_chn_count() const {
    return record_chn_count_;
  }

private:
  ::thead::voice::proto::RecordCmd cmd_;
  int32_t record_chn_count_;
};

/* Vad output message */
class VadOutMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return sizeof(int32_t);
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr = CX_HTOLE32(vad_status_);
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    vad_status_ = (int32_t)CX_LE32TOH(*ptr);
    return true;
  }

  void set_vad_status(int32_t value) {
    vad_status_ = value;
  }

  int vad_status() const {
    return vad_status_;
  }

private:
  int32_t vad_status_;
};

/* inference output message */
class InferOutMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (4 * sizeof(int32_t));
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr       = CX_HTOLE32(kws_chn_);
    *(ptr + 1) = CX_HTOLE32(kws_id_);
    *(ptr + 2) = CX_HTOLE32(kws_score_);
    *(ptr + 3) = CX_HTOLE32(first_wakeup_);
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    kws_chn_      = (int32_t)CX_LE32TOH(*ptr);
    kws_id_       = (int32_t)CX_LE32TOH(*(ptr + 1));
    kws_score_    = (int32_t)CX_LE32TOH(*(ptr + 2));
    first_wakeup_ = (int32_t)CX_LE32TOH(*(ptr + 3));
    return true;
  }

  int32_t first_wakeup() const {
    return first_wakeup_;
  }

  int32_t kws_score() const {
    return kws_score_;
  }

  int32_t kws_id() const {
    return kws_id_;
  }

  int32_t kws_chn() const {
    return kws_chn_;
  }

  void set_first_wakeup(int32_t value) {
    first_wakeup_ = value;
  }

  void set_kws_score(int32_t value) {
    kws_score_ = value;
  }

  void set_kws_id(int32_t value) {
    kws_id_ = value;
  }

  void set_kws_chn(int32_t value) {
    kws_chn_ = value;
  }

private:
    int32_t kws_chn_;
    int32_t kws_id_;
    int32_t kws_score_;
    int32_t first_wakeup_;
};


/* SSP output message */
class SspOutMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (4 * sizeof(int32_t));
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr       = CX_HTOLE32(vad_res_);
    *(ptr + 1) = CX_HTOLE32(chn_num_);
    *(ptr + 2) = CX_HTOLE32(sample_rate_);
    *(ptr + 3) = CX_HTOLE32(frame_);
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    vad_res_      = (int32_t)CX_LE32TOH(*ptr);
    chn_num_      = (int32_t)CX_LE32TOH(*(ptr + 1));
    sample_rate_  = (int32_t)CX_LE32TOH(*(ptr + 2));
    frame_        = (int32_t)CX_LE32TOH(*(ptr + 3));
    return true;
  }

  int32_t vad_res() const {
    return vad_res_;
  }

  int32_t chn_num() const {
    return chn_num_;
  }

  int32_t sample_rate() const {
    return sample_rate_;
  }

  int32_t frame() const {
    return frame_;
  }

  void set_vad_res(int32_t value) {
    vad_res_ = value;
  }

  void set_chn_num(int32_t value) {
    chn_num_ = value;
  }

  void set_sample_rate(int32_t value) {
    sample_rate_ = value;
  }

  void set_frame(int32_t value) {
    frame_ = value;
  }

private:
    int32_t vad_res_;
    int32_t chn_num_;
    int32_t sample_rate_;
    int32_t frame_;
};


/* data input message */
class DataInputMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (4 * sizeof(int32_t));
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr       = CX_HTOLE32(chn_num_);
    *(ptr + 1) = CX_HTOLE32(format_);
    *(ptr + 2) = CX_HTOLE32(sample_rate_);
    *(ptr + 3) = CX_HTOLE32(frame_);
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    chn_num_      = (int32_t)CX_LE32TOH(*ptr);
    format_       = (int32_t)CX_LE32TOH(*(ptr + 1));
    sample_rate_  = (int32_t)CX_LE32TOH(*(ptr + 2));
    frame_        = (int32_t)CX_LE32TOH(*(ptr + 3));
    return true;
  }

  int32_t format() const {
    return format_;
  }

  int32_t chn_num() const {
    return chn_num_;
  }

  int32_t sample_rate() const {
    return sample_rate_;
  }

  int32_t frame() const {
    return frame_;
  }

  void set_format(int32_t value) {
    format_ = value;
  }

  void set_chn_num(int32_t value) {
    chn_num_ = value;
  }

  void set_sample_rate(int32_t value) {
    sample_rate_ = value;
  }

  void set_frame(int32_t value) {
    frame_ = value;
  }

private:
  int32_t chn_num_;
  int32_t format_;
  int32_t sample_rate_;
  int32_t frame_;
};

/*************************************
 * 消息:算法到Dispatch节点
 *************************************/
enum DataType : int32_t {
  TYPE_PCM = 0,   /* 上云数据 */
  TYPE_KWS = 1,   /* 关键词数据 */
  TYPE_FEAEC = 2, /* 线性AEC后AGC前数据, 分布式唤醒使用 */
};
/* data input message */
class KwsOutMsg final : public posto::Serializable {
public:
  size_t ByteSizeLong() const override {
    return (2 * sizeof(int32_t));
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr       = CX_HTOLE32(type_);
    *(ptr + 1) = CX_HTOLE32(buf_len_);

    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    type_         = (::thead::voice::proto::DataType)CX_LE32TOH(*ptr);
    buf_len_      = (int32_t)CX_LE32TOH(*(ptr + 1));

    return true;
  }

  ::thead::voice::proto::DataType type() const {
    return type_;
  }

  int32_t buf_len() const {
    return buf_len_;
  }

  void set_type(::thead::voice::proto::DataType value) {
    type_ = value;
  }

  void set_buf_len(int32_t value) {
    buf_len_ = value;
  }

private:
  ::thead::voice::proto::DataType type_;
  int32_t buf_len_;
};

/*************************************
 * 消息:MIC应用发送到算法的消息
 *************************************/
enum AlgCmd : int32_t {
  PUSH2TALK_CMD          = 0,
  PLAYSTATE_CMD          = 1,
  WAKEUP_LEVEL_CMD       = 2,
  START_DOA_CMD          = 3,
  ENABLE_LINEAR_AEC_DATA_CMD = 4,
  ENABLE_ASR             = 5,
};
/* alg cmd message */
class AlgCmdMsg final : public posto::Serializable {
public:
  AlgCmdMsg() {
    ivalue_ = 0;
    ivalue2_ = 0;
    strvalue_.clear();
  }

  size_t ByteSizeLong() const override {
    return (4 * sizeof(int32_t) + strvalue_.size());
  }

  bool SerializeToArray(void* data, size_t size) const override {
    uint32_t* ptr = (uint32_t*)data;
    *ptr          = CX_HTOLE32(cmd_);
    *(ptr + 1)    = CX_HTOLE32(ivalue_);
    *(ptr + 2)    = CX_HTOLE32(ivalue2_);
    *(ptr + 3)    = CX_HTOLE32(strvalue_.size());
    std::memcpy(ptr + 4, strvalue_.data(), strvalue_.size());
    return true;
  }

  bool ParseFromArray(const void* data, size_t size) {
    uint32_t* ptr            = (uint32_t*)data;
    cmd_                     = (::thead::voice::proto::AlgCmd)CX_LE32TOH(*ptr);
    ivalue_                  = (int32_t)CX_LE32TOH(*(ptr + 1));
    ivalue2_                 = (int32_t)CX_LE32TOH(*(ptr + 2));
    size_t word_size         = CX_LE32TOH(*(ptr + 3));
    strvalue_.resize(word_size);
    std::memcpy(&strvalue_[0], ptr + 4, word_size);
  
    return true;
  }

  ::thead::voice::proto::AlgCmd cmd() const {
    return cmd_;
  }

  int32_t ivalue() const {
    return ivalue_;
  }

  int32_t ivalue2() const {
    return ivalue2_;
  }

  const std::string& strvalue() const {
    return strvalue_;
  }

  void set_cmd(::thead::voice::proto::AlgCmd value) {
    cmd_ = value;
  }

  void set_ivalue(int32_t value) {
    ivalue_ = value;
  }

  void set_ivalue2(int32_t value) {
    ivalue2_ = value;
  }

  void set_strvalue(const std::string& value) {
    strvalue_ = value;
  }

private:
  ::thead::voice::proto::AlgCmd cmd_;

  int32_t ivalue_;   /* 通用int32 Value， 简单的通讯配置统一使用该参数 */
  int32_t ivalue2_;  /* 通用int32 Value， 简单的通讯配置统一使用该参数 */
  std::string strvalue_;/* 通用字符串 Value */
};

}  // namespace proto
}  // namespace voice
}  // namespace thead

#endif  // CXVISION_PROTO_VOICE_H_
