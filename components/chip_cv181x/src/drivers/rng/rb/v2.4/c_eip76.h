/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/* c_eip76.h
 *
 * Default configuration parameters for the EIP76 Driver Library.
 */

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Top-level configuration
//#include "cs_eip76.h"
#include "soc.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#define RB_EIP76_BASE                RB_EIP150B_BASE
#define RB_EIP76_SIZE                0x3FFC

#define EIP76_IMPFRO_ENABLED_MASK       0x00000F

// Startup cycles in the EIP-76 HW clocks (clk input) that the device
// should wait before the entropy is gathered
// This field determines the number of samples (between 2^8 and 2^24) taken
// to gather entropy from the FROs during startup. If the written value of
// this field is zero, the number of samples is 2^24, otherwise the number
// of samples equals the written value times 2^8.
#ifndef EIP76_STARTUP_CYCLES
#define EIP76_STARTUP_CYCLES 0
#endif

// Number of bits to accumulate in the TRNG_RUN_CNT:run_test_count field
// before the Run/Poker test can be considered finished and the test results
// can be read
#ifndef EIP76_TEST_FINISHED_RUN_COUNT
#define EIP76_TEST_FINISHED_RUN_COUNT 20020
#endif

// FRO enable mask that determines how many FRO"s will be activated during
// the TRNG initialization. Default - 24 FRO's are enabled.
// Note: Some EIP-76 HW configurations support 8 FRO's only
#ifndef EIP76_FRO_ENABLED_MASK
#define EIP76_FRO_ENABLED_MASK 0xFFFFFF
#endif

// Sample divider
// This field directly controls the number of clock input cycles between
// samples taken from the FROs. The default value 0 indicates that samples are
// taken every "clk" cycle, maximum value 15 (decimal) takes one sample every
// 16 "clk" cycles. This field must be set to a value such that the slowest
// FRO (even under worst-case conditions) has a cycle time less than twice
// the sample period. As delivered, the default configuration of the FROs
// allows this field to remain 0.
#ifndef EIP76_SAMPLE_DIVIDER
#define EIP76_SAMPLE_DIVIDER 0
#endif

// Alarm threshold
// Alarm detection threshold for the repeating pattern detectors on each FRO.
// A FRO "alarm event" is declared when a repeating pattern (of up to four
// samples length) is detected continuously for the number of samples defined
// by this parameter. Reset value 255 (decimal) should keep the number of
// "alarm events" to a manageable level.
#ifndef EIP76_ALARM_THRESHOLD
#define EIP76_ALARM_THRESHOLD 255
#endif

// Shutdown threshold
// Threshold setting for generating the "shutdown_oflo" interrupt, which is
// activated when the "shutdown_count" value exceeds
// the threshold value set here.
// #ifndef EIP76_SHUTDOWN_THRESHOLD
// #define EIP76_SHUTDOWN_THRESHOLD 0
// #endif
/* note same as sim */
#define EIP76_SHUTDOWN_THRESHOLD 0x6

// Shutdown fatal (1 - yes, 0 - no)
// Consider the "shutdown_oflo" interrupt as a fatal error requiring taking
// the complete TRNG engine to the reset state
#ifndef EIP76_SHUTDOWN_FATAL
#define EIP76_SHUTDOWN_FATAL 0
#endif

// Secure Read Mode timeout
// When 0 the mode is disabled, 15 is the maximum value
// Timeout is (EIP76_READ_TIMEOUT + 1) * 16 EIP-76 TRNG input clocks (clk)
#define EIP76_READ_TIMEOUT      0

#ifndef EIP76_NOISE_BLOCKS
#define EIP76_NOISE_BLOCKS 12
#endif

#ifndef EIP76_SAMPLE_CYCLE
#define EIP76_SAMPLE_CYCLE 32
#endif

// Must be 0 for HW 2.0.2
#ifndef EIP76_SAMPLE_SCALE
#define EIP76_SAMPLE_SCALE 0
#endif


/*
 * Footprint optimization configuration parameters
 */

// Post-processor type:
// 0 - No Post Processor (64-bit random number)
// 5 - SP 800-90 AES-256 Post Processor (128-bit random number)
// 7 - SP 800-90 with BC_DF Post Processor
#define EIP76_POST_PROCESSOR_NONE       0x00
#define EIP76_POST_PROCESSOR_SP800_90   0x05
#define EIP76_POST_PROCESSOR_BC_DF      0x07

#define EIP76_POST_PROCESSOR_TYPE       EIP76_POST_PROCESSOR_BC_DF

// Strict argument checking for the input parameters
// If required then define this parameter in the top-level configuration
#define EIP76_STRICT_ARGS

// Finite State Machine that can be used for verifying that the Driver Library
// API is called in a correct order
//#define EIP76_DEBUG_FSM

// Disable Monobit test support
//#define EIP76_MONOBIT_DISABLE

// Enable an alternative Poker test ready check mechanism
//#define EIP76_CONT_POKER_TEST_READY_CDS

// If EIP76_BLOCKS_THRESHOLD_OPTION is enabled then set the number of blocks
// used for TRNG mode (Not for DRBG)
#ifdef EIP76_BLOCKS_THRESHOLD_OPTION
#define EIP76_BLOCKS_THRESHOLD_VALUE   4
#endif

// #ifdef EIP76_FIPS_80090B2_SUPPORT
// // Sample cycles when No Whitening option is used (HW 2.3 and later)
// #define EIP76_SAMPLE_CYCLES          EIP76_SAMPLE_CYCLE*2
// #else
// #define EIP76_SAMPLE_CYCLES          EIP76_SAMPLE_CYCLE
/* NOTE same as sim config */
#define EIP76_SAMPLE_CYCLES          (EIP76_SAMPLE_CYCLE * 5)
// #endif


/* from adapter */
#define ADAPTER_HRNG_RESEED_BUSYWAIT_SLEEP_MS   1
#define ADAPTER_HRNG_RESEED_BUSYWAIT_MAXLOOP    1000
#define EIP76_FATAL_EVENTS (EIP76_STUCK_OUT_EVENT| \
                            EIP76_NOISE_FAIL_EVENT)
#define ADAPTER_HRNG_BLOCK_SIZE                 16


/* end of file c_eip76.h */
