/*
 * Apache NuttX
 * Copyright 2020 The Apache Software Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __INCLUDE_USB_MSC_SCSI_H
#define __INCLUDE_USB_MSC_SCSI_H

#include <stdint.h>

#define USB_SCSI_KCQ_NOSENSE                       0x000000 /* SCSI error for No error */
#define USB_SCSI_KCQ_PFATHRESHOLDREACHED           0x005c00 /* SCSI error for No sense - PFA threshold reached */
#define USB_SCSI_KCQSE_RWENOINDEX                  0x010100 /* SCSI error for Recovered Write error - no index */
#define USB_SCSI_KCQSE_RECOVEREDNOSEEKCOMPLETION   0x010200 /* SCSI error for Recovered no seek completion */
#define USB_SCSI_KCQSE_RWEWRITEFAULT               0x010300 /* SCSI error for Recovered Write error - write fault */
#define USB_SCSI_KCQSE_TRACKFOLLOWINGERROR         0x010900 /* SCSI error for Track following error */
#define USB_SCSI_KCQSE_TEMPERATUREWARNING          0x010b01 /* SCSI error for Temperature warning */
#define USB_SCSI_KCQSE_RWEWARREALLOCATED           0x010c01 /* SCSI error for Recovered Write error with auto-realloc - reallocated */
#define USB_SCSI_KCQSE_RWERECOMMENDREASSIGN        0x010c03 /* SCSI error for Recovered Write error - recommend reassign */
#define USB_SCSI_KCQSE_RDWOEUSINGPREVLBI           0x011201 /* SCSI error for Recovered data without ECC using prev logical block ID */
#define USB_SCSI_KCQSE_RDWEUSINGPREVLBI            0x011202 /* SCSI error for Recovered data with ECC using prev logical block ID */
#define USB_SCSI_KCQSE_RECOVEREDRECORDNOTFOUND     0x011401 /* SCSI error for Recovered Record Not Found */
#define USB_SCSI_KCQSE_RWEDSME                     0x011600 /* SCSI error for Recovered Write error - Data Sync Mark Error */
#define USB_SCSI_KCQSE_RWEDSEDATAREWRITTEN         0x011601 /* SCSI error for Recovered Write error - Data Sync Error - data rewritten */
#define USB_SCSI_KCQSE_RWEDSERECOMMENDREWRITE      0x011602 /* SCSI error for Recovered Write error - Data Sync Error - recommend rewrite */
#define USB_SCSI_KCQSE_RWEDSEDATAAUTOREALLOCATED   0x011603 /* SCSI error for Recovered Write error - Data Sync Error - data auto-reallocated */
#define USB_SCSI_KCQSE_RWEDSERECOMMENDREASSIGNMENT 0x011604 /* SCSI error for Recovered Write error - Data Sync Error - recommend reassignment */
#define USB_SCSI_KCQSE_RDWNECORRECTIONAPPLIED      0x011700 /* SCSI error for Recovered data with no error correction applied */
#define USB_SCSI_KCQSE_RREWITHRETRIES              0x011701 /* SCSI error for Recovered Read error - with retries */
#define USB_SCSI_KCQSE_RDUSINGPOSITIVEOFFSET       0x011702 /* SCSI error for Recovered data using positive offset */
#define USB_SCSI_KCQSE_RDUSINGNEGATIVEOFFSET       0x011703 /* SCSI error for Recovered data using negative offset */
#define USB_SCSI_KCQSE_RDUSINGPREVIOUSLBI          0x011705 /* SCSI error for Recovered data using previous logical block ID */
#define USB_SCSI_KCQSE_RREWOEAUTOREALLOCATED       0x011706 /* SCSI error for Recovered Read error - without ECC, auto reallocated */
#define USB_SCSI_KCQSE_RREWOERECOMMENDREASSIGN     0x011707 /* SCSI error for Recovered Read error - without ECC, recommend reassign */
#define USB_SCSI_KCQSE_RREWOERECOMMENDREWRITE      0x011708 /* SCSI error for Recovered Read error - without ECC, recommend rewrite */
#define USB_SCSI_KCQSE_RREWOEDATAREWRITTEN         0x011709 /* SCSI error for Recovered Read error - without ECC, data rewritten */
#define USB_SCSI_KCQSE_RREWE                       0x011800 /* SCSI error for Recovered Read error - with ECC */
#define USB_SCSI_KCQSE_RDWEANDRETRIES              0x011801 /* SCSI error for Recovered data with ECC and retries */
#define USB_SCSI_KCQSE_RREWEAUTOREALLOCATED        0x011802 /* SCSI error for Recovered Read error - with ECC, auto reallocated */
#define USB_SCSI_KCQSE_RREWERECOMMENDREASSIGN      0x011805 /* SCSI error for Recovered Read error - with ECC, recommend reassign */
#define USB_SCSI_KCQSE_RDUSINGECCANDOFFSETS        0x011806 /* SCSI error for Recovered data using ECC and offsets */
#define USB_SCSI_KCQSE_RREWEDATAREWRITTEN          0x011807 /* SCSI error for Recovered Read error - with ECC, data rewritten */
#define USB_SCSI_KCQSE_DLNOTFOUND                  0x011c00 /* SCSI error for Defect List not found */
#define USB_SCSI_KCQSE_PRIMARYDLNOTFOUND           0x011c01 /* SCSI error for Primary defect list not found */
#define USB_SCSI_KCQSE_GROWNDLNOTFOUND             0x011c02 /* SCSI error for Grown defect list not found */
#define USB_SCSI_KCQSE_PARTIALDLTRANSFERRED        0x011f00 /* SCSI error for Partial defect list transferred */
#define USB_SCSI_KCQSE_INTERNALTARGETFAILURE       0x014400 /* SCSI error for Internal target failure */
#define USB_SCSI_KCQSE_PFATHRESHOLDREACHED         0x015d00 /* SCSI error for PFA threshold reached */
#define USB_SCSI_KCQSE_PFATESTWARNING              0x015dff /* SCSI error for PFA test warning */
#define USB_SCSI_KCQSE_INTERNALLOGICFAILURE        0x018100 /* SCSI error for Internal logic failure */
#define USB_SCSI_KCQNR_CAUSENOTREPORTABLE          0x020400 /* SCSI error Not Ready - Cause not reportable. */
#define USB_SCSI_KCQNR_BECOMINGREADY               0x020401 /* SCSI error Not Ready - becoming ready */
#define USB_SCSI_KCQNR_NEEDINITIALIZECOMMAND       0x020402 /* SCSI error Not Ready - need initialize command (start unit) */
#define USB_SCSI_KCQNR_MANUALINTERVENTIONREQUIRED  0x020403 /* SCSI error Not Ready - manual intervention required */
#define USB_SCSI_KCQNR_FORMATINPROGRESS            0x020404 /* SCSI error Not Ready - format in progress */
#define USB_SCSI_KCQNR_SELFTESTINPROGRESS          0x020409 /* SCSI error Not Ready - self-test in progress */
#define USB_SCSI_KCQNR_MEDIUMFORMATCORRUPTED       0x023100 /* SCSI error Not Ready - medium format corrupted */
#define USB_SCSI_KCQNR_FORMATCOMMANDFAILED         0x023101 /* SCSI error Not Ready - format command failed */
#define USB_SCSI_KCQNR_ESUNAVAILABLE               0x023502 /* SCSI error Not Ready - enclosure services unavailable */
#define USB_SCSI_KCQNR_MEDIANOTPRESENT             0x023a00 /* SCSI error Not Ready - media not present */
#define USB_SCSI_KCQDF_BRINGUPFAILORDEGRADEDMODE   0x024080 /* SCSI error Diagnostic Failure - bring-up fail or degraded mode */
#define USB_SCSI_KCQDF_HARDDISKCONTROLLER          0x024081 /* SCSI error Diagnostic Failure - Hard Disk Controller */
#define USB_SCSI_KCQDF_RAMMICROCODENOTLOADED       0x024085 /* SCSI error Diagnostic Failure - RAM microcode not loaded */
#define USB_SCSI_KCQDF_RROCALIBRATION              0x024090 /* SCSI error Diagnostic Failure - RRO Calibration */
#define USB_SCSI_KCQDF_CHANNELCALIBRATION          0x024091 /* SCSI error Diagnostic Failure - Channel Calibration */
#define USB_SCSI_KCQDF_HEADLOAD                    0x024092 /* SCSI error Diagnostic Failure - Head Load */
#define USB_SCSI_KCQDF_WRITEAE                     0x024093 /* SCSI error Diagnostic Failure - Write AE */
#define USB_SCSI_KCQDF_12VOVERCURRENT              0x024094 /* SCSI error Diagnostic Failure - 12V over current */
#define USB_SCSI_KCQDF_OTHERSPINDLEFAILURE         0x024095 /* SCSI error Diagnostic Failure - Other spindle failure */
#define USB_SCSI_KCQDF_SELFRESET                   0x0240b0 /* SCSI error Diagnostic Failure - self-reset */
#define USB_SCSI_KCQDF_CONFIGNOTLOADED             0x024c00 /* SCSI error Diagnostic Failure - config not loaded */

#define USB_SCSI_KCQME_WRITEFAULT                  0x030300 /* SCSI error Medium Error - write fault */
#define USB_SCSI_KCQME_WRITEFAULTAUTOREALLOCFAILED 0x030c02 /* SCSI error Medium Error - write error - auto-realloc failed */
#define USB_SCSI_KCQME_WRITERTLIMITEXCEEDED        0x030cbb /* SCSI error Medium Error - write recovery time limit exceeded */
#define USB_SCSI_KCQME_IDCRCERROR                  0x031000 /* SCSI error Medium Error - ID CRC error */
#define USB_SCSI_KCQME_UNRRE1                      0x031100 /* SCSI error Medium Error - unrecovered read error */
#define USB_SCSI_KCQME_READRETRIESEXHAUSTED        0x031101 /* SCSI error Medium Error - read retries exhausted */
#define USB_SCSI_KCQME_ERRORTOOLONGTOCORRECT       0x031102 /* SCSI error Medium Error - error too long to correct */
#define USB_SCSI_KCQME_UREAUTOREALLOCFAILED        0x031104 /* SCSI error Medium Error - unrecovered read error - auto re-alloc failed */
#define USB_SCSI_KCQME_URERECOMMENDREASSIGN        0x03110b /* SCSI error Medium Error - unrecovered read error - recommend reassign */
#define USB_SCSI_KCQME_READRTLIMITEXCEEDED         0x0311ff /* SCSI error Medium Error - read recovery time limit exceeded */
#define USB_SCSI_KCQME_RECORDNOTFOUND              0x031401 /* SCSI error Medium Error - record not found */
#define USB_SCSI_KCQME_DSME                        0x031600 /* SCSI error Medium Error - Data Sync Mark error */
#define USB_SCSI_KCQME_DSERECOMMENDREASSIGN        0x031604 /* SCSI error Medium Error - Data Sync Error - recommend reassign */
#define USB_SCSI_KCQME_DLE                         0x031900 /* SCSI error Medium Error - defect list error */
#define USB_SCSI_KCQME_DLNOTAVAILABLE              0x031901 /* SCSI error Medium Error - defect list not available */
#define USB_SCSI_KCQME_DLEINPRIMARYLIST            0x031902 /* SCSI error Medium Error - defect list error in primary list */
#define USB_SCSI_KCQME_DLEINGROWNLIST              0x031903 /* SCSI error Medium Error - defect list error in grown list */
#define USB_SCSI_KCQME_FEWERTHAN50PCTDLCOPIES      0x03190e /* SCSI error Medium Error - fewer than 50% defect list copies */
#define USB_SCSI_KCQME_MEDIUMFORMATCORRUPTED       0x033100 /* SCSI error Medium Error - medium format corrupted */
#define USB_SCSI_KCQME_FORMATCOMMANDFAILED         0x033101 /* SCSI error Medium Error - format command failed */
#define USB_SCSI_KCQME_DATAAUTOREALLOCATED         0x038000 /* SCSI error Medium Error - data auto-reallocated */

#define USB_SCSI_KCQHE_NOINDEXORSECTOR              0x040100 /* SCSI error Hardware Error - no index or sector */
#define USB_SCSI_KCQHE_NOSEEKCOMPLETE               0x040200 /* SCSI error Hardware Error - no seek complete */
#define USB_SCSI_KCQHE_WRITEFAULT                   0x040300 /* SCSI error Hardware Error - write fault */
#define USB_SCSI_KCQHE_COMMUNICATIONFAILURE         0x040800 /* SCSI error Hardware Error - communication failure */
#define USB_SCSI_KCQHE_TRACKFOLLOWINGERROR          0x040900 /* SCSI error Hardware Error - track following error */
#define USB_SCSI_KCQHE_UREINRESERVEDAREA            0x041100 /* SCSI error Hardware Error - unrecovered read error in reserved area */
#define USB_SCSI_KCQHE_DSMEINRESERVEDAREA           0x041600 /* SCSI error Hardware Error - Data Sync Mark error in reserved area */
#define USB_SCSI_KCQHE_DLE                          0x041900 /* SCSI error Hardware Error - defect list error */
#define USB_SCSI_KCQHE_DLEINPRIMARYLIST             0x041902 /* SCSI error Hardware Error - defect list error in Primary List */
#define USB_SCSI_KCQHE_DLEINGROWNLIST               0x041903 /* SCSI error Hardware Error - defect list error in Grown List */
#define USB_SCSI_KCQHE_REASSIGNFAILED               0x043100 /* SCSI error Hardware Error - reassign failed */
#define USB_SCSI_KCQHE_NODEFECTSPAREAVAILABLE       0x043200 /* SCSI error Hardware Error - no defect spare available */
#define USB_SCSI_KCQHE_UNSUPPORTEDENCLOSUREFUNCTION 0x043501 /* SCSI error Hardware Error - unsupported enclosure function */
#define USB_SCSI_KCQHE_ESUNAVAILABLE                0x043502 /* SCSI error Hardware Error - enclosure services unavailable */
#define USB_SCSI_KCQHE_ESTRANSFERFAILURE            0x043503 /* SCSI error Hardware Error - enclosure services transfer failure */
#define USB_SCSI_KCQHE_ESREFUSED                    0x043504 /* SCSI error Hardware Error - enclosure services refused */
#define USB_SCSI_KCQHE_SELFTESTFAILED               0x043e03 /* SCSI error Hardware Error - self-test failed */
#define USB_SCSI_KCQHE_UNABLETOUPDATESELFTEST       0x043e04 /* SCSI error Hardware Error - unable to update self-test */
#define USB_SCSI_KCQHE_DMDIAGNOSTICFAIL             0x044080 /* SCSI error Hardware Error - Degrade Mode. Diagnostic Fail */
#define USB_SCSI_KCQHE_DMHWERROR                    0x044081 /* SCSI error Hardware Error - Degrade Mode. H/W Error */
#define USB_SCSI_KCQHE_DMRAMMICROCODENOTLOADED      0x044085 /* SCSI error Hardware Error - Degrade Mode. RAM microcode not loaded */
#define USB_SCSI_KCQHE_SEEKTESTFAILURE              0x044090 /* SCSI error Hardware Error - seek test failure */
#define USB_SCSI_KCQHE_READWRITETESTFAILURE         0x0440a0 /* SCSI error Hardware Error - read/write test failure */
#define USB_SCSI_KCQHE_DEVICESELFRESET              0x0440b0 /* SCSI error Hardware Error - device self-reset */
#define USB_SCSI_KCQHE_COMPONENTMISMATCH            0x0440d0 /* SCSI error Hardware Error - component mismatch */
#define USB_SCSI_KCQHE_INTERNALTARGETFAILURE        0x044400 /* SCSI error Hardware Error - internal target failure */
#define USB_SCSI_KCQHE_INTERNALLOGICERROR           0x048100 /* SCSI error Hardware Error - internal logic error */
#define USB_SCSI_KCQHE_COMMANDTIMEOUT               0x048200 /* SCSI error Hardware Error - command timeout */

#define USB_SCSI_KCQIR_PARMLISTLENGTHERROR          0x051a00 /* SCSI error Illegal Request - parm list length error */
#define USB_SCSI_KCQIR_INVALIDCOMMAND               0x052000 /* SCSI error Illegal Request - invalid/unsupported command code */
#define USB_SCSI_KCQIR_LBAOUTOFRANGE                0x052100 /* SCSI error Illegal Request - LBA out of range */
#define USB_SCSI_KCQIR_INVALIDFIELDINCBA            0x052400 /* SCSI error Illegal Request - invalid field in CDB (Command Descriptor Block) */
#define USB_SCSI_KCQIR_INVALIDLUN                   0x052500 /* SCSI error Illegal Request - invalid LUN */
#define USB_SCSI_KCQIR_INVALIDFIELDSINPARMLIST      0x052600 /* SCSI error Illegal Request - invalid fields in parm list */
#define USB_SCSI_KCQIR_PARAMETERNOTSUPPORTED        0x052601 /* SCSI error Illegal Request - parameter not supported */
#define USB_SCSI_KCQIR_INVALIDPARMVALUE             0x052602 /* SCSI error Illegal Request - invalid parm value */
#define USB_SCSI_KCQIR_IFPTHRESHOLDPARAMETER        0x052603 /* SCSI error Illegal Request - invalid field parameter - threshold parameter */
#define USB_SCSI_KCQIR_INVALIDRELEASEOFPR           0x052604 /* SCSI error Illegal Request - invalid release of persistent reservation */
#define USB_SCSI_KCQIR_IFPTMSFIRMWARETAG            0x052697 /* SCSI error Illegal Request - invalid field parameter - TMS firmware tag */
#define USB_SCSI_KCQIR_IFPCHECKSUM                  0x052698 /* SCSI error Illegal Request - invalid field parameter - check sum */
#define USB_SCSI_KCQIR_IFPFIRMWARETAG               0x052699 /* SCSI error Illegal Request - invalid field parameter - firmware tag */
#define USB_SCSI_KCQIR_COMMANDSEQUENCEERROR         0x052c00 /* SCSI error Illegal Request - command sequence error */
#define USB_SCSI_KCQIR_UNSUPPORTEDENCLOSUREFUNCTION 0x053501 /* SCSI error Illegal Request - unsupported enclosure function */
#define USB_SCSI_KCQIR_SAVINGPARMSNOTSUPPORTED      0x053900 /* SCSI error Illegal Request - Saving parameters not supported */
#define USB_SCSI_KCQIR_INVALIDMESSAGE               0x054900 /* SCSI error Illegal Request - invalid message */
#define USB_SCSI_KCQIR_MEDIALOADOREJECTFAILED       0x055300 /* SCSI error Illegal Request - media load or eject failed */
#define USB_SCSI_KCQIR_UNLOADTAPEFAILURE            0x055301 /* SCSI error Illegal Request - unload tape failure */
#define USB_SCSI_KCQIR_MEDIUMREMOVALPREVENTED       0x055302 /* SCSI error Illegal Request - medium removal prevented */
#define USB_SCSI_KCQIR_SYSTEMRESOURCEFAILURE        0x055500 /* SCSI error Illegal Request - system resource failure */
#define USB_SCSI_KCQIR_SYSTEMBUFFERFULL             0x055501 /* SCSI error Illegal Request - system buffer full */
#define USB_SCSI_KCQIR_INSUFFICIENTRR               0x055504 /* SCSI error Illegal Request - Insufficient Registration Resources */

#define USB_SCSI_KCQUA_NOTREADYTOTRANSITION       0x062800 /* SCSI error Unit Attention - not-ready to ready transition (format complete) */
#define USB_SCSI_KCQUA_DEVICERESETOCCURRED        0x062900 /* SCSI error Unit Attention - POR or device reset occurred */
#define USB_SCSI_KCQUA_POROCCURRED                0x062901 /* SCSI error Unit Attention - POR occurred */
#define USB_SCSI_KCQUA_SCSIBUSRESETOCCURRED       0x062902 /* SCSI error Unit Attention - SCSI bus reset occurred */
#define USB_SCSI_KCQUA_TARGETRESETOCCURRED        0x062903 /* SCSI error Unit Attention - TARGET RESET occurred */
#define USB_SCSI_KCQUA_SELFINITIATEDRESETOCCURRED 0x062904 /* SCSI error Unit Attention - self-initiated-reset occurred */
#define USB_SCSI_KCQUA_TRANSCEIVERMODECHANGETOSE  0x062905 /* SCSI error Unit Attention - transceiver mode change to SE */
#define USB_SCSI_KCQUA_TRANSCEIVERMODECHANGETOLVD 0x062906 /* SCSI error Unit Attention - transceiver mode change to LVD */
#define USB_SCSI_KCQUA_PARAMETERSCHANGED          0x062a00 /* SCSI error Unit Attention - parameters changed */
#define USB_SCSI_KCQUA_MODEPARAMETERSCHANGED      0x062a01 /* SCSI error Unit Attention - mode parameters changed */
#define USB_SCSI_KCQUA_LOGSELECTPARMSCHANGED      0x062a02 /* SCSI error Unit Attention - log select parms changed */
#define USB_SCSI_KCQUA_RESERVATIONSPREEMPTED      0x062a03 /* SCSI error Unit Attention - Reservations pre-empted */
#define USB_SCSI_KCQUA_RESERVATIONSRELEASED       0x062a04 /* SCSI error Unit Attention - Reservations released */
#define USB_SCSI_KCQUA_REGISTRATIONSPREEMPTED     0x062a05 /* SCSI error Unit Attention - Registrations pre-empted */
#define USB_SCSI_KCQUA_COMMANDSCLEARED            0x062f00 /* SCSI error Unit Attention - commands cleared by another initiator */
#define USB_SCSI_KCQUA_OPERATINGCONDITIONSCHANGED 0x063f00 /* SCSI error Unit Attention - target operating conditions have changed */
#define USB_SCSI_KCQUA_MICROCODECHANGED           0x063f01 /* SCSI error Unit Attention - microcode changed */
#define USB_SCSI_KCQUA_CHANGEDOPERATINGDEFINITION 0x063f02 /* SCSI error Unit Attention - changed operating definition */
#define USB_SCSI_KCQUA_INQUIRYPARAMETERSCHANGED   0x063f03 /* SCSI error Unit Attention - inquiry parameters changed */
#define USB_SCSI_KCQUA_DEVICEIDENTIFIERCHANGED    0x063f05 /* SCSI error Unit Attention - device identifier changed */
#define USB_SCSI_KCQUA_INVALIDAPMPARAMETERS       0x063f90 /* SCSI error Unit Attention - invalid APM parameters */
#define USB_SCSI_KCQUA_WORLDWIDENAMEMISMATCH      0x063f91 /* SCSI error Unit Attention - world-wide name mismatch */
#define USB_SCSI_KCQUA_PFATHRESHOLDREACHED        0x065d00 /* SCSI error Unit Attention - PFA threshold reached */
#define USB_SCSI_KCQUA_PFATHRESHOLDEXCEEDED       0x065dff /* SCSI error Unit Attention - PFA threshold exceeded */

#define USB_SCSI_KCQWP_COMMANDNOTALLOWED 0x072700 /* Write Protect - command not allowed */

#define USB_SCSI_KCQAC_NOADDITIONALSENSECODE         0x0b0000 /* Aborted Command - no additional sense code */
#define USB_SCSI_KCQAC_SYNCDATATRANSFERERROR         0x0b1b00 /* Aborted Command - sync data transfer error (extra ACK) */
#define USB_SCSI_KCQAC_UNSUPPORTEDLUN                0x0b2500 /* Aborted Command - unsupported LUN */
#define USB_SCSI_KCQAC_ECHOBUFFEROVERWRITTEN         0x0b3f0f /* Aborted Command - echo buffer overwritten */
#define USB_SCSI_KCQAC_MESSAGEREJECTERROR            0x0b4300 /* Aborted Command - message reject error */
#define USB_SCSI_KCQAC_INTERNALTARGETFAILURE         0x0b4400 /* Aborted Command - internal target failure */
#define USB_SCSI_KCQAC_SELECTIONFAILURE              0x0b4500 /* Aborted Command - Selection/Reselection failure */
#define USB_SCSI_KCQAC_SCSIPARITYERROR               0x0b4700 /* Aborted Command - SCSI parity error */
#define USB_SCSI_KCQAC_INITIATORDETECTEDERRORECEIVED 0x0b4800 /* Aborted Command - initiator-detected error message received */
#define USB_SCSI_KCQAC_ILLEGALMESSAGE                0x0b4900 /* Aborted Command - inappropriate/illegal message */
#define USB_SCSI_KCQAC_DATAPHASEERROR                0x0b4b00 /* Aborted Command - data phase error */
#define USB_SCSI_KCQAC_OVERLAPPEDCOMMANDSATTEMPTED   0x0b4e00 /* Aborted Command - overlapped commands attempted */
#define USB_SCSI_KCQAC_LOOPINITIALIZATION            0x0b4f00 /* Aborted Command - due to loop initialization */

#define SCSO_KCQOTHER_MISCOMPARE 0x0e1d00 /* Miscompare - during verify byte check operation */

#define USB_SCSI_STATUS_OK                       0x00 /* SCSI Status OK */
#define USB_SCSI_STATUS_CHECKCONDITION           0x02 /* SCSI Status Check condition */
#define USB_SCSI_STATUS_CONDITIONMET             0x04 /* SCSI Status Condition met */
#define USB_SCSI_STATUS_BUSY                     0x08 /* SCSI Status Busy */
#define USB_SCSI_STATUS_INTERMEDIATE             0x10 /* SCSI Status Intermediate */
#define USB_SCSI_STATUS_DATAOVERUNDERRUN         0x12 /* SCSI Status Data Under/Over Run? */
#define USB_SCSI_STATUS_INTERMEDIATECONDITIONMET 0x14 /* SCSI Status Intermediate - Condition met */
#define USB_SCSI_STATUS_RESERVATIONCONFLICT      0x18 /* SCSI Status Reservation conflict */
#define USB_SCSI_STATUS_COMMANDTERMINATED        0x22 /* SCSI Status Command terminated */
#define USB_SCSI_STATUS_QUEUEFULL                0x28 /* SCSI Status Queue (task set) full */
#define USB_SCSI_STATUS_ACAACTIVE                0x30 /* SCSI Status ACA active */
#define USB_SCSI_STATUS_TASKABORTED              0x40 /* SCSI Status Task aborted */

/* Definitions for selected SCSI commands ***********************************/

/* Inquiry */

#define USB_SCSI_CMD_INQUIRYFLAGS_EVPD 0x01            /* Bit 0: EVPD */
                                                       /* Bits 5-7: Peripheral Qualifier */
#define USB_SCSI_RESP_INQUIRYPQ_CONNECTED    0x00      /*   000: Device is connected */
#define USB_SCSI_RESP_INQUIRYPQ_NOTCONNECTED 0x20      /*   001: Device is NOT connected */
#define USB_SCSI_RESP_INQUIRYPQ_NOTCAPABLE   0x60      /*   011: LUN not supported */
                                                       /* Bits 0-4: Peripheral Device */
#define USB_SCSI_RESP_INQUIRYPD_DIRECTACCESS      0x00 /*   Direct-access block device */
#define USB_SCSI_RESP_INQUIRYPD_SEQUENTIALACCESS  0x01 /*   Sequential-access block device */
#define USB_SCSI_RESP_INQUIRYPD_PRINTER           0x02 /*   Printer device */
#define USB_SCSI_RESP_INQUIRYPD_PROCESSOR         0x03 /*   Processor device */
#define USB_SCSI_RESP_INQUIRYPD_WRONCE            0x04 /*   Write once device */
#define USB_SCSI_RESP_INQUIRYPD_CDDVD             0x05 /*   CD/DVD device */
#define USB_SCSI_RESP_INQUIRYPD_SCANNER           0x06 /*   Scanner device (obsolete) */
#define USB_SCSI_RESP_INQUIRYPD_OPTICAL           0x07 /*   Optical memory device */
#define USB_SCSI_RESP_INQUIRYPD_MEDIUMCHANGER     0x08 /*   Medium changer device (Jukebox) */
#define USB_SCSI_RESP_INQUIRYPD_COMMUNICATIONS    0x09 /*   Communications device (obsolete) */
#define USB_SCSI_RESP_INQUIRYPD_STORAGEARRAY      0x0c /*   Storage array controller device */
#define USB_SCSI_RESP_INQUIRYPD_ENCLOSURESERVICES 0x0d /*   Enclosure services device */
#define USB_SCSI_RESP_INQUIRYPD_RBC               0x0e /*   Simplified direct-access device */
#define USB_SCSI_RESP_INQUIRYPD_OCRW              0x0f /*   Optical reader/writer device */
#define USB_SCSI_RESP_INQUIRYPD_BCC               0x10 /*   Bridge controller commands */
#define USB_SCSI_RESP_INQUIRYPD_OSD               0x11 /*   Object-based storage device */
#define USB_SCSI_RESP_INQUIRYPD_ADC               0x12 /*   Automation/drive interface */
#define USB_SCSI_RESP_INQUIRYPD_WKLU              0x1e /*   Well-known logical unit */
#define USB_SCSI_RESP_INQUIRYPD_UNKNOWN           0x1f /*   Direct-access block device */

#define USB_SCSI_RESP_INQUIRYFLAGS1_RMB     0x80 /* Bit 7: RMB */
#define USB_SCSI_RESP_INQUIRYFLAGS2_NORMACA 0x20 /* Bit 5: NormACA */
#define USB_SCSI_RESP_INQUIRYFLAGS2_HISUP   0x10 /* Bit 4: HiSup */
#define USB_SCSI_RESP_INQUIRYFLAGS2_FMTMASK 0x0f /* Bits 0-3: Response data format */

#define USB_SCSI_RESP_INQUIRYFLAGS3_SCCS     0x80 /* Bit 8: SCCS */
#define USB_SCSI_RESP_INQUIRYFLAGS3_ACC      0x40 /* Bit 7: ACC */
#define USB_SCSI_RESP_INQUIRYFLAGS3_TPGSMASK 0x30 /* Bits 4-5: TPGS */
#define USB_SCSI_RESP_INQUIRYFLAGS3_3PC      0x08 /* Bit 3: 3PC */
#define USB_SCSI_RESP_INQUIRYFLAGS3_PROTECT  0x01 /* Bit 0: Protect */

#define USB_SCSI_RESP_INQUIRYFLAGS4_BQUE    0x80 /* Bit 7: BQue */
#define USB_SCSI_RESP_INQUIRYFLAGS4_ENCSERV 0x40 /* Bit 6: EncServ */
#define USB_SCSI_RESP_INQUIRYFLAGS4_VS      0x20 /* Bit 5: VS */
#define USB_SCSI_RESP_INQUIRYFLAGS4_MULTIP  0x10 /* Bit 4: MultIP */
#define USB_SCSI_RESP_INQUIRYFLAGS4_MCHNGR  0x08 /* Bit 3: MChngr */
#define USB_SCSI_RESP_INQUIRYFLAGS4_ADDR16  0x01 /* Bit 0: Addr16 */

#define USB_SCSI_RESP_INQUIRYFLAGS5_WBUS16   0x20 /* Bit 5: WBus16 */
#define USB_SCSI_RESP_INQUIRYFLAGS5_SYNC     0x10 /* Bit 4: SYNC */
#define USB_SCSI_RESP_INQUIRYFLAGS5_LINKED   0x08 /* Bit 3: LINKED */
#define USB_SCSI_RESP_INQUIRYFLAGS5_CMDQUEUE 0x02 /* Bit 1: CmdQue */
#define USB_SCSI_RESP_INQUIRYFLAGS5_VS       0x01 /* Bit 0: VS */

#define USB_SCSI_RESP_INQUIRYFLAGS6_CLOCKINGMASK 0xc0 /* Bits 2-3: Clocking */
#define USB_SCSI_RESP_INQUIRYFLAGS6_QAS          0x02 /* Bit 1: QAS */
#define USB_SCSI_RESP_INQUIRYFLAGS6_IUS          0x01 /* Bit 0: IUS */

#define USB_SCSI_RESP_SENSEDATA_CURRENTFIXED  0x70 /* Byte 1 is always the response code */
#define USB_SCSI_RESP_SENSEDATA_DEFERREDFIXED 0x71
#define USB_SCSI_RESP_SENSEDATA_CURRENTDESC   0x72
#define USB_SCSI_RESP_SENSEDATA_DEFERREDDESC  0x73

#define USB_SCSI_RESP_SENSEDATA_RESPVALID 0x80

#define USB_SCSI_RESP_SENSEDATA_FILEMARK       0x80 /* Bit 7: FileMark */
#define USB_SCSI_RESP_SENSEDATA_EOM            0x40 /* Bit 6: EOM */
#define USB_SCSI_RESP_SENSEDATA_ILI            0x20 /* Bit 5: ILI */
#define USB_SCSI_RESP_SENSEDATA_SENSEKEYMASK   0x0f /* Bits 0-3: Sense key */
#define USB_SCSI_RESP_SENSEDATA_NOSENSE        0x00 /*   Nothing to be reported */
#define USB_SCSI_RESP_SENSEDATA_RECOVEREDERROR 0x01 /*   Successful after recovery action */
#define USB_SCSI_RESP_SENSEDATA_NOTREADY       0x02 /*   Logical unit is not accessible */
#define USB_SCSI_RESP_SENSEDATA_MEDIUMERROR    0x03 /*   Error possibly caused by flaw in medium */
#define USB_SCSI_RESP_SENSEDATA_HARDWAREERROR  0x04 /*   Non-recoverable hardware error */
#define USB_SCSI_RESP_SENSEDATA_ILLEGALREQUEST 0x05 /*   Error in received request */
#define USB_SCSI_RESP_SENSEDATA_UNITATTENTION  0x06 /*   Unit attention condition */
#define USB_SCSI_RESP_SENSEDATA_DATAPROTECT    0x07 /*   Action failed, medium protected */
#define USB_SCSI_RESP_SENSEDATA_BLANKCHECK     0x08 /*   Encountered blank media */
#define USB_SCSI_RESP_SENSEDATA_VENDORSPECIFIC 0x09 /*   Vendor specific condition */
#define USB_SCSI_RESP_SENSEDATA_ABORTEDCOMMAND 0x0b /*   Command was aborted */

#define USB_SCSI_RESP_SENSEDATA_KEYVALID 0x80 /* Sense-specific data valid */

#define USB_SCSI_CMD_MODESELECT6_PF 0x10 /* Bit 4: PF */
#define USB_SCSI_CMD_MODESELECT6_SP 0x01 /* Bit 0: SP */

#define USB_SCSI_CMD_MODESENSE6_DBD 0x08 /* Bit 3: PF */

#define USB_SCSI_CMD_MODESENSE_PCMASK       0xc0 /* Bits 6-7: Page control (PC) */
#define USB_SCSI_CMD_MODESENSE_PCCURRENT    0x00 /*   Current values */
#define USB_SCSI_CMD_MODESENSE_PCCHANGEABLE 0x40 /*   Changeable values */
#define USB_SCSI_CMD_MODESENSE_PCDEFAULT    0x80 /*   Default values */
#define USB_SCSI_CMD_MODESENSE_PCSAVED      0xc0 /*   Saved values */
#define USB_SCSI_CMD_MODESENSE_PGCODEMASK   0x3f /* Bits 0-5: Page code */

#define USB_SCSI_CMD_MODESENSE6_PCDEFAULT 0x80           /*   Default values */
                                                         /* Direct-access device page codes */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_VENDOR      0x00 /*   Vendor-specific */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_RWERROR     0x01 /*   Read/Write error recovery mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_RECONNECT   0x02 /*   Disconnect-reconnect mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_FORMATDEV   0x03 /*   Format device mode page (obsolete) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_RIGID       0x04 /*   Rigid disk geometry mode page (obsolete) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_FLEXIBLE    0x05 /*   Flexible disk geometry mode page (obsolete) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_VERIFY      0x07 /*   Verify error recovery mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_CACHING     0x08 /*   Caching mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_CONTROL     0x0a /*   Control mode page (0x0a/0x00) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_CONTROLEXT  0x0a /*   Control extension mode page (0x0a/0x01) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_MEDIUMTYPES 0x0b /*   Medum types supported mode page (obsolete) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_NP          0x0c /*   Notch and partition mode page (obsolete) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_XOR         0x10 /*   XOR control mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_ES          0x14 /*   Enclosure services mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_PSLUN       0x18 /*   Protocol-specific LUN mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_PSPORT      0x19 /*   Protocol-specific port mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_POWER       0x1a /*   Power condition mode page */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_IE          0x1c /*   Informational exceptions control mode page (0x1c/0x00) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_BC          0x1c /*   Background control mode page (0x1c/0x01) */
#define USB_SCSI_RESP_MODESENSE_PGCCODE_RETURNALL   0x3f /*   Return all mode pages */
                                                         /* Direct-access caching mode page */
#define USB_SCSI_RESP_CACHINGMODEPG_PS   0x80            /*   Byte 0, Bit 7: PS */
#define USB_SCSI_RESP_CACHINGMODEPG_SPF  0x60            /*   Byte 0, Bit 6: SPF */
#define USB_SCSI_RESP_CACHINGMODEPG_IC   0x80            /*   Byte 2, Bit 7: IC */
#define USB_SCSI_RESP_CACHINGMODEPG_ABPF 0x40            /*   Byte 2, Bit 6: ABPF */
#define USB_SCSI_RESP_CACHINGMODEPG_CAP  0x20            /*   Byte 2, Bit 5: CAP */
#define USB_SCSI_RESP_CACHINGMODEPG_DISC 0x10            /*   Byte 2, Bit 4: DISC */
#define USB_SCSI_RESP_CACHINGMODEPG_SIZE 0x08            /*   Byte 2, Bit 3: SIZE */
#define USB_SCSI_RESP_CACHINGMODEPG_WCE  0x04            /*   Byte 2, Bit 2: Write cache enable (WCE) */
#define USB_SCSI_RESP_CACHINGMODEPG_MF   0x02            /*   Byte 2, Bit 1: MF */
#define USB_SCSI_RESP_CACHINGMODEPG_RCD  0x01            /*   Byte 2, Bit 0: Read cache disable (RCD) */

#define USB_SCSI_RESP_MODEPARMHDR_DAPARM_WP     0x80 /* Bit 7: WP (Direct-access block devices only) */
#define USB_SCSI_RESP_MODEPARMHDR_DAPARM_DBPFUA 0x10 /* Bit 4: DBOFUA (Direct-access block devices only) */

#define USB_SCSI_RESP_PAGEFMT_PS         0x80 /* Bit 7: PS */
#define USB_SCSI_RESP_PAGEFMT_SPF        0x40 /* Bit 6: SPF */
#define USB_SCSI_RESP_PAGEFMT_PGCODEMASK 0x3f /* Bits 0-5: Page code */
#define USB_SCSI_CMD_PREVENTMEDIUMREMOVAL_TRANSPORT 0x01 /* Removal prohibited from data transport */
#define USB_SCSI_CMD_PREVENTMEDIUMREMOVAL_MCHANGER  0x02 /* Removal prohibited from medium changer */
#define SCIRESP_RDFMTCAPACITIES_UNFORMATED 0x01 /* Unformatted media */
#define SCIRESP_RDFMTCAPACITIES_FORMATED   0x02 /* Formatted media */
#define SCIRESP_RDFMTCAPACITIES_NOMEDIA    0x03 /* No media */
#define USB_SCSI_CMD_READ6_MSLBAMASK 0x1f
#define USB_SCSI_CMD_WRITE6_MSLBAMASK 0x1f
#define USB_SCSI_CMD_MODESELECT10_PF 0x10 /* Bit 4: PF */
#define USB_SCSI_CMD_MODESELECT10_SP 0x01 /* Bit 0: SP */
#define USB_SCSI_CMD_MODESENSE10_LLBAA 0x10 /* Bit 4: LLBAA */
#define USB_SCSI_CMD_MODESENSE10_DBD   0x08 /* Bit 3: PF */
#define USB_SCSI_CMD_READ10FLAGS_RDPROTECTMASK 0xe0
#define USB_SCSI_CMD_READ10FLAGS_DPO           0x10 /* Disable Page Out */
#define USB_SCSI_CMD_READ10FLAGS_FUA           0x08
#define USB_SCSI_CMD_READ10FLAGS_FUANV         0x02
#define USB_SCSI_CMD_WRITE10FLAGS_WRPROTECTMASK 0xe0
#define USB_SCSI_CMD_WRITE10FLAGS_DPO           0x10 /* Disable Page Out */
#define USB_SCSI_CMD_WRITE10FLAGS_FUA           0x08
#define USB_SCSI_CMD_WRITE10FLAGS_FUANV         0x02
#define USB_SCSI_CMD_VERIFY10_VRPROTECTMASK 0xe0 /* Byte 1: Bits 5-7: VRPROTECT */
#define USB_SCSI_CMD_VERIFY10_DPO           0x10 /* Byte 1: Bit 4: Disable Page Out (DPO) */
#define USB_SCSI_CMD_VERIFY10_BYTCHK        0x02 /* Byte 1: Bit 2: BytChk */
#define USB_SCSI_CMD_READ12FLAGS_RDPROTECTMASK 0xe0
#define USB_SCSI_CMD_READ12FLAGS_DPO           0x10 /* Disable Page Out */
#define USB_SCSI_CMD_READ12FLAGS_FUA           0x08
#define USB_SCSI_CMD_READ12FLAGS_FUANV         0x02
#define USB_SCSI_CMD_WRITE12FLAGS_WRPROTECTMASK 0xe0
#define USB_SCSI_CMD_WRITE12FLAGS_DPO           0x10 /* Disable Page Out */
#define USB_SCSI_CMD_WRITE12FLAGS_FUA           0x08
#define USB_SCSI_CMD_WRITE12FLAGS_FUANV         0x02
#define USB_SCSI_CMD_VERIFY12_VRPROTECTMASK 0xe0 /* Byte 1: Bits 5-7: VRPROTECT */
#define USB_SCSI_CMD_VERIFY12_DPO           0x10 /* Byte 1: Bit 4: Disable Page Out (DPO) */
#define USB_SCSI_CMD_VERIFY12_BYTCHK        0x02 /* Byte 1: Bit 2: BytChk */
#define USB_SCSI_CMD_TESTUNITREADY            0x00
#define USB_SCSI_CMD_REZEROUNIT               0x01
#define USB_SCSI_CMD_REQUESTSENSE             0x03
#define USB_SCSI_CMD_FORMAT_UNIT              0x04
#define USB_SCSI_CMD_REASSIGNBLOCKS           0x07
#define USB_SCSI_CMD_READ6                    0x08
#define USB_SCSI_CMD_WRITE6                   0x0a
#define USB_SCSI_CMD_SEEK6                    0x0b
#define USB_SCSI_CMD_SPACE6                   0x11
#define USB_SCSI_CMD_INQUIRY                  0x12
#define USB_SCSI_CMD_MODESELECT6              0x15
#define USB_SCSI_CMD_RESERVE6                 0x16
#define USB_SCSI_CMD_RELEASE6                 0x17
#define USB_SCSI_CMD_COPY                     0x18
#define USB_SCSI_CMD_MODESENSE6               0x1a
#define USB_SCSI_CMD_STARTSTOPUNIT            0x1b
#define USB_SCSI_CMD_RECEIVEDIAGNOSTICRESULTS 0x1c
#define USB_SCSI_CMD_SENDDIAGNOSTIC           0x1d
#define USB_SCSI_CMD_PREVENTMEDIAREMOVAL      0x1e
#define USB_SCSI_CMD_READFORMATCAPACITIES     0x23
#define USB_SCSI_CMD_READCAPACITY10           0x25
#define USB_SCSI_CMD_READ10                   0x28
#define USB_SCSI_CMD_WRITE10                  0x2a
#define USB_SCSI_CMD_SEEK10                   0x2b
#define USB_SCSI_CMD_WRITEANDVERIFY           0x2e
#define USB_SCSI_CMD_VERIFY10                 0x2f
#define USB_SCSI_CMD_SEARCHDATAHIGH           0x30
#define USB_SCSI_CMD_SEARCHDATAEQUAL          0x31
#define USB_SCSI_CMD_SEARCHDATALOW            0x32
#define USB_SCSI_CMD_SETLIMITS10              0x33
#define USB_SCSI_CMD_PREFETCH10               0x34
#define USB_SCSI_CMD_SYNCHCACHE10             0x35
#define USB_SCSI_CMD_LOCKCACHE                0x36
#define USB_SCSI_CMD_READDEFECTDATA10         0x37
#define USB_SCSI_CMD_COMPARE                  0x39
#define USB_SCSI_CMD_COPYANDVERIFY            0x3a
#define USB_SCSI_CMD_WRITEBUFFER              0x3b
#define USB_SCSI_CMD_READBUFFER               0x3c
#define USB_SCSI_CMD_READLONG10               0x3e
#define USB_SCSI_CMD_WRITELONG10              0x3f
#define USB_SCSI_CMD_CHANGEDEFINITION         0x40
#define USB_SCSI_CMD_WRITESAME10              0x41
#define USB_SCSI_CMD_LOGSELECT                0x4c
#define USB_SCSI_CMD_LOGSENSE                 0x4d
#define USB_SCSI_CMD_XDWRITE10                0x50
#define USB_SCSI_CMD_XPWRITE10                0x51
#define USB_SCSI_CMD_XDREAD10                 0x52
#define USB_SCSI_CMD_MODESELECT10             0x55
#define USB_SCSI_CMD_RESERVE10                0x56
#define USB_SCSI_CMD_RELEASE10                0x57
#define USB_SCSI_CMD_MODESENSE10              0x5a
#define USB_SCSI_CMD_PERSISTENTRESERVEIN      0x5e
#define USB_SCSI_CMD_PERSISTENTRESERVEOUT     0x5f
#define USB_SCSI_CMD_32                       0x7f
#define USB_SCSI_CMD_XDWRITEEXTENDED          0x80
#define USB_SCSI_CMD_REBUILD                  0x82
#define USB_SCSI_CMD_REGENERATE               0x82
#define USB_SCSI_CMD_EXTENDEDCOPY             0x83
#define USB_SCSI_CMD_COPYRESULTS              0x84
#define USB_SCSI_CMD_ACCESSCONTROLIN          0x86
#define USB_SCSI_CMD_ACCESSCONTROLOUT         0x87
#define USB_SCSI_CMD_READ16                   0x88
#define USB_SCSI_CMD_WRITE16                  0x8a
#define USB_SCSI_CMD_READATTRIBUTE            0x8c
#define USB_SCSI_CMD_WRITEATTRIBUTE           0x8d
#define USB_SCSI_CMD_WRITEANDVERIFY16         0x8e
#define USB_SCSI_CMD_PREFETCH16               0x90
#define USB_SCSI_CMD_SYNCHCACHE16             0x91
#define USB_SCSI_CMD_LOCKUNLOCKACACHE         0x92
#define USB_SCSI_CMD_WRITESAME16              0x93
#define USB_SCSI_CMD_READCAPACITY16           0x9e
#define USB_SCSI_CMD_READLONG16               0x9e
#define USB_SCSI_CMD_WRITELONG106             0x9f
#define USB_SCSI_CMD_REPORTLUNS               0xa0
#define USB_SCSI_CMD_MAINTENANCEIN            0xa3
#define USB_SCSI_CMD_MAINTENANCEOUT           0xa4
#define USB_SCSI_CMD_MOVEMEDIUM               0xa5
#define USB_SCSI_CMD_MOVEMEDIUMATTACHED       0xa7
#define USB_SCSI_CMD_READ12                   0xa8
#define USB_SCSI_CMD_WRITE12                  0xaa
#define USB_SCSI_CMD_READMEDIASERIALNUMBER    0xab
#define USB_SCSI_CMD_WRITEANDVERIFY12         0xae
#define USB_SCSI_CMD_VERIFY12                 0xaf
#define USB_SCSI_CMD_SETLIMITS12              0xb3
#define USB_SCSI_CMD_READELEMENTSTATUS        0xb4
#define USB_SCSI_CMD_READDEFECTDATA12         0xb7
#define USB_SCSI_CMD_REDUNDANCYGROUPIN        0xba
#define USB_SCSI_CMD_REDUNDANCYGROUPOUT       0xbb
#define USB_SCSI_CMD_SPAREIN                  0xbc
#define USB_SCSI_CMD_SPAREOUT                 0xbd
#define USB_SCSI_CMD_VOLUMESETIN              0xbe
#define USB_SCSI_CMD_VOLUMESETOUT             0xbf

#define USB_SCSI_CMD_TESTUNITREADY_SIZEOF 6

struct scsicmd_requestsense_s {
    uint8_t opcode;      /* 0: 0x03 */
    uint8_t flags;       /* 1: See USB_SCSI_CMD_REQUESTSENSE_FLAGS_* */
    uint8_t reserved[2]; /* 2-3: Reserved */
    uint8_t alloclen;    /* 4: Allocation length */
    uint8_t control;     /* 5: Control */
};
#define USB_SCSI_CMD_REQUESTSENSE_SIZEOF   6
#define USB_SCSI_CMD_REQUESTSENSE_MSSIZEOF 12 /* MS-Windows REQUEST SENSE with cbw->cdblen == 12 */

struct scsiresp_fixedsensedata_s {
    uint8_t code;       /* 0: Response code See  USB_SCSI_RESP_SENSEDATA_*FIXED defns */
    uint8_t obsolete;   /* 1: */
    uint8_t flags;      /* 2: See USB_SCSI_RESP_SENSEDATA_* definitions */
    uint8_t info[4];    /* 3-6: Information */
    uint8_t len;        /* 7: Additional length */
    uint8_t cmdinfo[4]; /* 8-11: Command-specific information */
    uint8_t code2;      /* 12: Additional sense code */
    uint8_t qual2;      /* 13: Additional sense code qualifier */
    uint8_t fru;        /* 14: Field replacement unit code */
    uint8_t key[3];     /* 15-17: Sense key specific */
                        /* 18-: Additional bytes may follow */
};
#define USB_SCSI_RESP_FIXEDSENSEDATA_SIZEOF 18 /* Minimum size */

struct scscicmd_inquiry_s {
    uint8_t opcode;      /* 0: 0x12 */
    uint8_t flags;       /* 1: See USB_SCSI_CMD_INQUIRY_FLAGS_* */
    uint8_t pagecode;    /* 2: Page code */
    uint8_t alloclen[2]; /* 3-4: Allocation length */
    uint8_t control;     /* 5: Control */
};
#define USB_SCSI_CMD_INQUIRY_SIZEOF 6

struct scsiresp_inquiry_s {
    /* Mandatory */

    uint8_t qualtype;      /* 0: Bits 5-7: Peripheral qualifier; Bits 0-4: Peripheral device type */
    uint8_t flags1;        /* 1: See USB_SCSI_RESP_INQUIRY_FLAGS1_* */
    uint8_t version;       /* 2: Version */
    uint8_t flags2;        /* 3: See USB_SCSI_RESP_INQUIRY_FLAGS2_* */
    uint8_t len;           /* 4: Additional length */
    uint8_t flags3;        /* 5: See USB_SCSI_RESP_INQUIRY_FLAGS3_* */
    uint8_t flags4;        /* 6: See USB_SCSI_RESP_INQUIRY_FLAGS4_* */
    uint8_t flags5;        /* 7: See USB_SCSI_RESP_INQUIRY_FLAGS5_* */
    uint8_t vendorid[8];   /* 8-15: T10 Vendor Identification */
    uint8_t productid[16]; /* 16-31: Product Identification */
    uint8_t revision[4];   /* 32-35: Product Revision Level */

    /* Optional */

    uint8_t vendor[20];    /* 36-55: Vendor specific */
    uint8_t flags6;        /* 56: See USB_SCSI_RESP_INQUIRY_FLAGS6_* */
    uint8_t reserved1;     /* 57: Reserved */
    uint8_t version1[2];   /* 58-59: Version Descriptor 1 */
    uint8_t version2[2];   /* 60-61: Version Descriptor 2 */
    uint8_t version3[2];   /* 62-63: Version Descriptor 3 */
    uint8_t version4[2];   /* 64-65: Version Descriptor 4 */
    uint8_t version5[2];   /* 66-67: Version Descriptor 5 */
    uint8_t version6[2];   /* 68-69: Version Descriptor 6 */
    uint8_t version7[2];   /* 70-71: Version Descriptor 7 */
    uint8_t version8[2];   /* 72-73: Version Descriptor 8 */
    uint8_t reserved2[22]; /* 74-95: Reserved */
                           /* 96-: Vendor-specific parameters may follow */
};
#define USB_SCSI_RESP_INQUIRY_SIZEOF 36 /* Minimum size */

struct scsicmd_modeselect6_s {
    uint8_t opcode;      /* 0x15 */
    uint8_t flags;       /* 1: See USB_SCSI_CMD_MODESELECT6_FLAGS_* */
    uint8_t reserved[2]; /* 2-3: Reserved */
    uint8_t plen;        /* 4: Parameter list length */
    uint8_t control;     /* 5: Control */
};
#define USB_SCSI_CMD_MODESELECT6_SIZEOF 6

struct scsicmd_modesense6_s {
    uint8_t opcode;    /* 0x1a */
    uint8_t flags;     /* 1: See USB_SCSI_CMD_MODESENSE6_FLAGS_* */
    uint8_t pcpgcode;  /* 2: Bits 6-7: PC, bits 0-5: page code */
    uint8_t subpgcode; /* 3: subpage code */
    uint8_t alloclen;  /* 4: Allocation length */
    uint8_t control;   /* 5: Control */
};
#define USB_SCSI_CMD_MODESENSE6_SIZEOF 6

struct scsiresp_modeparameterhdr6_s {
    uint8_t mdlen; /* 0: Mode data length */
    uint8_t type;  /* 1: Medium type */
    uint8_t param; /* 2: Device-specific parameter */
    uint8_t bdlen; /* 3: Block descriptor length */
};
#define USB_SCSI_RESP_MODEPARAMETERHDR6_SIZEOF 4

struct scsiresp_blockdesc_s {
    uint8_t density;    /* 0: density code */
    uint8_t nblocks[3]; /* 1-3: Number of blocks */
    uint8_t reserved;   /* 4: reserved */
    uint8_t blklen[3];  /* 5-7: Block len */
};
#define USB_SCSI_RESP_BLOCKDESC_SIZEOF 8

struct scsiresp_pageformat_s {
    uint8_t pgcode;   /* 0: See USB_SCSI_RESP_PAGEFMT_* definitions */
    uint8_t pglen;    /* 1: Page length (n-1) */
    uint8_t parms[1]; /* 2-n: Mode parameters */
};

struct scsiresp_subpageformat_s {
    uint8_t pgcode;    /* 0: See USB_SCSI_RESP_PAGEFMT_* definitions */
    uint8_t subpgcode; /* 1: sub-page code */
    uint8_t pglen[2];  /* 2-3: Page length (n-3) */
    uint8_t parms[1];  /* 4-n: Mode parameters */
};

struct scsiresp_cachingmodepage_s {
    uint8_t pgcode;      /* 0: Bit 7: PS; Bit 6: SPF, Bits 0-5: page code == 8 */
    uint8_t len;         /* 1: Page length (18) */
    uint8_t flags1;      /* 2: See USB_SCSI_RESP_CACHINGMODEPG_* definitions */
    uint8_t priority;    /* 3: Bits 4-7: Demand read retention priority; Bits 0-3: Write retention priority */
    uint8_t dpflen[2];   /* 4-5: Disable prefetch transfer length */
    uint8_t minpf[2];    /* 6-7: Minimum pre-fetch */
    uint8_t maxpf[2];    /* 8-9: Maximum pre-fetch */
    uint8_t maxpfc[2];   /* 10-11: Maximum pref-fetch ceiling */
    uint8_t flags2;      /* 12: See USB_SCSI_RESP_CACHINGMODEPG_* definitions */
    uint8_t nsegments;   /* 13: Number of cache segments */
    uint8_t segsize[2];  /* 14-15: Cache segment size */
    uint8_t reserved;    /* 16: Reserved */
    uint8_t obsolete[3]; /* 17-19: Obsolete */
};

/* Format structures for selected SCSI block commands */

struct scsicmd_read6_s {
    uint8_t opcode;   /* 0: 0x08 */
    uint8_t mslba;    /* 1: Bits 5-7: reserved; Bits 0-6: MS Logical Block Address (LBA) */
    uint8_t lslba[2]; /* 2-3: LS Logical Block Address (LBA) */
    uint8_t xfrlen;   /* 4: Transfer length (in contiguous logical blocks) */
    uint8_t control;  /* 5: Control */
};
#define USB_SCSI_CMD_READ6_SIZEOF 6

struct scsicmd_write6_s {
    uint8_t opcode;   /* 0: 0x0a */
    uint8_t mslba;    /* 1: Bits 5-7: reserved; Bits 0-6: MS Logical Block Address (LBA) */
    uint8_t lslba[2]; /* 2-3: LS Logical Block Address (LBA) */
    uint8_t xfrlen;   /* 4: Transfer length (in contiguous logical blocks) */
    uint8_t control;  /* 5: Control */
};
#define USB_SCSI_CMD_WRITE6_SIZEOF 6

struct scsicmd_startstopunit_s {
    uint8_t opcode;   /* 0: 0x1b */
    uint8_t immed;    /* 1: Bits 2-7: Reserved, Bit 0: Immed */
    uint8_t reserved; /* 2: reserved */
    uint8_t pcm;      /* 3: Bits 4-7: Reserved, Bits 0-3: Power condition modifier */
    uint8_t pc;       /* 4: Bits 4-7: Power condition, Bit 2: NO_FLUSH, Bit 1: LOEJ, Bit 0: START */
    uint8_t control;  /* 5: Control */
};
#define USB_SCSI_CMD_STARTSTOPUNIT_SIZEOF 6

struct scsicmd_preventmediumremoval_s {
    uint8_t opcode;      /* 0: 0x1e */
    uint8_t reserved[3]; /* 1-3: Reserved */
    uint8_t prevent;     /* 4: Bits 2-7: Reserved, Bits 0:1: prevent */
    uint8_t control;     /* 5: Control */
};
#define USB_SCSI_CMD_PREVENTMEDIUMREMOVAL_SIZEOF 6

struct scsicmd_readformatcapcacities_s {
    uint8_t opcode;      /* 0: 0x23 */
    uint8_t reserved[6]; /* 1-6: Reserved */
    uint8_t alloclen[2]; /* 7-8: Allocation length */
    uint8_t control;     /* 9: Control */
};
#define USB_SCSI_CMD_READFORMATCAPACITIES_SIZEOF 10

struct scsiresp_readformatcapacities_s {
    /* Current capacity header */

    uint8_t reserved[3]; /* 0-2: Reserved */
    uint8_t listlen;     /* 3: Capacity list length */

    /* Current/Maximum Capacity Descriptor (actually a separate structure) */

    uint8_t nblocks[4];  /* 4-7: Number of blocks */
    uint8_t type;        /* 8: Bits 2-7: Reserved, Bits 0-1: Descriptor type */
    uint8_t blocklen[3]; /* 9-11: Block length */
};
#define USB_SCSI_RESP_READFORMATCAPACITIES_SIZEOF 12
#define USB_SCSI_RESP_CURRCAPACITYDESC_SIZEOF     8

struct scsiresp_formattedcapacitydesc_s {
    uint8_t nblocks[4]; /* 0-3: Number of blocks */
    uint8_t type;       /* 4: Bits 2-7: Type, bits 0-1, reserved */
    uint8_t param[3];   /* 5-7: Type dependent parameter */
};
#define USB_SCSI_RESP_FORMATTEDCAPACITYDESC_SIZEOF 8

struct scsicmd_readcapacity10_s {
    uint8_t opcode;       /* 0: 0x25 */
    uint8_t reserved1;    /* 1: Bits 1-7: Reserved, Bit 0: Obsolete */
    uint8_t lba[4];       /* 2-5: Logical block address (LBA) */
    uint8_t reserved2[2]; /* 6-7: Reserved */
    uint8_t pmi;          /* 8: Bits 1-7 Reserved; Bit 0: PMI */
    uint8_t control;      /* 9: Control */
};
#define USB_SCSI_CMD_READCAPACITY10_SIZEOF 10

struct scsiresp_readcapacity10_s {
    uint8_t lba[4];    /* 0-3: Returned logical block address (LBA) */
    uint8_t blklen[4]; /* 4-7: Logical block length (in bytes) */
};
#define USB_SCSI_RESP_READCAPACITY10_SIZEOF 8

struct scsicmd_read10_s {
    uint8_t opcode;    /* 0: 0x28 */
    uint8_t flags;     /* 1: See USB_SCSI_CMD_READ10FLAGS_* */
    uint8_t lba[4];    /* 2-5: Logical Block Address (LBA) */
    uint8_t groupno;   /* 6: Bits 5-7: reserved; Bits 0-6: group number */
    uint8_t xfrlen[2]; /* 7-8: Transfer length (in contiguous logical blocks) */
    uint8_t control;   /* 9: Control */
};
#define USB_SCSI_CMD_READ10_SIZEOF 10

struct scsicmd_write10_s {
    uint8_t opcode;    /* 0: 0x2a */
    uint8_t flags;     /* 1: See USB_SCSI_CMD_WRITE10FLAGS_* */
    uint8_t lba[4];    /* 2-5: Logical Block Address (LBA) */
    uint8_t groupno;   /* 6: Bits 5-7: reserved; Bits 0-6: group number */
    uint8_t xfrlen[2]; /* 7-8: Transfer length (in contiguous logical blocks) */
    uint8_t control;   /* 9: Control */
};
#define USB_SCSI_CMD_WRITE10_SIZEOF 10

struct scsicmd_verify10_s {
    uint8_t opcode;  /* 0: 0x2f */
    uint8_t flags;   /* 1: See USB_SCSI_CMD_VERIFY10_* definitions */
    uint8_t lba[4];  /* 2-5: Logical block address (LBA) */
    uint8_t groupno; /* 6: Bit 7: restricted; Bits 5-6: Reserved, Bits 0-4: Group number */
    uint8_t len[2];  /* 7-8: Verification length (in blocks) */
    uint8_t control; /* 9: Control */
};
#define USB_SCSI_CMD_VERIFY10_SIZEOF 10

struct scsicmd_synchronizecache10_s {
    uint8_t opcode;  /* 0: 0x35 */
    uint8_t flags;   /* 1: See USB_SCSI_CMD_SYNCHRONIZECACHE10_* definitions */
    uint8_t lba[4];  /* 2-5: Logical block address (LBA) */
    uint8_t groupno; /* 6: Bit 7: restricted; Bits 5-6: Reserved, Bits 0-4: Group number */
    uint8_t len[2];  /* 7-8: Number of logical blocks */
    uint8_t control; /* 9: Control */
};
#define USB_SCSI_CMD_SYNCHRONIZECACHE10_SIZEOF 10

struct scsicmd_modeselect10_s {
    uint8_t opcode;      /* 0: 0x55 */
    uint8_t flags;       /* 1: See USB_SCSI_CMD_MODESELECT10_FLAGS_* */
    uint8_t reserved[5]; /* 2-6: Reserved */
    uint8_t parmlen[2];  /* 7-8: Parameter list length */
    uint8_t control;     /* 9: Control */
};
#define USB_SCSI_CMD_MODESELECT10_SIZEOF 10

struct scsiresp_modeparameterhdr10_s {
    uint8_t mdlen[2];    /* 0-1: Mode data length */
    uint8_t type;        /* 2: Medium type */
    uint8_t param;       /* 3: Device-specific parameter */
    uint8_t reserved[2]; /* 4-5: reserved */
    uint8_t bdlen[2];    /* 6-7: Block descriptor length */
};
#define USB_SCSI_RESP_MODEPARAMETERHDR10_SIZEOF 8

struct scsicmd_modesense10_s {
    uint8_t opcode;      /* O: 0x5a */
    uint8_t flags;       /* 1: See USB_SCSI_CMD_MODESENSE10_FLAGS_* */
    uint8_t pcpgcode;    /* 2: Bits 6-7: PC, bits 0-5: page code */
    uint8_t subpgcode;   /* 3: subpage code */
    uint8_t reserved[3]; /* 4-6: reserved */
    uint8_t alloclen[2]; /* 7-8: Allocation length */
    uint8_t control;     /* 9: Control */
};
#define USB_SCSI_CMD_MODESENSE10_SIZEOF 10

struct scsicmd_readcapacity16_s {
    uint8_t opcode;   /* 0: 0x9e */
    uint8_t action;   /* 1: Bits 5-7: Reserved, Bits 0-4: Service action */
    uint8_t lba[8];   /* 2-9: Logical block address (LBA) */
    uint8_t len[4];   /* 10-13: Allocation length */
    uint8_t reserved; /* 14: Reserved */
    uint8_t control;  /* 15: Control */
};
#define USB_SCSI_CMD_READCAPACITY16_SIZEOF 16

struct scsicmd_read12_s {
    uint8_t opcode;    /* 0: 0xa8 */
    uint8_t flags;     /* 1: See USB_SCSI_CMD_READ12FLAGS_* */
    uint8_t lba[4];    /* 2-5: Logical Block Address (LBA) */
    uint8_t xfrlen[4]; /* 6-9: Transfer length (in contiguous logical blocks) */
    uint8_t groupno;   /* 10: Bit 7: restricted; Bits 5-6: reserved; Bits 0-6: group number */
    uint8_t control;   /* 11: Control */
};
#define USB_SCSI_CMD_READ12_SIZEOF 12

struct scsicmd_write12_s {
    uint8_t opcode;    /* 0: 0xaa */
    uint8_t flags;     /* 1: See USB_SCSI_CMD_WRITE12FLAGS_* */
    uint8_t lba[4];    /* 2-5: Logical Block Address (LBA) */
    uint8_t xfrlen[4]; /* 6-9: Transfer length (in contiguous logical blocks) */
    uint8_t groupno;   /* 10: Bit 7: restricted; Bits 5-6: reserved; Bits 0-6: group number */
    uint8_t control;   /* 11: Control */
};
#define USB_SCSI_CMD_WRITE12_SIZEOF 12

struct scsicmd_verify12_s {
    uint8_t opcode;  /* 0: 0xaf */
    uint8_t flags;   /* 1: See USB_SCSI_CMD_VERIFY12_* definitions */
    uint8_t lba[4];  /* 2-5: Logical block address (LBA) */
    uint8_t len[4];  /* 6-9: Verification length */
    uint8_t groupno; /* 10: Bit 7: restricted; Bits 5-6: Reserved, Bits 0-4: Group number */
    uint8_t control; /* 11: Control */
};
#define USB_SCSI_CMD_VERIFY12_SIZEOF 12

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __INCLUDE_NUTTX_USB_SCSI_H */
