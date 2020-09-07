/*
* ubase.h                                                   Version 3.00
*
* smxUSBH Device Framework Utility Routines.
*
* Copyright (c) 2004-2018 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SU_BASE_H
#define SU_BASE_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* USB tansfer direction, USB 1.1 p183 */
#define SU_TRANSFER_DIRECTION_IN          0x80UL

#define SU_PIPE_TYPE_CTRL                 0UL
#define SU_PIPE_TYPE_ISOC                 1UL
#define SU_PIPE_TYPE_BULK                 2UL
#define SU_PIPE_TYPE_INT                  3UL

#define SU_PIPE_TYPE_NUM                  4UL
typedef enum SU_EP_TYPE 
{
    SU_EP_CTRL = SU_PIPE_TYPE_CTRL,
    SU_EP_ISOC = SU_PIPE_TYPE_ISOC,
    SU_EP_BULK = SU_PIPE_TYPE_BULK,
    SU_EP_INT  = SU_PIPE_TYPE_INT,
} SU_EP_TYPE_T;


/*============================================================================
                           GLOBAL MACROS DECLARATION
============================================================================*/

#define SU_OK                             (0) 
#define SU_FAIL                           (-1)
#define SU_BITS_TO_MASK(bits)             ((1 << bits) - 1)

#define SU_CTRL_SETUP                     (2)
#define SU_CTRL_DATA                      (1)  
#define SU_CTRL_STATUS                    (0)
#define SU_CTRL_COMPLETED(pipe)           (SU_PIPE_STAGE(pipe) == 0)
#define SU_CTRL_INCOMPLETED(pipe)         (SU_PIPE_STAGE(pipe) > 0)
#define SU_CTRL_GO_NEXT(pipe)             u8 temp = (SU_PIPE_STAGE(pipe) - 1); SU_PIPE_SETSTAGE(pipe, temp);

#define SU_PIPE_DIRECTION_OUT(x)          ((((x) >> 7) & 1) ^ 1)
#define SU_PIPE_DIRECTION_IN(x)           (((x) >> 7) & 1)
#define SU_PIPE_DIRECTION(x)              (((x) >> 7) & 1)
#define SU_PIPE_DEV(x)                    (((x) >> 8) & 0x7f)
#define SU_PIPE_EP(x)                     (((x) >> 15) & 0xf)
#define SU_PIPE_SLOWSPEED(x)              (((x) >> 26) & 1)
#define SU_PIPE_GETTYPE(x)                (((x) >> 30) & 3)

#define SU_PIPE_ISOCTYPE(x)               (SU_PIPE_GETTYPE((x)) == SU_PIPE_TYPE_ISOC)
#define SU_PIPE_INTTYPE(x)                (SU_PIPE_GETTYPE((x)) == SU_PIPE_TYPE_INT)
#define SU_PIPE_CTRLTYPE(x)               (SU_PIPE_GETTYPE((x)) == SU_PIPE_TYPE_CTRL)
#define SU_PIPE_BULKTYPE(x)               (SU_PIPE_GETTYPE((x)) == SU_PIPE_TYPE_BULK)
#define SU_PIPE_PERIODIC(x)               (SU_PIPE_ISOCTYPE(x) || SU_PIPE_INTTYPE(x))
#define SU_PIPE_NONEPERIODIC(x)           (SU_PIPE_CTRLTYPE(x) || SU_PIPE_BULKTYPE(x))
#define SU_PIPE_OUT                       (0)
#define SU_PIPE_IN                        (1)

#define SU_PIPE_STAGE_POS                 0
#define SU_PIPE_DOPING_POS                3
#define SU_PIPE_DOSSPLIT_POS              4
#define SU_PIPE_DOCSPLIT_POS              5
#define SU_PIPE_DONE_POS                  6
#define SU_PIPE_DIRECTION_POS             7
#define SU_PIPE_DEV_POS                   8
#define SU_PIPE_ENDPOINT_POS              15
#define SU_PIPE_ISOPKTIDX_POS             19
#define SU_PIPE_SLOWSPEED_POS             26
#define SU_PIPE_TYPE_POS                  30              

#define SU_PIPE_STAGE_BITS                3
#define SU_PIPE_DOPING_BITS               1
#define SU_PIPE_DOSSPLIT_BITS             1
#define SU_PIPE_DOCSPLIT_BITS             1
#define SU_PIPE_DONE_BITS                 1
#define SU_PIPE_DIRECTION_BITS            1
#define SU_PIPE_DEV_BITS                  7 
#define SU_PIPE_EP_BITS                   4
#define SU_PIPE_ISOPKTIDX_BITS            7
#define SU_PIPE_SLOWSPEED_BITS            1
#define SU_PIPE_TYPE_BITS                 2

#define SU_PIPE_STAGE_MSK                 (SU_BITS_TO_MASK(SU_PIPE_STAGE_BITS) << SU_PIPE_STAGE_POS)          
#define SU_PIPE_DOPING_MSK                (SU_BITS_TO_MASK(SU_PIPE_DOPING_BITS) << SU_PIPE_DOPING_POS)          
#define SU_PIPE_DOSSPLIT_MASK             (SU_BITS_TO_MASK(SU_PIPE_DOSSPLIT_BITS) << SU_PIPE_DOSSPLIT_POS) 
#define SU_PIPE_DOCSPLIT_MASK             (SU_BITS_TO_MASK(SU_PIPE_DOCSPLIT_BITS) << SU_PIPE_DOCSPLIT_POS) 
#define SU_PIPE_DONE_MASK                 (SU_BITS_TO_MASK(SU_PIPE_DONE_BITS) << SU_PIPE_DONE_POS)
#define SU_PIPE_ISOPKTIDX_MASK            (SU_BITS_TO_MASK(SU_PIPE_ISOPKTIDX_BITS) << SU_PIPE_ISOPKTIDX_POS)

#define SU_PIPE_POS(field)                SU_PIPE_##field##_POS
#define SU_PIPE_BITS(field)               SU_PIPE_##field##_BITS
#define SU_PIPE_MASK(field)               (SU_BITS_TO_MASK(SU_PIPE_BITS(field)) << SU_PIPE_POS(field))

#define SU_PIPE_CLR(field, pipe)          pipe &= ~SU_PIPE_MSK(field)
#define SU_PIPE_GET(field, pipe)          ((pipe & SU_PIPE_MASK(field)) >> SU_PIPE_POS(field))
#define SU_PIPE_SET(field, pipe, val)     SU_PIPE_CLR(field, pipe); \
                                          pipe |= ((val << SU_PIPE_POS(field)) & SU_PIPE_MASK(field));
                                            
#define SU_PIPE_STAGE(x)                  ((x & SU_PIPE_STAGE_MSK) >> SU_PIPE_STAGE_POS)
#define SU_PIPE_DOPING(x)                 ((x & SU_PIPE_DOPING_MSK) >> SU_PIPE_DOPING_POS)
#define SU_PIPE_DOSSPLIT(x)               ((x & SU_PIPE_DOSSPLIT_MASK) >> SU_PIPE_DOSSPLIT_POS)
#define SU_PIPE_DOCSPLIT(x)               ((x & SU_PIPE_DOCSPLIT_MASK) >> SU_PIPE_DOCSPLIT_POS)
#define SU_PIPE_DONE(x)                   ((x & SU_PIPE_DONE_MASK) >> SU_PIPE_DONE_POS)
#define SU_PIPE_ISOPKTIDX(x)              ((x & SU_PIPE_ISOPKTIDX_MASK) >> SU_PIPE_ISOPKTIDX_POS)
#define SU_PIPE_NAKCOUNT(x)               SU_PIPE_ISOPKTIDX(x)
#define SU_PIPE_MAX_NAKCOUNT              (SU_BITS_TO_MASK(SU_PIPE_ISOPKTIDX_BITS))

/* Pipe operation macros */
#define SU_PIPE_SETSTAGE(pipe, stage) \
                             pipe &= ~SU_PIPE_STAGE_MSK; \
                             pipe |= ((stage << SU_PIPE_STAGE_POS) & SU_PIPE_STAGE_MSK);
#define SU_PIPE_SETDOPING(pipe) \
                             pipe |= SU_PIPE_DOPING_MSK;
#define SU_PIPE_SETDOSSPLIT(pipe) \
                             pipe |= SU_PIPE_DOSSPLIT_MASK;
#define SU_PIPE_SETDOCSPLIT(pipe) \
                             pipe |= SU_PIPE_DOCSPLIT_MASK;
#define SU_PIPE_SETDONE(pipe, val) \
                             (val) ? (pipe |= SU_PIPE_DONE_MASK) : (pipe &= ~SU_PIPE_DONE_MASK);
#define SU_PIPE_CLRDOPING(pipe) \
                             pipe &= ~SU_PIPE_DOPING_MSK;
#define SU_PIPE_CLRDOSSPLIT(pipe) \
                             pipe &= ~SU_PIPE_DOSSPLIT_MASK;
#define SU_PIPE_CLRDOCSPLIT(pipe) \
                             pipe &= ~SU_PIPE_DOCSPLIT_MASK;
#define SU_PIPE_CLRDONE(pipe) \
                             pipe &= ~SU_PIPE_DONE_MASK;
#define SU_PIPE_CLRISOPKTIDX(pipe) \
                             pipe &= ~SU_PIPE_ISOPKTIDX_MASK;
#define SU_PIPE_SETISOPKTIDX(pipe, index) \
                             pipe &= ~SU_PIPE_ISOPKTIDX_MASK; \
                             pipe |= ((index << SU_PIPE_ISOPKTIDX_POS) & SU_PIPE_ISOPKTIDX_MASK);
#define SU_PIPE_CLR_NAKCOUNT(pipe) SU_PIPE_CLRISOPKTIDX(pipe)
#define SU_PIPE_SET_NAKCOUNT(pipe) SU_PIPE_SETISOPKTIDX(pipe)                            
#define SU_PIPE_CLR_ALL(pipe) \
                             pipe &= ~(SU_PIPE_STAGE_MSK | \
                                       SU_PIPE_DOPING_MSK | \
                                       SU_PIPE_DOSSPLIT_MASK | \
                                       SU_PIPE_DOCSPLIT_MASK | \
                                       SU_PIPE_DONE_MASK | \
                                       SU_PIPE_ISOPKTIDX_MASK);

/*  Pipe bitmap summary defines pipe characteristics.
    31:30 - pipe type (endpoint type)
    29:27 - unused
    26:26 - low speed flag
    25:19 - ISO: current transferring packet index; BULK: continuous NAK count
    18:15 - EP number
    14: 8 - device number
    7 : 7 - EP direction
    6 : 6 - transfer done flag
    5 : 5 - do csplit flag
    4 : 4 - do ssplit flag
    3 : 3 - do ping flag 
    2 : 0 - stage index for CTRL EP. Stages are: request, data, acknowledge
*/

/* Pipe Transfer and Toggle Macros.
   d = pDevInfo, e = endpoint, o = out, b = bit */
#define SU_CTRL   (u32)(SU_PIPE_TYPE_CTRL << 30)
#define SU_ISOC   (u32)(SU_PIPE_TYPE_ISOC << 30)
#define SU_BULK   (u32)(SU_PIPE_TYPE_BULK << 30)
#define SU_INT    (u32)(SU_PIPE_TYPE_INT  << 30)
#define SU_LSF(d) (u32)((((d)->speed == SU_SPEED_LOW) ? 1 : 0) << 26)
#define SU_EPN(e) (((u32)(e) & 0xf) << 15)
#define SU_DN(d)  (((u32)(d)->devAddr & 0x7f) << 8)
#define SU_IN     (u32)(1 << 7)
#define SU_OUT    0
#define SU_XFER(d,e) (SU_LSF(d) | SU_EPN(e) | SU_DN(d))

#define SU_PIPE_SENDCTRL(d,e) (SU_CTRL | SU_XFER(d,e) | SU_OUT)
#define SU_PIPE_RECVCTRL(d,e) (SU_CTRL | SU_XFER(d,e) | SU_IN)
#define SU_PIPE_SENDBULK(d,e) (SU_BULK | SU_XFER(d,e) | SU_OUT)
#define SU_PIPE_RECVBULK(d,e) (SU_BULK | SU_XFER(d,e) | SU_IN)
#define SU_PIPE_SENDISOC(d,e) (SU_ISOC | SU_XFER(d,e) | SU_OUT)
#define SU_PIPE_RECVISOC(d,e) (SU_ISOC | SU_XFER(d,e) | SU_IN)
#define SU_PIPE_SENDINT(d,e)  (SU_INT  | SU_XFER(d,e) | SU_OUT)
#define SU_PIPE_RECVINT(d,e)  (SU_INT  | SU_XFER(d,e) | SU_IN)
#define SU_PIPE_SENDDEFCRTL(d)(SU_CTRL | SU_LSF(d))

#define SU_TOG(d,o)                ((d)->toggle[(o)])
#define SU_PIPE_GETTOGGLE(d,e,o)   ((SU_TOG(d,o) >> (e)) & 1)
#define SU_PIPE_DOTOGGLE(d,e,o)    (SU_TOG(d,o) ^= (1 << (e)))
#define SU_PIPE_SETTOGGLE(d,e,o,b) (SU_TOG(d,o) = ((SU_TOG(d,o) \
                                                & ~(1 << (e))) | ((b) << (e))))

#define SU_GET_MAX_PKT_SIZE(s) (((s)&0x3FF)*((((s) >> 11)&0x3) + 1))


/*============================================================================
                          STRUCTURES AND OTHER TYPEDEFS
============================================================================*/

typedef struct SU_LIST
{
    struct SU_LIST *next;
    struct SU_LIST *prev;
}SU_LIST;

#define SU_GET_LIST_ITEM(pList, itemType, itemMember) \
                ((itemType *)((u8 *)(pList) - (uint)(&((itemType *)0)->itemMember)))

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

void su_ClrDevMapBit(uint value, u8 * pAddrPara);
void su_SetDevMapBit(uint value, u8 * pAddrPara);
uint su_GetDevMapBit(u8 *pMap, uint next);

void su_ListInit(SU_LIST *pList);
void su_ListAddItem(SU_LIST *pNewItem, SU_LIST *pPrevItem, SU_LIST *pNextItem);
void su_ListAddHead(SU_LIST *pNewItem, SU_LIST *pList);
void su_ListAddTail(SU_LIST *pNewItem, SU_LIST *pList);
void su_ListSplice(SU_LIST *pL1Head, SU_LIST *pL2Tail);
void su_ListDel(SU_LIST *pDelItem);
BOOLEAN  su_ListEmpty(SU_LIST *pList);

u32  su_GetUSBDelayTime(u32 speed, u32 transferDirect, u32 isocFlag, uint pktMaxLen);

#if defined(__cplusplus)
}
#endif

#endif /* SU_BASE_H */

