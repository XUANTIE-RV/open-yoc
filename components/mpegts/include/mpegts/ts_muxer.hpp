/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#ifndef __TS_MUXER_HPP__
#define __TS_MUXER_HPP__

#include <map>
#include <mpegts/ts_typedef.h>
#include <mpegts/bytes_io.hpp>
#include <mpegts/ts_frame.hpp>
#include <mpegts/ts_format.hpp>

#define PSI_INTERVAL_DEFAULT           (15)

typedef void (*muxer_cb_t)(void *user_data, const uint8_t *packet, size_t size);

class TsMuxer final
{
public:
    TsMuxer();
    ~TsMuxer();

    /**
     * @brief  set muxer callback(must)
     * @param  [in] cb
     * @param  [in] user_data
     * @return
     */
    void setCallback(muxer_cb_t cb, const void *user_data);

    /**
     * @brief  add one pmt table
     * @param  [in] pid : pid of the pmt table(0x10~0x1ffe)
     * @return 0/-1
     */
    int         addPmt(uint16_t pid);

    /**
     * @brief  add track-element info of the pmt
     * @param  [in] pid_pmt(0x10~0x1ffe)
     * @param  [in] type
     * @param  [in] pid : pid of the track-element(0x10~0x1ffe)
     * @return 0/-1
     */
    int         addPmtElement(uint16_t pid_pmt, es_type_t type, uint16_t pid);

    /**
     * @brief  set psi-pack interval(one second)
     * @param  [in] frame_interval
     * @return
     */
    void        setPsiInterval(uint16_t frame_interval = PSI_INTERVAL_DEFAULT);

    /**
     * @brief  pack the frame
     * @param  [in] frame : audio or video frame data
     * @return 0/-1
     */
    int         pack(const TsFrame *frame);
private:
    /**
     * @brief  get countinue counter of the pid
     * @param  [in] pid
     * @return 0x0~0xf
     */
    uint8_t     _getCC(uint16_t pid);

    /**
     * @brief  pack pat of psi
     * @return
     */
    void        _packPat();

    /**
     * @brief  pack pmt of psi
     * @return
     */
    void        _packPmt();

    /**
     * @brief  pack one pes of the frame
     * @param  [in] frame : audio or video frame data
     * @return
     */
    void        _packPes(const TsFrame *frame);
private:
    uint16_t                                         _mPcrPid;
    uint8_t                                          _mPsiInterval;
    uint8_t                                          _mPsiCurInterval;
    BytesIO*                                         _mIO;
    muxer_cb_t                                       _mCallBack;
    void*                                            _mUserData;
    map<uint16_t, vector<shared_ptr<PmtElement>>>    _mPmtInfo;
    map<uint16_t, uint8_t>                           _mPidCC;
};

#endif /* __TS_MUXER_HPP__ */

