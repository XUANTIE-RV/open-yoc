/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __TS_DEMUXER_HPP__
#define __TS_DEMUXER_HPP__

#include <map>
#include <mpegts/ts_typedef.h>
#include <mpegts/bytes_io.hpp>
#include <mpegts/ts_packet.hpp>
#include <mpegts/ts_format.hpp>

#define TS_SYNC_HDR_MAX       (2*1024)

class TsDemuxer final
{
    class TsFilter
    {
    public:
        TsFilter(TsDemuxer *dmx);
        virtual ~TsFilter() {};

        virtual int parse(const uint8_t *buf, size_t size)
        {
            return 0;
        };

        /**
         * @brief  collect sub pes or section data togethor for getting complete pes or section
         * @param  [in] buf
         * @param  [in] size
         * @param  [in] is_start
         * @return 0/-1
         */
        virtual int pushData(const uint8_t *buf, size_t size, int is_start)
        {
            return 0;
        };

    public:
        tsf_type_t          mFilterType;
        uint16_t            mPid;
        uint16_t            mLastCC;
        shared_ptr<BytesIO> mData;
        TsDemuxer*          mDmx;
    };

    class TsFilterPes : public TsFilter
    {
    public:
        TsFilterPes(TsDemuxer *dmx) : TsFilter(dmx)
        {
            mUnbound    = 0;
            mPts        = 0;
            mDts        = 0;
            mType       = ES_TYPE_UNKNOWN;
            mFilterType = TS_FILTER_TYPE_PES;
        }

        int parse(const uint8_t *buf, size_t size);
        int pushData(const uint8_t *buf, size_t size, int is_start);

    public:
        uint64_t            mPts;
        uint64_t            mDts;
        es_type_t           mType;
        uint8_t             mUnbound;
    };


    class TsFilterSection : public TsFilter
    {
    public:
        TsFilterSection(TsDemuxer *dmx) : TsFilter(dmx)
        {
            mFilterType = TS_FILTER_TYPE_SECTION;
        }

        int  pushData(const uint8_t *buf, size_t size, int is_start);
    };

    class TsFilterSectionPat : public TsFilterSection
    {
    public:
        TsFilterSectionPat(TsDemuxer *dmx) : TsFilterSection(dmx) {};

        int parse(const uint8_t *buf, size_t size);
    };

    class TsFilterSectionPmt : public TsFilterSection
    {
    public:
        TsFilterSectionPmt(TsDemuxer *dmx) : TsFilterSection(dmx) {};

        int parse(const uint8_t *buf, size_t size);
    };

public:
    TsDemuxer(BytesIO *io);
    ~TsDemuxer();

public:
    /**
     * @brief  get one complete a/v packet
     * @param  [in] packet
     * @return -1: error, 0: sync ok, 1: need more data
     */
    int unpack(TsPacket *packet);

    /**
     * @brief  flush inner pes-data before seeking, etc
     * @return
     */
    void flush();

private:
    /**
     * @brief  get one ts packet buffer(0x47-sync code)
     * @param  [in] packet[TS_PACKET_SIZE]
     * @return -1: error, 0: sync ok, 1: need more data
     */
    int _readSyncPacket(uint8_t packet[TS_PACKET_SIZE]);

    /**
     * @brief  parse the ts packet buffer
     * @param  [in] buf[TS_PACKET_SIZE]
     * @return  0/-1
     */
    int _parseOnePacket(const uint8_t buf[TS_PACKET_SIZE]);

public:
    map<uint16_t, vector<shared_ptr<PmtElement>>>    mPmtInfo;

private:
    TsPacket*                                        _mPacket;
    uint8_t                                          _mStopParse;
    BytesIO*                                         _mIO;
    map<uint16_t, shared_ptr<TsFilter>>              _mFilters;
};

#endif /* __TS_DEMUXER_HPP__ */

