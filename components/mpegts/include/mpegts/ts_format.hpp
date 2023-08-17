/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_FORMAT_HPP__
#define __TS_FORMAT_HPP__

#include <mpegts/ts_typedef.h>

class TsHeader final
{
public:
    TsHeader();
    void     pack(uint8_t buf[4]);

public:
    uint8_t  sync_byte: 8;                   // 8
    uint8_t  transport_error_indicator: 1;     // 1
    uint8_t  payload_unit_start_indicator: 1;  // 1
    uint8_t  transport_priority: 1;            // 1
    uint16_t pid: 13;                           // 13
    uint8_t  transport_scrambling_control: 2;  // 2
    uint8_t  adaptation_field_control: 2;      // 2
    uint8_t  continuity_counter: 4;            // 4
};

class PatHeader final
{
public:
    PatHeader();
    void pack(uint8_t buf[8]);

public:
    uint8_t  table_id: 8;                      // 8
    uint8_t  section_syntax_indicator: 1;      // 1
    uint8_t  const0: 1;                        // 1
    uint8_t  reserved0: 2;                     // 2
    uint16_t section_length: 12;                // 12
    uint16_t transport_stream_id: 16;           // 16
    uint8_t  reserved1: 2;                     // 2
    uint8_t  version_number: 5;                // 5
    uint8_t  current_next_indicator: 1;        // 1
    uint8_t  section_number: 8;                // 8
    uint8_t  last_section_number: 8;           // 8
};

class PmtElement final
{
public:
    PmtElement();
    void pack(uint8_t buf[5]);

public:
    uint8_t  stream_type: 8;                   // 8
    uint8_t  reserved0: 3;                     // 3
    uint16_t elementary_PID: 13;                // 13
    uint8_t  reserved1: 4;                     // 4
    uint16_t ES_info_length: 12;                // 12
};

class PmtHeader final
{
public:
    PmtHeader();
    void pack(uint8_t buf[12]);

public:
    uint8_t  table_id: 8;                      // 8
    uint8_t  section_syntax_indicator: 1;      // 1
    uint8_t  const0: 1;                        // 1
    uint8_t  reserved0: 2;                     // 2
    uint16_t section_length: 12;                // 12
    uint16_t program_number: 16;                // 16
    uint8_t  reserved1: 2;                     // 2
    uint8_t  version_number: 5;                // 5
    uint8_t  current_next_indicator: 1;        // 1
    uint8_t  section_number: 8;                // 8
    uint8_t  last_section_number: 8;           // 8
    uint8_t  reserved2: 3;                     // 3
    uint16_t PCR_PID: 13;                       // 13
    uint8_t  reserved3: 4;                     // 4
    uint16_t program_info_length: 12;           // 12
};

class PesHeader final
{
public:
    PesHeader();
    void pack(uint8_t buf[9]);

public:
    uint32_t packet_start_code: 24;             // 24
    uint8_t  stream_id: 8;                     // 8
    uint16_t pes_packet_length: 16;             // 16
    uint8_t  const0: 2;                        // 2
    uint8_t  pes_scrambling_control: 2;        // 2
    uint8_t  pes_priority: 1;                  // 1
    uint8_t  data_alignment_indicator: 1;      // 1
    uint8_t  copyright: 1;                     // 1
    uint8_t  original_or_copy: 1;              // 1
    uint8_t  pts_dts_flags: 2;                 // 2
    uint8_t  escr_flag: 1;                     // 1
    uint8_t  es_rate_flag: 1;                  // 1
    uint8_t  dsm_trick_mode_flag: 1;           // 1
    uint8_t  additional_copy_info_flag: 1;     // 1
    uint8_t  pes_crc_flag: 1;                  // 1
    uint8_t  pes_extention_flag: 1;            // 1
    uint8_t  header_data_length: 8;            // 8
};

class AdaptationHeader final
{
public:
    AdaptationHeader();
    void pack(uint8_t buf[2]);

public:
    uint8_t adaptation_field_length: 8;               // 8 bits
    uint8_t discontinuity_indicator: 1;               // 1 bit
    uint8_t random_access_indicator: 1;               // 1 bit
    uint8_t elementary_stream_priority_indicator: 1;  // 1 bit
    uint8_t pcr_flag: 1;                              // 1 bit
    uint8_t opcr_flag: 1;                             // 1 bit
    uint8_t splicing_point_flag: 1;                   // 1 bit
    uint8_t transport_private_data_flag: 1;           // 1 bit
    uint8_t adaptation_field_extension_flag: 1;       // 1 bit
};

#endif

