/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */
#include <mpegts/ts_demuxer.hpp>
#include <mpegts/ts_misc.h>
#include <iostream>
#include <cassert>
#include <sstream>
#include <string>
using namespace std;

static string getFileName(bool video, uint16_t id, uint16_t prog)
{
    stringstream outMediaPath;
    outMediaPath << (video ? "video" : "audio") << "_id_";
    outMediaPath << id;
    return outMediaPath.str();
}

Stream::Stream(const char *filename, uint16_t id, uint16_t pid)
    : m_file(NULL), m_id(id), m_pid(pid)
{
    assert(filename != NULL);
    name = filename;
    //m_file = fopen(filename, "wb");
    //if (!m_file) {
    //    fprintf(stderr, "stream=%d PID=%d can't open file %s!\n",m_id, m_pid, filename);
    //}
}

Stream::~Stream()
{
    if (m_file) fclose(m_file);
}

void Stream::Write(Packet::const_iterator b, Packet::const_iterator e)
{
    if (!m_file) return; // Failed to create stream. Exit quietly
    while (b != e)
    {
        int r = fputc(*b++, m_file);
        if (r == EOF)
        {
            fprintf(stderr, "stream=%d PID=%d write failed!\n", m_id, m_pid);
            fclose(m_file);
            m_file = NULL;
        }
    }
}

void Stream::Write_Vec(Packet::const_iterator b, Packet::const_iterator e, vector<uint8_t> &pes_data)
{
    while (b != e)
    {
        pes_data.push_back(*b++);
    }
}

TsDemuxer::TsDemuxer(): m_pnum(0)
{
    // List know PIDs in the filters.
    m_filters.insert(make_pair(PACKET_ID::NULL_PID, DEMUXER_EVENT_NIL));
    m_filters.insert(make_pair(PACKET_ID::PAT_PID, DEMUXER_EVENT_PAT));
}

void TsDemuxer::reset()
{
    m_pnum = 0;
    mDts = 0;
    mPts = 0;
}

TsDemuxer::~TsDemuxer()
{
    for (Streams::iterator i = m_streams.begin(), e = m_streams.end(); i != e; ++i)
    {
        delete (*i).second;
    }
}

//Entrance of depack the packet
bool TsDemuxer::demuxPacket(const Packet &packet)
{
    ++m_pnum;  //packet num +1
    Packet::const_iterator p = packet.begin(), e = packet.end();
    part_data.clear();
    PacketHeader header;
    if (!readHeader(p, e, header))
        return false;

    uint16_t &id = header.id;

    Filters::iterator f = m_filters.find(id);
    if (f != m_filters.end())
    {
        DemuxerEvents E = (*f).second;
        switch (E)
        {
        case DEMUXER_EVENT_NIL:
            return true;
        case DEMUXER_EVENT_PES:
            if (!readPES(p, e, header, id))
                return false;
            break;
        case DEMUXER_EVENT_PAT:
            if (!readPAT(p, e))
                return false;
            mOutPropty.add_unknown();
            break;
        case DEMUXER_EVENT_PMT:
            if (!readPMT(p, e))
                return false;
            mOutPropty.add_unknown();
            break;
        default:
            fprintf(stderr, "[warning]: pakcet id: %lu filtered but not processed!.\n", m_pnum);
            break;
        }
    }
    else   //not transport PAT and PMT, and get the PES stream directly!
    {
        if (checkPES(p, e, header))
        {
            if (!readPES(p, e, header, id))
                return false;
        }
    }
    return true;
}

bool TsDemuxer::readPES(Packet::const_iterator &p, Packet::const_iterator e, PacketHeader &header, uint16_t id)
{
    if (header.pusi)
    {
        /// PES PSI
        if (p[0] == 0x0 && p[1] == 0x0 && p[2] == 0x01)
        {
            uint8_t pts_dts_flags = 0;
            p += 3;
            const uint8_t sidx = *p++;    //get 8 bit to indicate the media type 0xc0 ,0xe0
            const bool video = VIDEO_PACKET_ID(sidx);
            const bool expected = AUDIO_PACKET_ID(sidx) || video;
            if (!expected)
            {
                fprintf(stderr,
                        "[Stream Error]: unexpected input of stream type at the %lu packet.: 0x%02x\n",
                        m_pnum, sidx);
                return false;
            }
            Streams::iterator s = m_streams.find(id);
            if (s == m_streams.end())
            {
                if (!registerStream(id, Program(), video))
                    return false;
                s = m_streams.find(id);
            }
            Stream *S = (*s).second;

            // escape packet length
            p += 2;
            // The rest of the header is of variable length.
            // but we know that there are at least 3 bytes.
            //The third is the length of the optional fields followed by a byte that stores the
            p += 1;
            pts_dts_flags = (*p & 0xc0) >> 6;
            p += 1;

            const uint8_t pl = *p++;  //pes data length, the length of latter data
            if (p + pl >= e)
            {
                fprintf(stderr, "[Stream Error]: the %lu packet has invalid PES length.", m_pnum);
                return false;
            }
            if (pts_dts_flags == 0x02)
            {
                mPts = mDts = ts_read_pts(p);
            }
            else if (pts_dts_flags == 0x03)
            {
                mPts = ts_read_pts(p);
                mDts = ts_read_pts(p + 5);
            }
            else
            {
                fprintf(stderr, "[Error] pts_dts_flag is invalid value.\n");
                return false;
            }
            p += pl;
            S->Write_Vec(p, e, part_data);
            Typemaps::iterator typeptr = m_typemaps.find(id);
            if (typeptr == m_typemaps.end())
            {
                fprintf(stderr, "[Error] find media type false!\n");
                return false;
            }
            mOutPropty.add_known(part_data, id, mPts, mDts, typeptr->second);
        }
        else
        {
            fprintf(stderr, "[Stream Error]: the %lu packet have no PES start code.", m_pnum);
            return false;
        }
    }
    else
    {
        Streams::iterator s = m_streams.find(id);
        if (s == m_streams.end())
        {
            fprintf(stderr, "[Stream Error]: the %lu packet has a new stream id %d w/o pusi.",
                    m_pnum, id);
            return false;
        }
        Stream *S = (*s).second;
        S->Write_Vec(p, e, part_data);
        Typemaps::iterator typeptr = m_typemaps.find(id);
        if (typeptr == m_typemaps.end())
        {
            fprintf(stderr, "[Error] find media type false!\n");
            return false;
        }
        mOutPropty.add_known(part_data, id, mPts, mDts, typeptr->second);
    }
    return true;
}

//pes start code: 0x000001 fixed
bool TsDemuxer::checkPES(Packet::const_iterator &p, Packet::const_iterator e, PacketHeader &header)
{
    if (header.pusi)
    {
        return (p[0] == 0x0 && p[1] == 0x0 && p[2] == 0x01);
    }
    return false;
}

bool TsDemuxer::readPMT(Packet::const_iterator &p, Packet::const_iterator e)
{

    const uint8_t pointer = *p++;
    p += pointer; // Jump to table at pointer offset
    bool done = false;
    // Tables can be repeated.
    while (p < e)
    {
        if (p[0] == STUFFING_BYTE)
        {
            if (!done)
            {
                fprintf(stderr, "[error]: packet=%lu, no data in PMT table!\n", m_pnum);
                return false;
            }
            return true;
        }
        const uint8_t id = *p++;
        if (id == TABLE_ID::NIL_TABLE || id != TABLE_ID::PMS_TABLE)
        {
            if (!done)
            {
                fprintf(stderr, "[error]: PMT packet have wrong table id!");
                return false;
            }
            return true;
        }

        const bool ssi = static_cast<bool>(*p & 0x80); // PAT/PMT/CAT == 1
        if (!ssi)
        {
            fprintf(stderr, "[error]: packet=%lu, PMT section syntax indicator not set!\n", m_pnum);
            return false;
        }

        const bool pb = static_cast<bool>(*p & 0x40); // PAT/PMT/CAT == 0
        if (pb)
        {
            fprintf(stderr, "[error]: packet=%lu, PMT private bit is set.\n", m_pnum);
            return false;
        }

        if (((*p & 0x30) >> 4) != 0x03)
        {
            fprintf(stderr, "[error]: packet=%lu, PMT PSI reserved bits not set.\n", m_pnum);
            return false;
        }

        if (p + 1 >= e)
        {
            fprintf(stderr, "[error]: packet=%lu, Not enough data in PMT table.\n", m_pnum);
            return false;
        }
        const uint16_t slength = uint16_t((p[0] & 0x03) << 10) | uint16_t(p[1]);
        p += 2;
        if (p + slength >= e)
        {
            fprintf(stderr, "[error]: packet=%lu, PMT bad section length.\n", m_pnum);
            return false;
        }
        PacketSection section;
        if (!readSection(p, e, section))
            return false;
        // Get the program this PMT referring to
        Programs::const_iterator prog = m_programs.find(section.id);
        if (prog == m_programs.end())
        {
            fprintf(stdout, "[error]: packet=%lu, PMT references non existing PROGRAM=%d\n", m_pnum, section.id);
            return false;
        }
        if (((*p & 0xE0) >> 5) != 0x07)
        {
            fprintf(stdout, "[error]: packet=%lu, PMT reserved bits not set.\n", m_pnum);
            return false;
        }
        if (p + 1 >= e)
        {
            fprintf(stdout, "[error]: packet=%lu, Not enough data in PMT table.\n", m_pnum);
            return false;
        }
        //PID pcrid = uint16_t((p[0]&0x1F)<<8) | uint16_t(p[1]); // clock frequency
        p += 2;
        // expect PES packet with clock frequency
        //m_filters.insert(make_pair(pcrid, DEMUXER_EVENT_PCR));

        if (((*p & 0xF0) >> 4) != 0x0F)
        {
            fprintf(stdout, "[error]: packet=%lu, PMT reserved bits not set.\n", m_pnum);
            return false;
        }
        // program descriptors
        if (p + 1 >= e)
        {
            fprintf(stdout, "[error]: packet=%lu, Not enough data in PMT table.\n", m_pnum);
            return false;
        }
        const uint16_t pinfol = uint16_t((p[0] & 0x03) << 8) | uint16_t(
                                    p[1]); //program info length, the first two bits are 00, so it is 0x03 rather than 0x0f
        p += 2;
        if (p + pinfol >= e)
        {
            cerr << "[ERROR]: PKT#" << m_pnum << " PMT bad program info length." << endl;
            return false;
        }
        p += pinfol;

        // read data stream info
        if (!readESD(p, e, (*prog).second))
            return false;

        // End of the section
        //uint32_t crc = uint32_t(p[0]<<24) | uint32_t(p[1]<<16) | uint32_t(p[2]<<8) | uint32_t(p[3]);
        p += 4;
        //cout << "CRC: " << hex << crc << dec << endl;
        done = true;
    }
    return done;
}

bool TsDemuxer::readESD(Packet::const_iterator &p, Packet::const_iterator e, const Program &prog)
{
    bool found = false;
    while (p + 3 < e)
    {
        // watch out not to read CRC
        if (p[4] == STUFFING_BYTE)
        {
            if (!found)
            {
                fprintf(stderr, "[error]: packet=%lu, empty ESD section!\n", m_pnum);
                return false;
            }
            return true;
        }

        const uint8_t st = *p++;     //stream type  8 bit;
        if (((*p & 0xE0) >> 5) != 0x07)
        {
            fprintf(stderr, "[error]: packet=%lu, ESD reserved bits not set.\n", m_pnum);
            return false;
        }

        // See what is the stream type
        const bool video = VIDEO_ES_TYPE_ID(st);
        const bool needed = AUDIO_ES_TYPE_ID(st) || video;

        uint16_t id = uint16_t((p[0] & 0x1F) << 8) | uint16_t(p[1]); //the stream id of the program packet.
        p += 2;

        if (((*p & 0xF0) >> 4) != 0x0F)
        {
            fprintf(stderr, "[error]: packet=%lu, ESD reserved bits not set.\n", m_pnum);
            return false;
        }

        //const uint16_t esil = uint16_t((p[0]&0x03)<<8) | uint16_t(p[1]);  //ES_info_length
        const uint16_t esil = uint16_t((p[0] & 0x0f) << 8) | uint16_t(p[1]); //ES_info_length
        p += 2;

        if (p + esil >= e)
        {
            fprintf(stderr, "[error]: packet=%lu, ESD invalid info length.\n", m_pnum);
            return false;
        }
        p += esil;

        if (needed)
        {
            if (!registerTypemap(id, ES_TYPE_ID(st)))
            {
                return false;
            }
            if (!registerStream(id, prog, video))
                return false;
            found = true;
        }
    }
    return true;
}

bool TsDemuxer::registerTypemap(uint16_t id, ES_TYPE_ID mtype)
{
    Typemaps::iterator s = m_typemaps.find(id);
    if (s == m_typemaps.end())
    {
        m_typemaps.insert(make_pair(id, mtype));
    }
    return true;
}

bool TsDemuxer::registerStream(uint16_t id, const Program &prog, bool video)
{
    Streams::iterator s = m_streams.find(id);
    //fprintf(stdout,"oooooo!\n");
    if (s == m_streams.end())
    {
        string filename = getFileName(video, id, prog.id);
        Stream *S = new Stream(filename.c_str(), id, prog.id);
        m_streams.insert(make_pair(id, S));
        //fprintf(stdout, "packet=%lu, stream=%d, program=%d, type=%s\n", m_pnum, id, prog.id, (video ? "video" : "audio"));
        // expect packet for stream
        m_filters.insert(make_pair(id, DEMUXER_EVENT_PES));
    }
    else
    {
        if ((*s).second->GetPId() != prog.id && prog.id != PACKET_ID::NULL_PID)
        {
            //fprintf(stdout, "[warning]: packet=%lu, stream=%d, program=%d, appeared in program=%d\n", m_pnum, id,
            //        (*s).second->GetPId(), prog.id);
            (*s).second->SetPId(prog.id);
        }
    }
    return true;
}

/// ////////////////////////////////////////////////////////////////////////////
bool TsDemuxer::readPAT(Packet::const_iterator &p, Packet::const_iterator e)
{
    const uint8_t pointer =
        *p++;   //PAT(Program Association Table)'s head pointer is a 1byte data, and it indicate the offset of the PAS table
    p += pointer; // Jump to table at pointer offset

    bool done = false; // Must get at least 1 Program
    // Tables can be repeated.
    while (p < e)
    {
        const uint8_t id = *p++;
        if (id == STUFFING_BYTE)
        {
            if (!done)
            {
                fprintf(stderr, "[error]: there is a empty PAT packet!\n");
                return false;
            }
            return true;
        }
        else if (id == TABLE_ID::NIL_TABLE)
        {
            if (!done)
            {
                fprintf(stderr, "[error]: PAT packet have wrong table id!");
                return false;
            }
            return true;
        }
        else if (id != TABLE_ID::PAS_TABLE)
        {
            fprintf(stderr, "[error]: packet: %lu expected PAS table id = %d but find id=%d\n", m_pnum, TABLE_ID::PAS_TABLE, id);
            return false;
        }

        const bool ssi = static_cast<bool>(*p & 0x80); // PAT/PMT/CAT == 1   section syntax indicator
        if (!ssi)
        {
            fprintf(stderr, "[error]: packet=%lu, PAT section syntax indicator not set!\n", m_pnum);
            return false;
        }

        const bool pb = static_cast<bool>(*p & 0x40); // PAT/PMT/CAT == 0   private bit set to 0
        if (pb)
        {
            fprintf(stderr, "[error]: packet=%lu, PAT private bit is set!\n", m_pnum);
            return false;
        }

        if (((*p & 0x30) >> 4) != 0x03)         //reserved bits set to 1
        {
            fprintf(stderr, "[error]: packet=%lu, PAT PSI reserved bits not set.\n", m_pnum);
            return false;
        }

        if (p + 1 >= e)       //section need 12 bit
        {
            fprintf(stderr, "[error]: packet=%lu, PAT PSI not enough data.\n", m_pnum);
            return false;
        }

        const uint16_t slength = uint16_t((p[0] & 0x03) << 8) | uint16_t(p[1]);
        p += 2;
        if (p + slength >= e)
        {
            fprintf(stderr, "[error]: packet=%lu, PAT bad section length.\n", m_pnum);
            return false;
        }
        //this time the pointer p come to the front of transport stream id
        PacketSection section;
        if (!readSection(p, e, section))
            return false;

        // Read PAT, can be repeated
        if (!readPrograms(p, e))
            return false;

        // End of the section
        //uint32_t crc = uint32_t(p[0]<<24) | uint32_t(p[1]<<16) | uint32_t(p[2]<<8) | uint32_t(p[3]);
        //cout << "CRC: " << hex << crc << dec << endl;
        p += 4;
        done = true;
    }

    return done;
}


/// Read PAT tables
bool TsDemuxer::readPrograms(Packet::const_iterator &p, Packet::const_iterator e)
{
    bool done = false;
    // Read PAT table till end of packet (CRC)
    while (p + 3 < e)
    {
        if (p[4] == STUFFING_BYTE)
        {
            // Read PAT table till end of packet (CRC+STUFFING)
            break;
        }
        uint16_t pnum = uint16_t(p[0] << 8) | uint16_t(p[1]); //program number id
        p += 2;

        if (((*p & 0xE0) >> 5) != 0x07)        //reserved bits need to be 1
        {
            cerr << "[ERROR]: PKT#" << m_pnum << " PAT reserved bits not set." << endl;
            return false;
        }
        uint16_t pmid = uint16_t((p[0] & 0x1F) << 8) | uint16_t(p[1]); //id of the pmt table for the program
        p += 2;

        registerProgram(pnum, pmid);
        done = true;
    }
    if (!done)
    {
        fprintf(stderr, "[error]: packet=%lu, PAT can't find programs!\n", m_pnum);
    }
    return done;
}


void TsDemuxer::registerProgram(uint16_t id, uint16_t pid)
{
    assert(id != PACKET_ID::NULL_PID);

    Programs::iterator p = m_programs.find(id);
    if (p == m_programs.end())
    {
        //fprintf(stdout, "packet=%lu, program=%d, PMT=%d\n", m_pnum, id, pid);
        Program P;
        P.id = id;
        P.pid = pid;
        m_programs.insert(make_pair(id, P));
        m_filters.insert(make_pair(pid, DEMUXER_EVENT_PMT));
    }
    else
    {
        if (p->second.pid != pid)
        {
            //fprintf(stdout, "[warning]: packet=%lu, program=%d, PMT=%d appeared with new PMT=%d\n", m_pnum, id, p->second.pid, pid);
            p->second.pid = pid;
            m_filters.insert(make_pair(pid, DEMUXER_EVENT_PMT));
        }
    }
}



bool TsDemuxer::readSection(Packet::const_iterator &p, Packet::const_iterator e, PacketSection &section)
{
    if (p + 4 >= e)
    {
        fprintf(stderr, "packet=%lu, has not got enough data in PSI section.\n", m_pnum);
        return false;
    }

    section.id = uint16_t(p[0] << 8) | uint16_t(p[1]);
    p += 2;

    if (((*p & 0xC0) >> 6) != 0x03) //all need 1
    {
        fprintf(stderr, "[error]: section reserved bits not set!\n");
        return false;
    }
    section.version = (*p & 0x3E) >> 1;
    section.cni = static_cast<bool>(*p++ & 0x1); //current next indicator

    section.sn = *p++;                 //section number
    section.lsn = *p++;                //last section number
    return true;
}


bool TsDemuxer::readHeader(Packet::const_iterator &p, Packet::const_iterator e, PacketHeader &header)
{
    if (*p != SYNC_BYTE)
    {
        fprintf(stderr, "[error]: packet=%lu, has no sync byte!\n", m_pnum);
        return false;
    }
    p++;

    header.tei = static_cast<bool>(*p & 0x80);
    header.pusi = static_cast<bool>(*p & 0x40);   //payload start indicator
    header.tp = static_cast<bool>(*p & 0x20);
    header.id = uint16_t((p[0] & 0x1F) << 8) | uint16_t(p[1]);
    p += 2;

    // Indicates whether the header has adjustment fields or payload. '00' is reserved for future use by ISO/IEC; '01' contains payload only
    //load, no adjustment field; '10' no payload, only adjustment field; '11' payload after the adjustment field, the first byte in the adjustment field indicates the length of the adjustment field
    //length, the position where the payload starts should be offset by [length] bytes. Empty packets should be '10'
    header.afc = (((*p & 0x30) >> 4) == 0x2) || (((*p & 0x30) >> 4) == 0x3);
    header.payload = (((*p & 0x30) >> 4) == 0x1) || (((*p & 0x30) >> 4) == 0x3);
    header.cc = *p & 0xF;
    p++;

    // Greedy, just escape the adaptation control field
    if (header.afc)
    {
        const uint8_t l = *p++;
        p += l;
    }
    //Here p is in front of the effective field
    return true;
}
