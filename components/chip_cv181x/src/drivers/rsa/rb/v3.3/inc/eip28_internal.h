/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* eip28_internal.h
 *
 * Internal interface used by the EIP-28 Driver Library sub-components.
 *
 */



#ifndef EIP28_INTERNAL_H_
#define EIP28_INTERNAL_H_

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

#include "basic_defs.h"         // bool

#include "device_types.h"       // Device_Handle_t


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

typedef struct
{
    Device_Handle_t Device;

    // location of result vector(s) for last started operation
    unsigned int LastOperation_Result_wo;
    unsigned int LastOperation_Remainder_wo;

    // MSW register is not set for Sequencer operations,
    // so ReadResult_GetWordCount needs an alternative method
    bool LastOperationUsedSequencer;
    unsigned int LastOperation_ResultLen;
    // Number of words detected in PKA RAM
    unsigned int PKARamNrOfWords;
    // Number of words detected in PKA Program RAM
    unsigned int PKAProgNrOfWords;
} EIP28_True_IOArea_t;

#define IOAREA(_p) ((volatile EIP28_True_IOArea_t *)_p)


#ifdef EIP28_STRICT_ARGS
#define EIP28_INSERTCODE_FUNCTION_ENTRY_CODE \
    EIP28_Status_t rv = EIP28_STATUS_OK; \
    if (!IOArea_p) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    } \
    { /* MATCH_1 */ \
    volatile EIP28_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p); \
    Device_Handle_t Device = TrueIOArea_p->Device;

#define EIP28_INSERTCODE_FUNCTION_EXIT_CODE \
    } /* MATCH_1 */ \
FUNC_RETURN: \
    return rv;

#define EIP28_CHECK_POINTER(_p) \
    if (NULL == (_p)) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#define EIP28_CHECK_INT_ATLEAST(_i, _min) \
    if ((_i) < (_min)) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#define EIP28_CHECK_INT_ATMOST(_i, _max) \
    if ((_i) > (_max)) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#define EIP28_CHECK_INT_INRANGE(_i, _min, _max) \
    if ((_i) < (_min) || (_i) > (_max)) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#define EIP28_CHECK_BIGUINT_ODD(_i) \
   if (((_i) & 1) == 0) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#define EIP28_CHECK_64BIT_ALIGNMENT(_i) \
   if (((_i) & 1) != 0) \
   { \
       rv = EIP28_INVALID_PARAMETER; \
       goto FUNC_RETURN; \
   }

#define EIP28_CHECK_NO_OVERLAP(_start_addr1, _len1, _start_addr2, _len2) \
 { \
   unsigned int _end_addr1 = (unsigned int)((_start_addr1) + (_len1) - 1); \
   unsigned int _end_addr2 = (unsigned int)((_start_addr2) + (_len2) - 1); \
   if (!((_end_addr1 < (_start_addr2)) || (_end_addr2 < (_start_addr1)))) \
   { \
         rv = EIP28_INVALID_PARAMETER; \
         goto FUNC_RETURN; \
   } \
 }

#define EIP28_CHECK_MEMORY_OVERLAP(_start_addr1, _len1, _start_addr2) \
    { \
        unsigned int _end_addr1 = (_start_addr1) + (_len1) - 1; \
        if ((_start_addr2 > _start_addr1) && \
            (_start_addr2 <= (EIP28_WordOffset_t)_end_addr1)) \
        { \
            rv = EIP28_INVALID_PARAMETER; \
            goto FUNC_RETURN; \
        } \
    }

#define EIP28_CHECK_VECTOR_LIMIT(_startword, _wordcount) \
    if ( (_startword) + (_wordcount) > TrueIOArea_p->PKARamNrOfWords) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#define EIP28_CHECK_MEM_REFERENCE(_addr1, _addr2) \
    if (_addr1 != _addr2) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    }

#else
/* EIP28_STRICT_VARS disabled */

#define EIP28_INSERTCODE_FUNCTION_ENTRY_CODE \
    EIP28_Status_t rv = EIP28_STATUS_OK; \
    volatile EIP28_True_IOArea_t * const TrueIOArea_p = IOAREA(IOArea_p); \
    Device_Handle_t Device = TrueIOArea_p->Device; \
    if (TrueIOArea_p) {}    /* avoids warning */

#define EIP28_INSERTCODE_FUNCTION_EXIT_CODE \
    return rv;

#define EIP28_CHECK_POINTER(_p)
#define EIP28_CHECK_INT_ATLEAST(_i, _min)
#define EIP28_CHECK_INT_ATMOST(_i, _max)
#define EIP28_CHECK_INT_INRANGE(_i, _min, _max)
#define EIP28_CHECK_BIGUINT_ODD(_i)
#define EIP28_CHECK_64BIT_ALIGNMENT(_i)
#define EIP28_CHECK_MODULUS_ODD(_i)
#define EIP28_CHECK_NO_OVERLAP(_addr1, _len1, _addr2, _len2)
#define EIP28_CHECK_MEMORY_OVERLAP(_start_addr1, _len1, _start_addr2)
#define EIP28_CHECK_VECTOR_LIMIT(_startword, _bytecount)
#define EIP28_CHECK_MEM_REFERENCE(_addr1, _addr2)

#endif /* end of EIP28_STRICT_ARGS */


#ifdef EIP28_STRICT_VECTORS
#define CHECK_PKARAM_WORD_NOT_ZERO(_wo, _len) \
 { \
    unsigned int val = EIP28_PKARAM_RD(Device, _wo + _len - 1); \
    if (0 == val) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    } \
 }

#define CHECK_PKARAM_WORD_IS_ODD(_wo) \
 { \
    unsigned int val = EIP28_PKARAM_RD(Device, _wo); \
    if ((val & 1) == 0) \
    { \
        rv = EIP28_INVALID_PARAMETER; \
        goto FUNC_RETURN; \
    } \
 }

#else
/* EIP28_STRICT_VECTORS disabled */

#define CHECK_PKARAM_WORD_NOT_ZERO(_wo, _len)
#define CHECK_PKARAM_WORD_IS_ODD(_wo)

#endif /* end of EIP28_STRICT_VECTORS */

// Scratchpad size (in 32-bit words) for ModExp operations
#define EIP28_MODEXP_SCRATCHPAD_WORD_COUNT           9
#define EIP28_MODEXP_SCAP_SCRATCHPAD_WORD_COUNT      12

#define TEST_SIZEOF(type, size) \
    extern int size##_must_bigger[1 - 2*(sizeof(type) > size)]

#define VLEN(_l)            (_l)
#define VLEN1(_l)           ((_l)+ 1)
#define VLEN2(_l)           ((_l)+ 2)
#define VLEN3(_l)           ((_l)+ 3)
#define VLENA(_l)           ((_l) + 0 + ((_l + 0) & 1))
#define VLEN1A(_l)          ((_l) + 1 + ((_l + 1) & 1))
#define VLEN2A(_l)          ((_l) + 2 + ((_l + 2) & 1))
#define VLEN3A(_l)          ((_l) + 3 + ((_l + 3) & 1))



#define PROBE_PKA_PROG_SIZE      0
#define PROBE_PKA_RAM_SIZE       1

// validate the size of the fake and real IOArea structures
TEST_SIZEOF(EIP28_True_IOArea_t, EIP28_IOAREA_REQUIRED_SIZE);


/*----------------------------------------------------------------------------
 * EIP28_MakePkaRamAccessible
 */
void
EIP28_MakePkaRamAccessible(
        const Device_Handle_t Device);


#endif /* EIP28_INTERNAL_H_ */


/* end of file eip28_internal.h */
