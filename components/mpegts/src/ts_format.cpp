/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */
#include <mpegts/ts_format.hpp>

TsHeader::TsHeader()
{
    sync_byte                    = 0x47;
    transport_error_indicator    = 0;
    payload_unit_start_indicator = 0;
    transport_priority           = 0;
    pid                          = 0;
    transport_scrambling_control = 0;
    adaptation_field_control     = 0;
    continuity_counter           = 0;
}

void TsHeader::pack(uint8_t buf[4])
{
    buf[0]  = sync_byte;
    buf[1]  = (pid >> 8) & 0x1F;
    buf[1] |= (transport_priority << 5) & 0x20;
    buf[1] |= (payload_unit_start_indicator << 6) & 0x40;
    buf[1] |= (transport_error_indicator << 7) & 0x80;
    buf[2]  = pid & 0xFF;
    buf[3]  = continuity_counter & 0x0F;
    buf[3] |= (adaptation_field_control << 4) & 0x30;
    buf[3] |= (transport_scrambling_control << 6) & 0xC0;
}

PatHeader::PatHeader()
{
    table_id                 = 0;
    section_syntax_indicator = 0;
    const0                   = 0;
    reserved0                = 0x3;
    section_length           = 0;
    transport_stream_id      = 0;
    reserved1                = 0x3;
    version_number           = 0;
    current_next_indicator   = 0;
    section_number           = 0;
    last_section_number      = 0;
}

void PatHeader::pack(uint8_t buf[8])
{
    memset(buf, 0, 8);
    buf[0]  = table_id;
    buf[1]  = (section_length >> 8) & 0x0F;
    buf[1] |= (reserved0 << 4) & 0x30;
    buf[1] |= (section_syntax_indicator << 7) & 0x80;
    buf[2]  = section_length & 0xFF;
    buf[3]  = (transport_stream_id >> 8) & 0xFF;
    buf[4]  = transport_stream_id & 0xFF;
    buf[5]  = current_next_indicator & 0x01;
    buf[5] |= (version_number << 1) & 0x3E;
    buf[5] |= (reserved1  << 6) & 0xc0;
    buf[6]  = section_number;
    buf[7]  = last_section_number;
}

PmtElement::PmtElement()
{
    stream_type    = 0;
    reserved0      = 0x7;
    elementary_PID = 0;
    reserved1      = 0xf;
    ES_info_length = 0;
}

void PmtElement::pack(uint8_t buf[5])
{
    buf[0]  = stream_type;
    buf[1]  = (elementary_PID >> 8) & 0x1F;
    buf[1] |= (reserved0 << 5) & 0xE0;
    buf[2]  = elementary_PID & 0xFF;

    buf[3]  = (ES_info_length >> 8) & 0xF;
    buf[3] |= (reserved1 << 4) & 0xF0;
    buf[4] = ES_info_length & 0xFF;
}

PmtHeader::PmtHeader()
{
    table_id                 = TABLE_ID::PMS_TABLE;
    section_syntax_indicator = 0;
    const0                   = 0;
    reserved0                = 0x3;
    section_length           = 0;
    program_number           = 0;
    reserved1                = 0x3;
    version_number           = 0;
    current_next_indicator   = 0;
    section_number           = 0;
    last_section_number      = 0;
    reserved2                = 0x7;
    PCR_PID                  = 0;
    reserved3                = 0xf;
    program_info_length      = 0;
}

void PmtHeader::pack(uint8_t buf[12])
{
    buf[0] = table_id;
    buf[1] = (section_length >> 8) & 0xF;
    buf[1] |= (reserved0 << 4) & 0x30;
    buf[1] |= (section_syntax_indicator << 7) & 0x80;
    buf[2]  = section_length & 0xFF;
    buf[3]  = (program_number >> 8) & 0xFF;
    buf[4]  = program_number & 0xFF;
    buf[5] = current_next_indicator & 0x01;
    buf[5] |= (version_number << 1) & 0x3E;
    buf[5] |= (reserved1 << 6) & 0xC0;
    buf[6] = section_number;
    buf[7] = last_section_number;

    buf[8] = (PCR_PID >> 8) & 0x1F;
    buf[8] |= (reserved2 << 5) & 0xE0;
    buf[9] = PCR_PID & 0xFF;

    buf[10] = (program_info_length >> 8) & 0xF;
    buf[10] |= (reserved3 << 4) & 0xF0;
    buf[11] = program_info_length & 0xFF;
}

PesHeader::PesHeader()
{
    packet_start_code         = 0x1;
    stream_id                 = 0;
    pes_packet_length         = 0;
    const0                    = 0x2;
    pes_scrambling_control    = 0;
    pes_priority              = 0;
    data_alignment_indicator  = 0;
    copyright                 = 0;
    original_or_copy          = 0;
    pts_dts_flags             = 0;
    escr_flag                 = 0;
    es_rate_flag              = 0;
    dsm_trick_mode_flag       = 0;
    additional_copy_info_flag = 0;
    pes_crc_flag              = 0;
    pes_extention_flag        = 0;
    header_data_length        = 0;
}

void PesHeader::pack(uint8_t buf[9])
{
    memset(buf, 0, PES_HEADER_SIZE);
    buf[2]  = packet_start_code & 0xFF;
    buf[3]  = stream_id & 0xFF;
    buf[4]  = (pes_packet_length >> 8) & 0xFF;
    buf[5]  = pes_packet_length & 0xFF;

    buf[6]  = original_or_copy & 0x01;
    buf[6] |= (copyright << 1) & 0x02;
    buf[6] |= (data_alignment_indicator << 2) & 0x04;
    buf[6] |= (pes_priority << 3) & 0x08;
    buf[6] |= (pes_scrambling_control << 4) & 0x30;
    buf[6] |= (const0 << 6) & 0xC0;

    buf[7]  = pes_extention_flag & 0x01;
    buf[7] |= (pes_crc_flag << 1) & 0x02;
    buf[7] |= (additional_copy_info_flag << 2) & 0x04;
    buf[7] |= (dsm_trick_mode_flag << 3) & 0x08;
    buf[7] |= (es_rate_flag << 4) & 0x10;
    buf[7] |= (escr_flag << 5) & 0x20;
    buf[7] |= (pts_dts_flags << 6) & 0xC0;

    buf[8]  = header_data_length & 0xFF;
}

AdaptationHeader::AdaptationHeader()
{
    adaptation_field_length              = 0;
    discontinuity_indicator              = 0;
    random_access_indicator              = 0;
    elementary_stream_priority_indicator = 0;
    pcr_flag                             = 0;
    opcr_flag                            = 0;
    splicing_point_flag                  = 0;
    transport_private_data_flag          = 0;
    adaptation_field_extension_flag      = 0;
}

void AdaptationHeader::pack(uint8_t buf[2])
{
    buf[0] = adaptation_field_length;
    if (adaptation_field_length)
    {
        buf[1]  = adaptation_field_extension_flag & 0x1;
        buf[1] |= (transport_private_data_flag << 1) & 0x2;
        buf[1] |= (splicing_point_flag << 2) & 0x4;
        buf[1] |= (opcr_flag << 3) & 0x8;
        buf[1] |= (pcr_flag << 4) & 0x10;
        buf[1] |= (elementary_stream_priority_indicator << 5) & 0x20;
        buf[1] |= (random_access_indicator << 6) & 0x40;
        buf[1] |= (discontinuity_indicator << 7) & 0x80;
    }
}



