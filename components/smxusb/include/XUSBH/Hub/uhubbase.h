/*
* uhubbase.h                                                Version 3.00
*
* smxUSBH Hub Utility Routines.
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

#ifndef SU_HUB_BASE_H
#define SU_HUB_BASE_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

#define SU_HUB_REQ_CMD_TYPE_DEV \
                (SU_TYPE_STANDARD | SU_TYPE_CLASS_RECIP_DEV)
#define SU_HUB_REQ_CMD_TYPE_HUB \
                (SU_TYPE_CLASS_MAIN | SU_TYPE_CLASS_RECIP_DEV)
#define SU_HUB_REQ_CMD_TYPE_PORT \
                (SU_TYPE_CLASS_MAIN | SU_TYPE_CLASS_RECIP_OTHER)

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* USB device feature selector USB 2.0, Table 9-6. Standard Feature Selectors*/
#define SU_HUB_DEV_FEATURE_CMD_TEST_MODE            2

/* USB hub port set feature command */
/*Hub feature selector, USB 1.1 p268*/
#define SU_HUB_PORT_FEATURE_CMD_CONNECTION          0
#define SU_HUB_PORT_FEATURE_CMD_ENABLE              1
#define SU_HUB_PORT_FEATURE_CMD_SUSPEND             2
#define SU_HUB_PORT_FEATURE_CMD_OVERCURRENT         3
#define SU_HUB_PORT_FEATURE_CMD_RESET               4
#define SU_HUB_PORT_FEATURE_CMD_POWER               8
#define SU_HUB_PORT_FEATURE_CMD_LOWSPEED            9
#define SU_HUB_PORT_FEATURE_CMD_HIGHSPEED           10
#define SU_HUB_PORT_FEATURE_CMD_CONN_CONNECTION     16
#define SU_HUB_PORT_FEATURE_CMD_CONN_ENABLE         17
#define SU_HUB_PORT_FEATURE_CMD_CONN_SUSPEND        18
#define SU_HUB_PORT_FEATURE_CMD_CONN_OVERCURRENT    19
#define SU_HUB_PORT_FEATURE_CMD_CONN_RESET          20
#define SU_HUB_PORT_FEATURE_CMD_PORT_TEST           21

#define SU_HUB_C_FEATURE_CMD_POWER                  0
#define SU_HUB_C_FEATURE_CMD_OVER_CURRENT           1

/* USB hub port status: bit 5 and bit 7 are reserved USB 1.1 p274 */
#define SU_HUB_PORT_STATUS_CONNECT                  0x0001
#define SU_HUB_PORT_STATUS_ENABLE                   0x0002
#define SU_HUB_PORT_STATUS_SUSPEND                  0x0004
#define SU_HUB_PORT_STATUS_OVERCURRENT              0x0008
#define SU_HUB_PORT_STATUS_RESET                    0x0010
#define SU_HUB_PORT_STATUS_POWER                    0x0100
#define SU_HUB_PORT_STATUS_LOWSPEED                 0x0200
#define SU_HUB_PORT_STATUS_HIGHSPEED                0x0400
#define SU_HUB_PORT_STATUS_TEST                     0x0800
#define SU_HUB_PORT_STATUS_INDICATOR                0x1000

/* Hub Class feature numbers, USB 2.0 spec Table 11-17 */
#define SU_HUB_LOCAL_POWER                          0
#define SU_HUB_OVER_CURRENT                         1

/* Hub port status changes */
#define SU_HUB_PORT_STATUS_CONN_CONNECTION          0x0001
#define SU_HUB_PORT_STATUS_CONN_ENABLE              0x0002
#define SU_HUB_PORT_STATUS_CONN_SUSPEND             0x0004
#define SU_HUB_PORT_STATUS_CONN_OVERCURRENT         0x0008
#define SU_HUB_PORT_STATUS_CONN_RESET               0x0010

/* Roothub internal request */
#define SU_HUB_ROOT_SET_PORT_POWER                  0
#define SU_HUB_ROOT_SET_PORT_ENABLE                 1
#define SU_HUB_ROOT_SET_PORT_SUSPEND                2
#define SU_HUB_ROOT_SET_PORT_RESET                  3

#define SU_HUB_ROOT_CLR_PORT_POWER                  10
#define SU_HUB_ROOT_CLR_PORT_ENABLE                 11
#define SU_HUB_ROOT_CLR_PORT_SUSPEND                12
#define SU_HUB_ROOT_CLR_PORT_RESET                  13

#define SU_HUB_ROOT_CLR_PORT_CHG_CONNECTION         20
#define SU_HUB_ROOT_CLR_PORT_CHG_ENABLE             21
#define SU_HUB_ROOT_CLR_PORT_CHG_SUSPEND            22
#define SU_HUB_ROOT_CLR_PORT_CHG_OVERCURRENT        23
#define SU_HUB_ROOT_CLR_PORT_CHG_RESET              24

#define SU_HUB_ROOT_CLR_OVERCURRENT                 30

#define SU_HUB_ROOT_GET_PORT_STATUS                 80
#define SU_HUB_ROOT_GET_PORT_NUMBER                 81
#define SU_HUB_ROOT_GET_CHG_STATUS                  82

typedef struct SU_VIRTUALROOTHUB_STATUS
{
    u8 StatusChanged;
    u8 Connected;
} SU_VIRTUALROOTHUB_STATUS_T;

#if defined(__cplusplus)
extern "C" {
#endif


void su_HubRootProcess(uint index, uint poll);
void su_HubRootEnablePorts(SU_DEV_INFO *pHubDevInfo);

int  su_HubStart(void);
void su_HubStop(void);

#if defined(__cplusplus)
}
#endif

#endif /* SU_HUB_BASE_H */

