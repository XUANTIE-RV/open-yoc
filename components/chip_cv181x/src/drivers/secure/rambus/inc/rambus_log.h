/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* log.h
 *
 * Logging API
 *
 * The service provided by this interface allows the caller to output trace
 * messages. The implementation can use whatever output channel is available
 * in a specific environment.
 */



#ifndef INCLUDEGUARD_LOG_H
#define INCLUDEGUARD_LOG_H

// Driver Framework Basic Defs API
#include "basic_defs.h"

// Logging API
#include "stdio.h"           // implementation specifics


#define Log_Message           printf
#define Log_FormattedMessage  printf

// backwards compatible implementation
#define Log_FormattedMessageINFO  Log_FormattedMessage
#define Log_FormattedMessageWARN  Log_FormattedMessage
#define Log_FormattedMessageCRIT  Log_FormattedMessage


#define LOG_SEVERITY_MAX                    LOG_SEVERITY_CRIT
// #define LOG_DUMP_ENABLED
/*----------------------------------------------------------------------------
 * LOG_SEVERITY_MAX
 *
 * This preprocessor symbol is used to control the definition of three macros
 * that can be used to selectively compile three classes of log messages:
 * Informational, Warnings and Critical. Define this symbol before including
 * this header file. When absent, full logging is assumed.
 */

// set LOG_SEVERITY_MAX to one of the following values
#define LOG_SEVERITY_NO_OUTPUT  0
#define LOG_SEVERITY_CRIT       1
#define LOG_SEVERITY_CRITICAL   1
#define LOG_SEVERITY_WARN       2
#define LOG_SEVERITY_WARNING    2
#define LOG_SEVERITY_INFO       3

/*----------------------------------------------------------------------------
 * LOG_CRIT_ENABLED
 * LOG_WARN_ENABLED
 * LOG_INFO_ENABLED
 *
 * This preprocessor symbols can be used to test if a specific class of log
 * message has been enabled by the LOG_SEVERITY_MAX selection.
 *
 * Example usage:
 *
 * #ifdef LOG_SEVERITY_INFO
 * // dump command descriptor details to log
 * #endif
 */

#if LOG_SEVERITY_MAX >= LOG_SEVERITY_CRITICAL
#define LOG_CRIT_ENABLED
#endif

#if LOG_SEVERITY_MAX >= LOG_SEVERITY_WARNING
#define LOG_WARN_ENABLED
#endif

#if LOG_SEVERITY_MAX >= LOG_SEVERITY_INFO
#define LOG_INFO_ENABLED
#endif


/*----------------------------------------------------------------------------
 * Log_Message
 *
 * This function adds a simple constant message to the log buffer.
 *
 * Message_p
 *     Pointer to the zero-terminated log message. The message must be
 *     complete and terminated with a newline character ("\n"). This avoids
 *     blending of partial messages.
 *
 * Return Value
 *     None.
 */
#ifndef Log_Message
void
Log_Message(
        const char * szMessage_p);
#endif



/*----------------------------------------------------------------------------
 * LOG_CRIT
 * LOG_WARN
 * LOG_INFO
 *
 * These three helper macros can be used to conditionally compile code that
 * outputs log messages and make the actual log line more compact.
 * Each macro is enabled when the class of messages is activated with the
 * LOG_SEVERITY_MAX setting.
 *
 * Example usage:
 *
 * LOG_INFO("MyFunc: selected mode %u (%s)\n", mode, Mode2Str[mode]);
 *
 * LOG_INFO(
 *      "MyFunc: "
 *      "selected mode %u (%s)\n",
 *      mode,
 *      Mode2Str[mode]);
 *
 * LOG_WARN("MyFunc: Unexpected return value %d\n", res);
 */

#ifdef LOG_CRIT_ENABLED
#define LOG_CRIT Log_FormattedMessageCRIT
#else
#define LOG_CRIT(...)
#endif

#ifdef LOG_WARN_ENABLED
#define LOG_WARN Log_FormattedMessageWARN
#else
#define LOG_WARN(...)
#endif

#ifdef LOG_INFO_ENABLED
#define LOG_INFO Log_FormattedMessageINFO
#else
#define LOG_INFO(...)
#endif


/*----------------------------------------------------------------------------
 * Log_HexDump
 *
 * This function logs Hex Dump of a Buffer
 *
 * szPrefix
 *     Prefix to be printed on every row.
 *
 * PrintOffset
 *     Offset value that is printed at the start of every row. Can be used
 *     when the byte printed are located at some offset in another buffer.
 *
 * Buffer_p
 *     Pointer to the start of the array of bytes to hex dump.
 *
 * ByteCount
 *     Number of bytes to include in the hex dump from Buffer_p.
 *
 * Return Value
 *     None.
 */
#ifdef LOG_DUMP_ENABLED
void
Log_HexDump(
        const char * szPrefix_p,
        const unsigned int PrintOffset,
        const uint8_t * Buffer_p,
        const unsigned int ByteCount);
#else
#define Log_HexDump(...)
#endif

#define TRACE() LOG_INFO(" %s, %d\n", __FUNCTION__, __LINE__)
#define TRACE_N(n) LOG_INFO(" %s, %d, %d\n", __FUNCTION__, __LINE__, n)
#define TRACE_ERR(e) LOG_CRIT("err : %s, %d, 0x%08x\n", __FUNCTION__, __LINE__, e)

#endif

