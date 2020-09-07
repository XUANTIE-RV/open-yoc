/*
* udevice.h                                                 Version 3.00
*
* smxUSBH Device Framework Descriptor Definitions.
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

#ifndef SU_DEV_H
#define SU_DEV_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* USB types */
#define SU_TYPE_STANDARD                  (0x00 << 5)
#define SU_TYPE_CLASS_MAIN                (0x01 << 5)
#define SU_TYPE_VENDOR                    (0x02 << 5)
#define SU_TYPE_RESERVED                  (0x03 << 5)

/* USB request command type, USB 1.1 p187 */
#define SU_REQ_CMD_GET_STATUS             0x00
#define SU_REQ_CMD_CLEAR_FEATURE          0x01
#define SU_REQ_CMD_SET_FEATURE            0x03
#define SU_REQ_CMD_SET_ADDRESS            0x05
#define SU_REQ_CMD_GET_DESC               0x06
#define SU_REQ_CMD_SET_DESC               0x07
#define SU_REQ_CMD_GET_CFG                0x08
#define SU_REQ_CMD_SET_CFG                0x09
#define SU_REQ_CMD_GET_INTF               0x0A
#define SU_REQ_CMD_SET_INTF               0x0B

/* Value of USB class type, USB 1.1 p183 */
#define SU_TYPE_CLASS_RECIP_DEV           0x00
#define SU_TYPE_CLASS_RECIP_INTF          0x01
#define SU_TYPE_CLASS_RECIP_EP            0x02
#define SU_TYPE_CLASS_RECIP_OTHER         0x03

/* Descriptor types, USB 1.1 187 */
#define SU_DESC_DEV                       0x01
#define SU_DESC_CFG                       0x02
#define SU_DESC_STRING                    0x03
#define SU_DESC_INTF                      0x04
#define SU_DESC_EP                        0x05
#define SU_DESC_OTG                       0x09
#define SU_DESC_IAD                       0x0B

#define SU_DESC_HID                       (SU_TYPE_CLASS_MAIN | 0x01)
#define SU_DESC_REPORT                    (SU_TYPE_CLASS_MAIN | 0x02)
#define SU_DESC_PHYSICAL                  (SU_TYPE_CLASS_MAIN | 0x03)
#define SU_DESC_HUB                       (SU_TYPE_CLASS_MAIN | 0x09)
#define SU_DESC_SMARTCARD                 (SU_TYPE_CLASS_MAIN | 0x01)

/* Endpoints mask, USB 1.1 p204 */
#define SU_EP_ATTR_TYPE_MASK              0x03
#define SU_EP_ADDR_NUM_MASK               0x0F
#define SU_EP_DIR_MASK                    0x80

#define SU_EP_DIR_IN                      0x80

/* Endpoints attribute, USB 1.1 p204 */
#define SU_EP_ATTR_CONTROL                0
#define SU_EP_ATTR_ISOC                   1
#define SU_EP_ATTR_BULK                   2
#define SU_EP_ATTR_INT                    3

#if SU_OTG
/* OTG Feature Selectors */
#define SU_OTG_B_HNP_ENABLE               0x03
#define SU_OTG_A_HNP_SUPPORT              0x04
#define SU_OTG_A_ALT_HNP_SUPPORT          0x05

/* OTG Descriptor Attributes */
#define SU_OTG_SRP_SUPPORT                0x01
#define SU_OTG_HNP_SUPPORT                0x02
#endif

#endif /* SU_DEV_H */

