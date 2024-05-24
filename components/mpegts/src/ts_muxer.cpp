/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */
#include <mpegts/ts_typedef.h>
#include <mpegts/ts_muxer.hpp>
#include <mpegts/ts_misc.h>

TsMuxer::TsMuxer()
{
    _mPcrPid         = 0x1FFF;
    _mPsiCurInterval = 0;
    _mPsiInterval    = PSI_INTERVAL_DEFAULT;
    _mIO             = new BytesIO(TS_PACKET_SIZE);
}

TsMuxer::~TsMuxer()
{
    if (_mIO)delete _mIO;
}

void TsMuxer::reset()
{
    if (_mIO) delete _mIO;
    _mPcrPid         = 0x1FFF;
    _mPsiCurInterval = 0;
    _mPsiInterval    = PSI_INTERVAL_DEFAULT;
    _mIO             = new BytesIO(TS_PACKET_SIZE);
    _mPidCC.clear();
    _mPmtInfo.clear();
}

/**
 * @brief  add one pmt table
 * @param  [in] pid : pid of the pmt table (0x10~0x1ffe)
 * @return 0/-1
 */
void TsMuxer::addPmt(uint16_t pid)
{
    if (!(pid >= 0x10 && pid <= 0x1ffe))
    {
        fprintf(stderr, "[error] Use pmt pid not in 0x0010 ~ 0x1ffe! Current pid: 0x%04x\n", pid);
        abort();
    }
    if (_mPmtInfo.find(pid) != _mPmtInfo.end())
    {
        fprintf(stderr, "[error] Pmt pid is exist already! Current pid: 0x%04x\n", pid);
        abort();
    }
    vector<shared_ptr<PmtElement>> vec;
    _mPmtInfo.insert(make_pair(pid, vec));
}

/**
 * @brief  add track-element info of the pmt
 * @param  [in] pid_pmt(0x10~0x1ffe)
 * @param  [in] type
 * @param  [in] pid : pid of the track-element(0x10~0x1ffe)
 * @return 0/-1
 */
void TsMuxer::addPmtElement(uint16_t pid_pmt, ES_TYPE_ID type, uint16_t pid)
{
    PmtElement *element;
    map<uint16_t, vector<shared_ptr<PmtElement>>>::iterator iter;

    if (!(pid >= 0x10 && pid <= 0x1ffe))
    {
        fprintf(stderr, "[error] Use stream pid not in 0x0010 ~ 0x1ffe! Current stream pid: 0x%04x\n", pid);
        abort();
    }
    if (!(pid_pmt >= 0x10 && pid_pmt <= 0x1ffe))
    {
        fprintf(stderr, "[error] Use pmt pid not in 0x0010 ~ 0x1ffe! Current pmt pid: 0x%04x\n", pid_pmt);
        abort();
    }
    element = new PmtElement();
    element->stream_type    = type;
    element->elementary_PID = pid & 0x1fff;

    iter = _mPmtInfo.find(pid_pmt);
    if (iter != _mPmtInfo.end())
    {
        for (int i = 0; i < (int)iter->second.size(); i++)
        {
            if (iter->second[i]->elementary_PID == pid)
            {
                delete element;
                fprintf(stderr, "[error] stream pid is exist already! Current stream pid: 0x%04x\n", pid);
                abort();
            }
        }
        iter->second.push_back(shared_ptr<PmtElement>(element));
    }
    else
    {
        vector<shared_ptr<PmtElement>> vec;
        vec.push_back(shared_ptr<PmtElement>(element));
        _mPmtInfo[pid_pmt] = vec;
    }
}

/**
 * @brief  pack the frame
 * @param  [in] frame : audio or video frame data
 * @return 0/-1
 */
int TsMuxer::pack(const TsFrame *frame)
{
    if (!(frame && frame->frame_setted()))
    {
        fprintf(stderr, "[error] frame params set error!\n");
        abort();
    }
    if (!_mPmtInfo.size())
    {
        fprintf(stderr, "[error] please add pmt & element before!\n");
        abort();
    }
    if (_mPsiCurInterval++ == 0)
    {
        _packPat();
        _packPmt();
    }
    _mPsiCurInterval = _mPsiCurInterval >= _mPsiInterval ? 0 : _mPsiCurInterval;
    _packPes(frame);
    return 0;
}

/**
 * @brief  pack pat of psi
 * @return
 */
void TsMuxer::_packPat()
{
    uint32_t crc;
    uint8_t buf[32];
    TsHeader ts_hdr;
    PatHeader pat_hdr;
    uint16_t program_number = 1;
    uint8_t adaptation_field_length = 0;
    uint16_t pid_pmt = (0x7 << 13) | _mPmtInfo.begin()->first;

    _mIO->resetCurrPos();

    ts_hdr.sync_byte                    = 0x47;
    ts_hdr.transport_error_indicator    = 0;
    ts_hdr.payload_unit_start_indicator = 1;
    ts_hdr.transport_priority           = 0;
    ts_hdr.pid                          = PACKET_ID::PAT_PID;
    ts_hdr.transport_scrambling_control = 0;
    ts_hdr.adaptation_field_control     = ADAPTATION_FIELD_TYPE::FIELD_TYPE_PAYLOAD;
    ts_hdr.continuity_counter           = _getCC(PACKET_ID::PAT_PID);

    pat_hdr.table_id                    = TABLE_ID::PAS_TABLE;
    pat_hdr.section_syntax_indicator    = 1;
    pat_hdr.transport_stream_id         = 0;
    pat_hdr.version_number              = 0;
    pat_hdr.current_next_indicator      = 1;
    pat_hdr.section_number              = 0x0;
    pat_hdr.last_section_number         = 0x0;
    //FIXME: support one program only
    pat_hdr.section_length              = 5 + (4 * 1) + 4;

    ts_hdr.pack(buf);
    _mIO->append((const uint8_t *)&buf[0], TS_HEADER_SIZE);
    _mIO->w8(adaptation_field_length);

    pat_hdr.pack(buf);
    _mIO->append((const uint8_t *)&buf[0], PAT_HEADER_SIZE);
    _mIO->w16be(program_number);
    _mIO->w16be(pid_pmt);

    crc = ts_crc32((const uint8_t *)_mIO->data() + 5, _mIO->size() - 5);
    _mIO->w32be(crc);
    _mIO->append(0xff, TS_PACKET_SIZE - _mIO->getCurrPos());
}


/**
 * @brief  pack pmt of psi
 * @return
 */
void TsMuxer::_packPmt()
{
    int i;
    uint32_t crc;
    uint8_t buf[32];
    TsHeader ts_hdr;
    PmtHeader pmt_hdr;
    int elems_size = 0;
    shared_ptr<PmtElement> element;
    uint8_t adaptation_field_length = 0;
    uint16_t pid_pmt = _mPmtInfo.begin()->first;
    int elems_nb = (int)_mPmtInfo.begin()->second.size();

    _mIO->resetCurrPos();

    for (i = 0; i < elems_nb; i++)
    {
        element = _mPmtInfo.begin()->second[i];
        //cout<<"element ES info length: "<<element->ES_info_length<<endl;
        elems_size += PMT_ELEMENT_HEADER_SIZE + element->ES_info_length;
    }
    ts_hdr.sync_byte                     = 0x47;
    ts_hdr.transport_error_indicator     = 0;
    ts_hdr.payload_unit_start_indicator  = 1;
    ts_hdr.transport_priority            = 0;
    ts_hdr.pid                           = pid_pmt;
    ts_hdr.transport_scrambling_control  = 0;
    ts_hdr.adaptation_field_control      = ADAPTATION_FIELD_TYPE::FIELD_TYPE_PAYLOAD;
    ts_hdr.continuity_counter            = _getCC(pid_pmt);

    pmt_hdr.table_id                     = TABLE_ID::PMS_TABLE;
    pmt_hdr.section_syntax_indicator     = 1;
    pmt_hdr.section_length               = 0;
    pmt_hdr.program_number               = 1;
    pmt_hdr.version_number               = 0;
    pmt_hdr.current_next_indicator       = 1;
    pmt_hdr.section_number               = 0;
    pmt_hdr.last_section_number          = 0;
    pmt_hdr.PCR_PID                      = _mPcrPid;
    pmt_hdr.program_info_length          = 0;
    pmt_hdr.section_length               = PMT_HEADER_SIZE - 3;
    pmt_hdr.section_length              += pmt_hdr.program_info_length + elems_size + 4;

    ts_hdr.pack(buf);
    _mIO->append((const uint8_t *)&buf[0], TS_HEADER_SIZE);
    _mIO->w8(adaptation_field_length);

    pmt_hdr.pack(buf);
    _mIO->append((const uint8_t *)&buf[0], PMT_HEADER_SIZE);

    for (i = 0; i < elems_nb; i++)
    {
        element = _mPmtInfo.begin()->second[i];
        element->pack(buf);
        //FIXME: no ES_info_length now
        _mIO->append((const uint8_t *)&buf[0], PMT_ELEMENT_HEADER_SIZE + element->ES_info_length);
    }

    crc = ts_crc32((const uint8_t *)_mIO->data() + 5, _mIO->size() - 5);
    _mIO->w32be(crc);
    _mIO->append(0xff, TS_PACKET_SIZE - _mIO->getCurrPos());
}

/**
 * @brief  pack one pes of the frame
 * @param  [in] frame : audio or video frame data
 * @return
 */
void TsMuxer::_packPes(const TsFrame *frame)
{
    int first = 1;
    uint8_t buf[32];
    TsHeader ts_hdr;
    PesHeader pes_hdr;
    size_t remain_size = frame->_mSize;
    size_t sent_size = 0;
    size_t pes_size, filled_size, body_size, pad_size;
    int size_now = 0;

    while (remain_size)
    {
        _mIO->resetCurrPos();
        size_now = _mIO->size();

        ts_hdr.sync_byte                     = 0x47;
        ts_hdr.transport_error_indicator     = 0;
        ts_hdr.payload_unit_start_indicator  = first ? 1 : 0;
        ts_hdr.transport_priority            = 0;
        ts_hdr.pid                           = frame->_mPid;
        ts_hdr.transport_scrambling_control  = 0;
        ts_hdr.adaptation_field_control      = ADAPTATION_FIELD_TYPE::FIELD_TYPE_PAYLOAD;
        ts_hdr.continuity_counter            = _getCC(frame->_mPid);

        ts_hdr.pack(buf);
        _mIO->append((const uint8_t *)&buf[0], TS_HEADER_SIZE);

        if (first)
        {
            uint8_t pts_dts[5];
            pes_hdr.stream_id = ts_get_stream_id(frame->_mType);     //modified

            if (frame->_mPts != frame->_mDts)
            {
                pes_hdr.pts_dts_flags      = 0x3;
                pes_hdr.header_data_length = 10;
            }
            else
            {
                pes_hdr.pts_dts_flags      = 0x2;
                pes_hdr.header_data_length = 5;
            }
            pes_size = frame->_mSize + pes_hdr.header_data_length + (PES_HEADER_SIZE - 6);
            pes_hdr.pes_packet_length = pes_size > 0xffff ? 0 : pes_size;
            pes_hdr.pack(buf);
            _mIO->append((const uint8_t *)&buf[0], PES_HEADER_SIZE);

            /* set pts & dts */
            if (pes_hdr.pts_dts_flags == 0x3)
            {
                ts_write_pts(pts_dts, frame->_mPts, 0x3);
                _mIO->append((const uint8_t *)&pts_dts[0], sizeof(pts_dts));

                ts_write_pts(pts_dts, frame->_mDts, 0x1);
                _mIO->append((const uint8_t *)&pts_dts[0], sizeof(pts_dts));
            }
            else
            {
                ts_write_pts(pts_dts, frame->_mDts, 0x2);
                _mIO->append((const uint8_t *)&pts_dts[0], sizeof(pts_dts));
            }
            first = 0;
        }

        filled_size = _mIO->getCurrPos();
        body_size   = TS_PACKET_SIZE - filled_size;

        if (body_size <= remain_size)
        {
            _mIO->append((const uint8_t *)frame->_mData + (frame->_mSize - remain_size), body_size);
            remain_size -= body_size;
            sent_size += body_size;
        }
        else
        {
            int pre_size = _mIO->getCurrPos() - TS_HEADER_SIZE;

            pad_size = body_size - remain_size;
            _mIO->append(0xff, body_size);

            *(_mIO->data() + size_now + 3) |= ADAPTATION_FIELD_TYPE::FIELD_TYPE_BOTH << 4;  //payload and adaption

            if (pre_size > 0)
            {
                memcpy(buf, _mIO->data() + size_now + TS_HEADER_SIZE, pre_size);
                _mIO->setData(buf, pre_size, size_now + TS_HEADER_SIZE + pad_size);
            }

            *(_mIO->data() + size_now + 4) = pad_size - 1;

            if (pad_size >= 2)
            {
                *(_mIO->data() + size_now + 5) = 0;
                //memset(currfp+6, 0xff, pad_size - 2);
            }
            _mIO->setData((const uint8_t *)frame->_mData + (frame->_mSize - remain_size), remain_size,
                          size_now + TS_PACKET_SIZE - remain_size);
            remain_size = 0;
        }
    }
}

/**
 * @brief  get countinue counter of the pid
 * @param  [in] pid
 * @return 0x0~0xf
 */
uint8_t TsMuxer::_getCC(uint16_t pid)
{
    uint8_t cc = 0;

    if (_mPidCC.find(pid) == _mPidCC.end())
    {
        /* insert the pid */
        _mPidCC[pid] = 0;
    }
    else
    {
        _mPidCC[pid] = (_mPidCC[pid] + 1) & 0xF;
        cc           = _mPidCC[pid];
    }
    return cc;
}

uint8_t *TsMuxer::data()
{
    return _mIO->data();
}

size_t TsMuxer::size()
{
    return _mIO->size();
}



