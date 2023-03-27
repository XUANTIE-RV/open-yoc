/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* eip28.h
 *
 * EIP28 Driver Library API
 */



#ifndef INCLUDE_GUARD_EIP28_H
#define INCLUDE_GUARD_EIP28_H

#include "basic_defs.h"         // uint32_t, bool, inline, etc.
#include "device_types.h"       // Device_Handle_t
#include "rambus.h"

typedef enum
{
    EIP28_STATUS_OK,
    EIP28_INVALID_PARAMETER,
    EIP28_ERROR
} EIP28_Status_t;

// all memory locations in the PKA RAM are addressed as Word offsets
// a Word can hold 4 bytes of data
// there is typically 2 kilobytes or 4 kilobytes of memory
// use EIP28_Memory_GetWordCount() to get the number of words
// the first memory location is at word offset 0.
typedef unsigned short EIP28_WordOffset_t;


typedef struct
{
    // FALSE/0 = Store with Least Significant Byte first
    // TRUE/>0 = Store with Most  Significant Byte first
    bool StoreAsMSB;

    // reference to the data belonging to the big unsigned integer
    // the data must be stored elsewhere and must be accessible through the
    // pointer the integer is expected as a contiguous stream of bytes
    uint8_t * Bytes_p;

    // number of bytes from Bytes_p with significant bits
    // that is, Bytes_p[ByteCount-1] is the last byte with significant bits
    // must be at least 1
    unsigned int ByteCount;

} EIP28_BigUInt_t;

typedef enum
{
    EIP28_COMPARERESULT_A_EQUALS_B = 0,
    EIP28_COMPARERESULT_A_LESSTHAN_B,
    EIP28_COMPARERESULT_A_GREATERTHAN_B
} EIP28_CompareResult_t;

#define EIP28_IOAREA_REQUIRED_SIZE (8 * sizeof(void *))

typedef struct
{
    uint32_t placeholder[EIP28_IOAREA_REQUIRED_SIZE / sizeof(uint32_t)];
} EIP28_IOArea_t;

typedef enum
{
    MULTIPLY,
    ADD,
    SUBTRACT,
    ADDSUB,
    RIGHTSHIFT,
    LEFTSHIFT,
    DIVIDE,
    MODULO,
    COMPARE,
    COPY,
    MODEXP,
    MODEXP_CRT,
    MODINV,
    ECC_ADD_AFFINE,
    ECC_ADD_PROJECTIVE,
    ECC_MUL_AFFINE,
    ECC_MUL_PROJECTIVE,
    ECC_MUL_MONT,
    DSA_SIGN,
    DSA_VERIFY,
    ECCDSA_SIGN,
    ECCDSA_VERIFY,
    MODEXP_SCAP,
    MODEXP_CRT_SCAP,
    ECC_MUL_PROJ_SCAP,
    ECC_MUL_MONT_SCAP,
    DSA_SIGN_SCAP,
    ECCDSA_SIGN_SCAP,
    PREP_BLIND_MSG
} EIP28_Operation_List_t;

typedef struct
{
    struct
    {
        uint8_t Major;
        uint8_t Minor;
        uint8_t PatchLevel;
    } HW;

    struct
    {
        uint8_t Major;
        uint8_t Minor;
        uint8_t PatchLevel;
        uint8_t Capabilities;
    } FW;

    uint32_t Options;       // raw register value

} EIP28_RevisionInfo_t;


/*------------------------------------------------------------------------
 * Note: The EIP-28 Driver Library does not use any synchronization primitives
 *       (like mutex). All context synchronization to prevent concurrent
 *       access must be handled by the Library user. The Library API functions
 *       that require access synchronization have the _CALLATOMIC word at the
 *       end of the function name. This means these functions are not re-entrant
 *       and cannot be called concurrently.
 */

/*------------------------------------------------------------------------
 * Initialize
 */
EIP28_Status_t
EIP28_Initialize_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        Device_Handle_t Device,
        const uint32_t * const Firmware_p,
        unsigned int FirmwareWordCount);


/*------------------------------------------------------------------------
 * Revision Info
 */
EIP28_Status_t
EIP28_GetRevisionInfo(
        EIP28_IOArea_t * const IOArea_p,
        EIP28_RevisionInfo_t * const Info_p);


/*------------------------------------------------------------------------
 * BigUInt transfers
 */
unsigned int
EIP28_Memory_GetWordCount(
        EIP28_IOArea_t * const IOArea_p);

EIP28_Status_t
EIP28_Memory_PutBigUInt_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t StartWord,
        const unsigned int NrOfWordsToFill,         // 0 if not used
        const EIP28_BigUInt_t * const BigUInt_p,
        EIP28_WordOffset_t * const LastWordUsed_p); // optional, NULL allowed

EIP28_Status_t
EIP28_Memory_GetBigUInt_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t StartWord,
        const unsigned int NrOfWords,
        EIP28_BigUInt_t * const BigUInt_p);

EIP28_Status_t
EIP28_Memory_GetBigUInt_ZeroPad_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t StartWord,
        const unsigned int NrOfWords,
        EIP28_BigUInt_t * const BigUInt_p,
        const unsigned int WantedNumberLength_NrOfBytes);

// increment word offset
// and align to the next 64bit-aligned offset
// (could require additional increase)
static inline EIP28_WordOffset_t
EIP28_Memory_IncAndAlignNext64(
        const EIP28_WordOffset_t wo)
{
    return (EIP28_WordOffset_t)(wo+2-(wo & 1));
}


/*------------------------------------------------------------------------
 * Basic operations
 */

int
EIP28_GetScratchpadSize(
        const EIP28_Operation_List_t op);

int
EIP28_GetWorkAreaSize(
        const EIP28_Operation_List_t op,
        const unsigned int NrOfOddPowers,
        const unsigned int ALen,    // In words
        const unsigned int ModLen); // In words

void
EIP28_CheckIfDone(
        EIP28_IOArea_t * const IOArea_p,
        bool * const IsDone_p);

uint32_t
EIP28_GetPkaRamSize(
        EIP28_IOArea_t * const IOArea_p);

void
EIP28_SetSCAP_DummyCorr_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const bool Dummy_Corr);

bool
EIP28_CheckReset_SeqCtrl(
        EIP28_IOArea_t * const IOArea_p,
        bool * const Parity_Err,
        bool * const Hamming_Err);

bool
EIP28_CheckFsmError_SCAPCtrl_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p);

bool
EIP28_CheckFsmError_LNME0Status_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p);

uint32_t
EIP28_Status_SeqCntrl_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p);

// use for all operations, except Modulo
void
EIP28_ReadResult_WordCount_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        unsigned int * const Result_NrOfWords_p);

// use for Divide and Modulo
void
EIP28_ReadResult_RemainderWordCount_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        unsigned int * const Remainder_NrOfWords_p);

void
EIP28_ReadResult_Compare(
        EIP28_IOArea_t * const IOArea_p,
        EIP28_CompareResult_t * const CompareResult_p);

void
EIP28_ReadResult_ModInv_Status(
        EIP28_IOArea_t * const IOArea_p,
        bool * const Result_IsUndefined_p);

void
EIP28_ReadResult_ECCPoint_Status(
        EIP28_IOArea_t * const IOArea_p,
        bool * const Result_IsAtInfinity_p,
        bool * const Result_IsUndefined_p);

EIP28_Status_t
EIP28_StartOp_Add_AplusB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_Sub_AminB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_SubAdd_AplusCminB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const EIP28_WordOffset_t C_wo,
        const unsigned int ABC_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_ModuloOrCopy_AmodB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_Multiply_AmulB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_Divide_AdivB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Remainder_wo,
        const EIP28_WordOffset_t Quotient_wo);

EIP28_Status_t
EIP28_StartOp_Modulo_AmodB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_ShiftRight_AshiftrightS_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const unsigned int A_Len,
        const unsigned int S,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_ShiftLeft_AshiftleftS_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const unsigned int A_Len,
        const unsigned int S,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_Copy_Adup_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const unsigned int A_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_Compare_AcmpB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int AB_Len);

EIP28_Status_t
EIP28_StartOp_ModInv_invAmodB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_ModExp_CupAmodB_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const EIP28_WordOffset_t C_wo,
        const unsigned int A_Len,
        const unsigned int BC_Len,
        const unsigned int NrOfOddPowers,
        const EIP28_WordOffset_t Result_wo);

EIP28_Status_t
EIP28_StartOp_ModExp_CRT_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t ExpPQ_wo,
        const EIP28_WordOffset_t ModPQ_wo,
        const EIP28_WordOffset_t InvQ_wo,
        const EIP28_WordOffset_t InputResult_wo,
        const unsigned int NrOfOddPowers,
        const unsigned int A_Len,
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_EccAdd_Affine_AplusC_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t Axy_wo,
        const EIP28_WordOffset_t pa_wo,
        const EIP28_WordOffset_t Cxy_wo,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Resultxy_wo);

EIP28_Status_t
EIP28_StartOp_EccAdd_Projective_AplusC_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t Axyz_wo,
        const EIP28_WordOffset_t pa_wo,
        const EIP28_WordOffset_t Cxyz_wo,
        const unsigned int B_Len,
        const EIP28_WordOffset_t Resultxyz_wo);

EIP28_Status_t
EIP28_StartOp_EccMul_Affine_kmulC_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pab_wo,
        const EIP28_WordOffset_t Cxy_wo,
        const unsigned int A_Len,       // used for k only
        const unsigned int B_Len,
        const EIP28_WordOffset_t Resultxy_wo);

EIP28_Status_t
EIP28_StartOp_EccMul_Projective_kmulC_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pab_wo,
        const EIP28_WordOffset_t Cxy_wo,
        const unsigned int A_Len,       // used for k only
        const unsigned int B_Len,
        const EIP28_WordOffset_t Resultxyz_wo);

EIP28_Status_t
EIP28_StartOp_EccMul_Montgomery_kmulC_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pab_wo,
        const EIP28_WordOffset_t Cxy_wo,
        const unsigned int A_Len,       // used for k only
        const unsigned int B_Len,
        const EIP28_WordOffset_t Resultxy_wo);

EIP28_Status_t
EIP28_StartOp_DSA_Sign_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t alpha_wo,
        const EIP28_WordOffset_t pgn_wo,
        const EIP28_WordOffset_t h_wo,
        const EIP28_WordOffset_t inputResult_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const unsigned int S);

EIP28_Status_t
EIP28_StartOp_DSA_Verify_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t y_wo,
        const EIP28_WordOffset_t pgn_wo,
        const EIP28_WordOffset_t h_wo,
        const EIP28_WordOffset_t inputResult_wo,
        const unsigned int A_Len,
        const unsigned int B_Len,
        const unsigned int S);

EIP28_Status_t
EIP28_StartOp_ECCDSA_Sign_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pab_wo,
        const EIP28_WordOffset_t Cxy_wo,
        const EIP28_WordOffset_t inputResult_wo,
        const unsigned int A_Len,       // used for k only
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_ECCDSA_Verify_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pab_wo,
        const EIP28_WordOffset_t Cxy_wo,
        const EIP28_WordOffset_t inputResult_wo,
        const unsigned int A_Len,       // used for k only
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_ModExpScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const EIP28_WordOffset_t C_wo,
        const EIP28_WordOffset_t Result_wo,
        const unsigned int B_Len,
        const unsigned int Flags);

EIP28_Status_t
EIP28_StartOp_ModExpCrtScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const EIP28_WordOffset_t C_wo,
        const EIP28_WordOffset_t InputResult_wo,
        const unsigned int A_Len,
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_ECCMulProjScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pabn_wo,
        const EIP28_WordOffset_t P1xy_wo,
        const EIP28_WordOffset_t Resultxyz_wo,
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_ECCMulMontScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t k_wo,
        const EIP28_WordOffset_t pan_wo,
        const EIP28_WordOffset_t p1x_wo,
        const EIP28_WordOffset_t Resultx_wo,
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_DSASignScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t alpha_wo,
        const EIP28_WordOffset_t pgn_wo,
        const EIP28_WordOffset_t h_wo,
        const EIP28_WordOffset_t inputResult_wo,
        const unsigned int A_Len,
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_ECCDSASignScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t alpha_wo,
        const EIP28_WordOffset_t pabxy_wo,
        const EIP28_WordOffset_t h_wo,
        const EIP28_WordOffset_t inputResult_wo,
        const unsigned int B_Len);

EIP28_Status_t
EIP28_StartOp_PrepBlindMsgScap_CALLATOMIC(
        EIP28_IOArea_t * const IOArea_p,
        const EIP28_WordOffset_t A_wo,
        const EIP28_WordOffset_t B_wo,
        const EIP28_WordOffset_t Result_wo,
        const unsigned int A_Len,
        const unsigned int B_Len);

#endif /* Include Guard */

/* end of file eip28.h */
