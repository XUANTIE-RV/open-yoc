/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_PACKET_HPP__
#define __TS_PACKET_HPP__

#include <mpegts/ts_typedef.h>

class Packet
{
public:
    using value_type = uint8_t;
    using iterator = uint8_t*;
    using const_iterator = const uint8_t*;

    iterator begin()
    {
        return &m_data[0];
    }
    const_iterator begin() const
    {
        return &m_data[0];
    }

    iterator end()
    {
        return &m_data[TS_PACKET_SIZE - 1] + 1;
    }
    const_iterator end() const
    {
        return &m_data[TS_PACKET_SIZE - 1] + 1;
    }

    uint8_t m_data[TS_PACKET_SIZE];
};

/// ///////////////////////////////////////////////////////////////////////////
/// Packet Header
struct PacketHeader
{
    bool tei; /// Transport Error Indicator
    bool pusi; /// Payload Uint Start Indicator
    bool tp; /// Transport priority
    uint16_t id; /// Packet ID
    // Transport Scrambling Control goes here
    bool afc; /// Adaptation field
    bool payload; /// Has payload
    uint8_t cc; /// Continuity counter
};

/// ///////////////////////////////////////////////////////////////////////////
/// Packet Section
struct PacketSection
{
    uint16_t id; /// Table ID extension
    uint8_t version;
    bool cni; /// Current/Next indicator
    uint8_t sn; /// Section number
    uint8_t lsn; /// Last section number
};

#endif
