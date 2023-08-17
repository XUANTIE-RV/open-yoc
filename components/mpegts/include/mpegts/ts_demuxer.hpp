/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_DEMUXER_HPP__
#define __TS_DEMUXER_HPP__

#include <mpegts/ts_typedef.h>
#include <mpegts/ts_packet.hpp>
#include <cstdio>
#include <map>
#include <set>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class Stream
{
public:
    Stream(const char *filename, uint16_t id, uint16_t pid);
    Stream(const Stream &) = delete;
    Stream &operator=(const Stream &) = delete;
    ~Stream();

    uint16_t GetId() const
    {
        return m_id;
    }
    uint16_t GetPId() const
    {
        return m_pid;
    }
    void SetPId(uint16_t id)
    {
        m_pid = id;
    }

    /// Write data from packet to sink
    void Write(Packet::const_iterator b, Packet::const_iterator e);
    void Write_Vec(Packet::const_iterator b, Packet::const_iterator e, vector<uint8_t> &pes_data);

private:
    FILE *m_file;
    string name;
    uint16_t m_id; /// Packet identifier for this stream
    uint16_t m_pid; /// Program id for this stream
};

struct Program
{
    uint16_t id; /// Program ID
    uint16_t pid; /// Id of the PMT table for this program

    bool operator==(const Program &p)
    {
        return id == p.id;
    }
    bool operator!=(const Program &p)
    {
        return id != p.id;
    }
    friend bool operator<(const Program &p1, const Program &p2);

    Program() : id(PACKET_ID::NULL_PID), pid(PACKET_ID::NULL_PID)
    {
        // default to NULL packet
    }
};

inline bool operator<(const Program &p1, const Program &p2)
{
    return p1.id < p2.id;
}


/// Type of packets Demuxer is listening to
enum DemuxerEvents
{
    DEMUXER_EVENT_PCR,
    DEMUXER_EVENT_PMT,
    DEMUXER_EVENT_PES,
    DEMUXER_EVENT_PAT,
    DEMUXER_EVENT_NIL
};

class outPropty
{
public:
    vector<uint8_t>  pes_data;
    int              stream_id;
    uint64_t         pts;
    uint64_t         dts;
    ES_TYPE_ID       estype;
    void reset()
    {
        pes_data.clear();
    }
    void add_unknown()
    {
        stream_id = -1;
        pts = 0;
        dts = 0;
        estype = ES_TYPE_ID::UNKNOWN;
        pes_data.clear();
        return;
    }
    void add_known(vector<uint8_t>part_data, int stream_id_in, uint64_t pts_in, uint64_t dts_in, ES_TYPE_ID mtype)
    {
        pes_data.clear();
        pes_data.assign(part_data.begin(), part_data.end());
        stream_id = stream_id_in;
        pts = pts_in;
        dts = dts_in;
        estype = mtype;
        return;
    }
};

/// /////////////////////////////////////////////////////////////////////////////
/// Demuxer Interface.
/// Demuxer assumes all packets are related, it will keep list of streams
/// as they appear in TS.
class TsDemuxer
{
public:
    using Programs = std::map<uint16_t, Program>;
    using Streams = std::map<uint16_t, Stream *>;
    using Filters = std::map<uint16_t, DemuxerEvents>;
    using Typemaps = std::map<uint16_t, ES_TYPE_ID>;

    TsDemuxer();
    ~TsDemuxer();

    bool demuxPacket(const Packet &packet);
    vector<uint8_t>          part_data;
    outPropty                mOutPropty;
    void                     reset();
protected:
    /// Fills a header with data
    bool readHeader(Packet::const_iterator &p, Packet::const_iterator e, PacketHeader &header);
    /// Reads PAT packet
    bool readPAT(Packet::const_iterator &p, Packet::const_iterator e);
    /// Reads PMT packet
    bool readPMT(Packet::const_iterator &p, Packet::const_iterator e);
    /// Reads PES packet
    bool readPES(Packet::const_iterator &p, Packet::const_iterator e, PacketHeader &header, uint16_t id);
    bool checkPES(Packet::const_iterator &p, Packet::const_iterator e, PacketHeader &header);
    /// Fills a section header info
    bool readSection(Packet::const_iterator &p, Packet::const_iterator e, PacketSection &section);
    /// Read PAT tables
    bool readPrograms(Packet::const_iterator &p, Packet::const_iterator e);
    /// Read Elementary stream data
    bool readESD(Packet::const_iterator &p, Packet::const_iterator e, const Program &prog);

    /// Add program if new
    void registerProgram(uint16_t id, uint16_t pid);
    bool registerStream(uint16_t id, const Program &prog, bool video);
    bool registerTypemap(uint16_t id, ES_TYPE_ID mtype);
private:
    Programs m_programs; /// List of programs
    Streams m_streams; /// List of streams
    Filters m_filters;
    Typemaps m_typemaps;
    uint64_t m_pnum; /// Packet number
    uint64_t mPts;
    uint64_t mDts;
};

#endif
