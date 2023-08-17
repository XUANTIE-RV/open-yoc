/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/

#ifndef __TS_MUXER_HPP__
#define __TS_MUXER_HPP__

#include <mpegts/ts_typedef.h>
#include <mpegts/ts_frame.hpp>
#include <mpegts/ts_format.hpp>
#include <mpegts/bytes_io.hpp>
#include <memory>
#include <map>
using namespace std;
#define PSI_INTERVAL_DEFAULT           (15)

class TsMuxer
{
public:
    TsMuxer();
    ~TsMuxer();

    /**
     * @brief  add one pmt table
     * @param  [in] pid : pid of the pmt table(0x10~0x1ffe)
     * @return 0/-1
     */
    void         addPmt(uint16_t pid);

    /**
     * @brief  add track-element info of the pmt
     * @param  [in] pid_pmt(0x10~0x1ffe)
     * @param  [in] type
     * @param  [in] pid : pid of the track-element(0x10~0x1ffe)
     * @return 0/-1
     */
    void         addPmtElement(uint16_t pid_pmt, ES_TYPE_ID type, uint16_t pid);

    /**
     * @brief  pack the frame
     * @param  [in] frame : audio or video frame data
     * @return 0/-1
     */
    int          pack(const TsFrame *frame);
    void         reset();
    uint8_t      *data();
    size_t       size();

private:
    /**
     * @brief  get countinue counter of the pid
     * @param  [in] pid
     * @return 0x0~0xf
     */
    uint8_t     _getCC(uint16_t pid);   //get countinue counter of the pid

    /**
     * @brief  pack pat of psi
     * @return
     */
    void        _packPat();  //pack pat of psi

    /**
    * @brief  pack pmt of psi
    * @return
    */
    void        _packPmt();   //pack pmt of psi

    /**
     * @brief  pack one pes of the frame
     * @param  [in] frame : audio or video frame data
     * @return
     */
    void        _packPes(const TsFrame *frame);  //pack one pes of the frame, [in] frame : audio or video frame data

    uint16_t                                         _mPcrPid;
    uint8_t                                          _mPsiInterval;
    uint8_t                                          _mPsiCurInterval;
    BytesIO                                         *_mIO;
    void                                            *_mUserData;
    map<uint16_t, vector<shared_ptr<PmtElement>>>    _mPmtInfo;
    map<uint16_t, uint8_t>                           _mPidCC;
};

#endif
