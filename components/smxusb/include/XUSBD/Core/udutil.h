/*
* udutil.h                                                  Version 2.34
*
* smxUSBD Utility Routines.
*
* Copyright (c) 2005-2008 Micro Digital Inc.
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

#ifndef SUD_UTIL_H
#define SUD_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Linked List Handler */

typedef struct SUD_ListHeader_T {
    struct SUD_ListHeader_T *next;
    struct SUD_ListHeader_T *prev;
} SUD_LISTHEADER_T;

/* Declare and init a linked list */
#define SUD_LISTHEADER_BUILD(list) SUD_LISTHEADER_T list = { &(list), &(list) }

void sud_ListInit(SUD_LISTHEADER_T *head);
void sud_ListAddElement(SUD_LISTHEADER_T *elem, SUD_LISTHEADER_T *head);
void sud_ListDelElement(SUD_LISTHEADER_T *entry);


#ifdef __cplusplus
}
#endif

#endif /* SUD_UTIL_H */

