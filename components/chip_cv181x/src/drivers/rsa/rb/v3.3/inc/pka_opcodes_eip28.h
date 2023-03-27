/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/* pka_opcodes_eip28.h
 *
 * Public Key Big Integer Math Acceleration (PKA) Opcodes and Results for
 * EIP28
 */



#ifndef INCLUDE_GUARD_PKA_DETAILS_H
#define INCLUDE_GUARD_PKA_DETAILS_H

// Align64 if not aligned
#define PKA_WORDLEN_ROUNDUP_64(_i) \
    ((_i ) + (_i & 1))


/*----------------------------------------------------------------------------
 * PKA_Opcodes_t
 *
 * PKA Operation Codes available for EIP-28. This is a contract between
 * Application and Driver. These Opcodes can be set in "PKA_Command_t.OpCode"
 * by the Application to tell which operation need to be performed.
 * Before using the PKA Module, Application must get allocated a unique ID,
 * i.e. unique among existing applications, called session using PKA_Open
 * function
 *
 * INPUT PARAMETERS
 * ----------------
 * Following variables of PKA_Command_t are specific to particular Opcode and
 * should be set according to guidelines provided with each Opcode.
 *
 * "PKA_Command_t.Handles[]"
 * Depending on the Operation there can be Multiple Input/Output Parameter
 * Vectors, buffers for these parameters must be allocated by the Application
 * using DMAResource API and reference is set in the Handles. It should be
 * noted that some Operations require additional PKA RAM area, apart from
 * Input and Output vectors (for its intermediate processes) and this
 * limits the maximum length of input vectors (and some additional
 * parameters for e.g. Number of Odd Powers in Modular Exponentiation
 * Operation).
 *
 * Six vectors available for parameter passing and retrieving the result.
 * In some Operations, few Input Parameter buffers are grouped together, have
 * mandatory buffers in between and are required to be placed
 * sequentially/back-to-back, in which leading sub-vector must be zero-padded
 * to make it a multiple of 64bits before filling next sub-vector.
 * Mandatory buffer requirement and Zero-padding must be taken care while
 * calculating the length of the these kind of buffers.
 * The head pointer to the sequence of input parameter is provided in
 * the Handles.
 *
 * "PKA_Command_t.A_Len"
 * "PKA_Command_t.B_Len"
 * Length of each Parameter can be either length A_Length or B_Length, which
 * must be set in A_Len and B_Len. These length are in 32bit words - bit[0-31],
 * in which bit[31] is the most significant bit. Application must fill the
 * buffer with least significant word first. In some Operations it is required
 * that all the parameter must be of same length = size of largest vector
 * (for e.g. ECC Add), or any other condition arising in which vector doesn't
 * completely occupy allocated buffer; Application must zero out the unused
 * Most Significant words and bits of the buffer before submitting to PKA.
 *
 * "PKA_Command_t.Extra"
 * Some Opcodes require special information like - Shift value, Odd Powers etc
 * and is set in Extra. Extra is specified only in Opcode which requires it.
 *
 * "PKA_Command_t.Encrypted"
 * EIP-28 doesn't support input vectors in encrypted form, therefore is unused.
 *
 *
 * OUTPUT PARAMETERS
 * -----------------
 * These are considered valid only after PKA Module has completed the requested
 * Operation.
 * Outcome of any operation is updated in
 *  a) Output Handles in "PKA_Command_t.Handles"
 *  b) Result/Status in "PKA_Result_t.Result"
 *
 * Output Handles are updated with the output of the operation requested.
 * Size for the output vector R_Len, mentioned with each Operation is the
 * suggested length for allocation of output vector buffers. Actual size of
 * the output result can be read from PKA_Result_t.MSW and PKA_Result_t.MSbit,
 * using PKA_ResultGet() function after completion of operation.
 *
 * In Operations where result consists of sub-vectors, second sub-vector
 * starts at R_Len.
 *
 * "PKA_Result_t.Result" is set with the current status of the operation
 * requested after Command is successfully submitted to PKA Module and is
 * different from return codes of functions. For example result can be,
 * comparison result i.e. greater/less/equal, status of processing i.e. done,
 * pending etc. Results are enumerated in "PKA_Result_Internal_t".
 * For most of the Operations, result/status are PKA_RESULT_DONE or
 * PKA_RESULT_PENDING, other specific result apart from these is mentioned
 * with respective Opcode.
 *
 *
 * Note: The Handle mentioned in the table of each Operation is the "Handle
 *  Index Number". User must provide the Vectors in the associated Handle Index
 *  detailed with that Operation.
 */
typedef enum
{
    // Operation = Multiply A and B ( C = A * B )
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Multiplicand  A_Len                       Input
    //   [1]     Multiplier    B_Len                       Input
    //
    //  Handle   Vector        Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [2]     Product       R_Len                       Output
    //
    //  R_Len = A_Len + B_Len
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING

    PKA_OPCODE_MULTIPLY = 0x01,

    // Operation = Add A + B ( C = A + B)
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Addend        A_Len                       Input
    //   [1]     Addend        B_Len                       Input
    //
    //  Handle   Vector        Size                  Input/Output
    //  -------------------------------------------------------------------
    //   [2]     Result        R_Len                       Output
    //
    //  R_Len = Max(A_Len, BLen)+1
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ADD = 0x02,

    // Operation = Subtract A - B ( C = A - B)
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Minuend       A_Len                       Input
    //   [1]     Subtrahend    B_Len                       Input
    //
    //  Handle   Vector        Size                  Input/Output
    //  -------------------------------------------------------------------
    //   [2]     Result        R_Len                       Output
    //
    //  R_Len = Max(A_Len, BLen)
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_SUBTRACT = 0x03,

    // Operation = AddSubtract A + C - B ( D = A + C - B )
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Addend        A_Len                       Input
    //   [1]     Subtrahend    B_Len                       Input
    //   [2]     Addend        C_Len                       Input
    //
    //  Handle   Vector        Size                  Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result        R_Len                       Output
    //
    //  R_Len = A_Len+1
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ADD_SUBTRACT = 0x04,

    // Operation = Right-Shift A ( A >> shift C)
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Input         A_Len                       Input
    //
    //  Handle   Vector        Size                  Input/Output
    //  -------------------------------------------------------------------
    //   [1]     Result        R_Len                       Output
    //
    //  R_Len = A_Len
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_RIGHT_SHIFT = 0x05,

    // Operation = Left-Shift A ( A << shift C)
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Input         A_Len                       Input
    //
    //  Handle   Vector        Size                  Input/Output
    //  -------------------------------------------------------------------
    //   [1]     Result        R_Len                       Output
    //
    //  R_Len = A_Len+1 when shift is non-zero else A_LEN
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_LEFT_SHIFT = 0x06,

    // Operation = Divide Divisor 'A' by Dividend 'B'
    //      Provide Remainder 'C' and Quotient 'D'  ( C = A % B, D = A / B )
    //  Handle   Vector       Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Dividend     A_Len                       Input
    //   [1]     Divisor      B_Len                       Input
    //
    //  Handle   Vector       Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [2]     Remainder    R1_Len                       Output
    //   [3]     Quotient     R2_Len                       Output
    //
    //  R1_Len = B_Len + 1
    //  R2_Len = A_Len - B_Len + 1
    //
    //  Size of the Reminder vector can be calculated using PKA_Result_t.MSW2 .
    //  Size of the Quotient vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING

    PKA_OPCODE_DIVIDE = 0x07,

    // Operation = Divide two operands and provide Remainder ( C = A % B )
    //  Handle   Vector       Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Dividend     A_Len                       Input
    //   [1]     Divisor      B_Len                       Input
    //
    //  Handle   Vector       Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [2]     Remainder    R_Len                       Output
    //
    //  R_Len = B_Len + 1
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_MODULO = 0x08,

    // Operation = Compare A == B
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Input1        A_Len                       Input
    //   [1]     Input2        B_Len                       Input
    //
    //  Result is in PKA_COMPARE register
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_COMPARE = 0x09,

    // Operation = Copy A ( C = A)
    //  Handle   Vector        Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Input         A_Len                       Input
    //
    //  Handle   Vector        Size                  Input/Output
    //  -------------------------------------------------------------------
    //   [1]     Result        R_Len                       Output
    //
    //  R_Len = A_Len
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_COPY = 0x0A,

    /**********************************************************************/
    /*                          Sequencer Operations                      */
    /**********************************************************************/

    // Operation = Modular Exponentiation
    //                  (ModExp = (base^exponent) % modulus)
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Exponent            A_Len                       Input
    //   [1]     Modulus             B_Len                       Input
    //   [2]     Base                B_Len                       Input
    //   Extra   Number of           N/A                         Input
    //           Odd Powers
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = B_Len + 1
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING

    PKA_OPCODE_MODEXP = 0x0B,

    // Operation = Modular Exponentiation using CRT
    //              (ModExp = (base^exponent) % modulus)
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Exponent P (ExpP)   A_Len                       Input
    //           Exponent Q (ExpQ)   A_Len
    //   [1]     Modulus P  (ModP)   B_Len                       Input
    //           Modulus Q  (ModQ)   B_Len
    //   [2]     Inverse Q  (InvQ)   B_Len                       Input
    //   [3]     Input for           2 * B_Len                   Input
    //           MODEXP
    //   Extra   Number of           N/A                         Input
    //           Odd Powers
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = 2 * B_Len
    //
    //  Note: Input and Result Vector share same buffer
    //
    //  Vector Placement Requirements:
    //  Mandatory Buffer word after ModP. ExpP and ModP must be zero-padded to
    //      next multiple of 64bit (after the mandatory buffer if any), before
    //      placing ExpQ and ModQ respectively.
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING

    PKA_OPCODE_MODEXP_CRT = 0x0C,


    // Operation = Modular Inversion
    //              ModInv = (NumtoInvert^-1) % modulus
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //  [0]      Number to invert     A_Len                     Input
    //           for Inverse
    //  [1]      Modulus              B_Len                     Input
    //
    //  Handle   Vector               Size                   Input/Output
    //  -------------------------------------------------------------------
    //  [2]      Result               R_Len                     Output
    //
    //  R_Len = B_Len
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  If Result is undefined, then PKA_Result_t.fIsZero is TRUE,
    //  PKA_Result_t.MSW contains undefined value, and
    //  PKA_Result_t.Result is PKA_RESULT_ERR_NO_INV .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    //      PKA_RESULT_ERR_NO_INV
    //      PKA_RESULT_ERR_MOD_EVEN
    PKA_OPCODE_MODINV = 0x0D,

    // Operation = ECC Point Addition Affine
    //  Handle   Vector               Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     A.x                  B_Len                      Input
    //           A.y                  B_Len                      Input
    //   [1]     B.p                  B_Len                      Input
    //           B.a                  B_Len                      Input
    //           B.b                  B_Len                      Input
    //   [2]     C.x                  B_Len                      Input
    //           C.y                  B_Len                      Input
    //
    //  Handle   Vector               Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result.x             R_Len                      Output
    //           Result.y             R_Len                      Output
    //
    //  R_Len =  (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after A.x, C.x, p and a.
    //  A.x, p, a and C.x must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing A.y, a, b and
    //      C.y respectively.
    //  In the Result vector, Result.x is zero-padded to multiple of 64bit
    //      before placing Result.y.
    //  Size of Result.x and Result.y can be calculated using PKA_Result_t.MSW,
    //  but only if PKA_Result_t.Result is PKA_RESULT_DONE .
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  PKA_Result_t.fIsZero is always TRUE .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    //      PKA_RESULT_ERR_AT_INFINITY
    //      PKA_RESULT_ERR_NO_INV
    PKA_OPCODE_ECC_ADD_AFFINE = 0x0E,

    // Operation = ECC Point Addition Projective
    //  Handle   Vector               Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     A.x                  B_Len                      Input
    //           A.y                  B_Len                      Input
    //           A.z                  B_Len                      Input
    //   [1]     p                    B_Len                      Input
    //           a                    B_Len                      Input
    //           b                    B_Len                      Input
    //   [2]     C.x                  B_Len                      Input
    //           C.y                  B_Len                      Input
    //           C.z                  B_Len                      Input
    //
    //  Handle   Vector               Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result - Res.x       R_Len                      Output
    //           Res.y                R_Len                      Output
    //           Res.z                R_Len                      Output
    //
    //  R_Len =  (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after A.x, A.y, C.x, C.y, p and a.
    //  A.x, A,y, p , a, C.x and C.y must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing A.y, a and
    //      C.y respectively.
    //  In the Result vector, Res.x is zero-padded to multiple of 64bit before
    //      placing Res.y.
    //  Size of Res.x and Res.y can be calculated using PKA_Result_t.MSW, but
    //  only if PKA_Result_t.Result is PKA_RESULT_DONE .
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  PKA_Result_t.fIsZero is always TRUE .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    //      PKA_RESULT_ERR_AT_INFINITY
    //      PKA_RESULT_ERR_NO_INV
    PKA_OPCODE_ECC_ADD_PROJECTIVE = 0x10,

    // Operation = ECC Point Multiplication Affine
    //
    //  Handle   Vector               Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Scalar (k)           A_Len                      Input
    //   [1]     p                    B_Len                      Input
    //           a                    B_Len                      Input
    //           b                    B_Len                      Input
    //   [2]     A.x                  B_Len                      Input
    //           A.y                  B_Len                      Input
    //           Rz                   B_Len                      Input
    //
    //  Handle   Vector               Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result - Res.x       R_Len                      Output
    //           Res.y                R_Len                      Output
    //
    //  R_Len = (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after A.x, A.y, p and a.
    //      A.x, A.y, p and a must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing C.y, a and b.
    //  In the Result vector, Res.x is zero-padded to multiple of 64bit before
    //      placing Res.y.
    //  Size of Res.x and Res.y can be calculated using PKA_Result_t.MSW, but
    //  only if PKA_Result_t.Result is PKA_RESULT_DONE
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  PKA_Result_t.fIsZero is always TRUE .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    //      PKA_RESULT_ERR_AT_INFINITY
    //      PKA_RESULT_ERR_NO_INV
    PKA_OPCODE_ECC_MULTIPLY_AFFINE = 0x0F,

    // Operation = ECC Point Multiplication Projective
    //
    //  Handle   Vector               Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Scalar (k)           A_Len                      Input
    //   [1]     p                    B_Len                      Input
    //           a                    B_Len                      Input
    //           b                    B_Len                      Input
    //   [2]     A.x                  B_Len                      Input
    //           A.y                  B_Len                      Input
    //           Rz                   B_Len                      Input
    //
    //  Handle   Vector               Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result - Res.x       R_Len                      Output
    //           Res.y                R_Len                      Output
    //           Res.z                R_Len                      Output
    //
    //  R_Len =  (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after A.x, A.y, p and a.
    //      A.x, A.y, p and a must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing C.y, a and b.
    //  In the Result vector, Res.x and Res.y are zero-padded to multiple of 64bit before
    //      placing Res.y.
    //  Size of Res.x, Res.y and Res.z can be calculated using PKA_Result_t.MSW, but
    //  only if PKA_Result_t.Result is PKA_RESULT_DONE
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  PKA_Result_t.fIsZero is always TRUE .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    //      PKA_RESULT_ERR_AT_INFINITY
    //      PKA_RESULT_ERR_NO_INV
    PKA_OPCODE_ECC_MULTIPLY_PROJECTIVE = 0x11,

    // Operation = ECC Multiply with Montgomery
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Scalar (k)          A_Len                       Input
    //   [2]     Modulus (p)         B_Len                       Input
    //           Curve Parameter (a) B_Len                       Input
    //   [2]     P1_x                B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len =  B_Len
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after p.
    //      p must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing a.
    //  Size of Res can be calculated using PKA_Result_t.MSW, but
    //  only if PKA_Result_t.Result is PKA_RESULT_DONE
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ECC_MULTIPLY_MONTGOMERY = 0x12,

    // Operation = DSA signature generation
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Alpha               B_Len                       Input
    //   [1]     Modulus (p)         A_Len                       Input
    //           g                   B_Len                       Input
    //           n                   B_Len                       Input
    //   [2]     h                   B_Len                       Input
    //   [3]     k                   B_Len                       Input
    //   Extra   Number of           N/A                         Input
    //           Odd Powers
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result.r            R_Len                       Output
    //           Result.s            R_Len                       Output
    //
    //  R_Len = (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after p and g.
    //      p and g must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing g and n.
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_DSA_SIGN = 0x13,

    // Operation = DSA signature verification
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     y                   A_Len                       Input
    //   [1]     p                   A_Len                       Input
    //           g                   A_Len                       Input
    //           n                   B_Len                       Input
    //   [2]     h                   B_Len                       Input
    //   [3]     r'                  B_Len                       Input
    //           s'                  B_Len                       Input
    //   Extra   Number of           N/A                         Input
    //           Odd Powers
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after p g and r'.
    //      p, g and r' must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing g, n and s'.

    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_DSA_VERIFY = 0x14,

    // Operation = ECC DSA signature generation
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Alpha               B_Len                       Input
    //   [1]     Modulus (p)         B_Len                       Input
    //           a                   B_Len                       Input
    //           b                   B_Len                       Input
    //           n                   B_Len                       Input
    //           G_x                 B_Len                       Input
    //           G_y                 B_Len                       Input
    //           G_Rz                B_Len                       Input
    //   [2]     h                   B_Len                       Input
    //   [3]     k                   B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after p, a, b, n, G_x and G_y.
    //      p, a, b, n, G_x and G_y must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing a, b, n, G_x
    //      and G_y and G_Rx.
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ECDSA_SIGN = 0x15,

    // Operation = ECC DSA signature verification
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     PKP_x               B_Len                       Input
    //           PKP_y               B_Len                       Input
    //           PKP_Rz              B_Len                       Input
    //   [1]     Modulus (p)         A_Len                       Input
    //           a                   B_Len                       Input
    //           b                   B_Len                       Input
    //           n                   B_Len                       Input
    //           G_x                 B_Len                       Input
    //           G_y                 B_Len                       Input
    //           G_Rz                B_Len                       Input
    //   [2]     h                   B_Len                       Input
    //   [3]     r'                  B_Len                       Input
    //           s'                  B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory two buffer words after PKP_x, PKP_y, p, a, b, n, G_x and G_y.
    //      PKP_x, PKP_y, p, a, b, n, G_x and G_y must be zero-padded to next
    //      multiple of 64bit (after the mandatory buffer if any),
    //      before placing PKP_y, PKP_Rz, a, b, n, G_x and G_y and G_Rx.
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ECDSA_VERIFY = 0x16,

    // Operation = Modular Exponentiation with SCAP
    //                  (ModExp = (base^exponent) % modulus)
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     D                   B_Len                       Input
    //   [1]     N                   B_Len                       Input
    //           UnBlind             B_Len                       Input
    //           Blind               B_Len                       Input
    //           phiN                B_Len                       Input
    //   [2]     M                   B_Len                       Input
    //   Extra   Values available    N/A                         Input
    //           bit_0=1 no (un)blinding values present
    //           bit_1=1 phiN value present
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = B_Len + 1
    //
    //  Vector Placement Requirements:
    //  Mandatory 3 buffer words after B and one buffer word after all other values.
    //  UnBlind, Blind and phiNmust be zero-padded to
    //      next multiple of 64bit (after the mandatory buffer if any), before
    //      placing ExpQ and ModQ respectively.
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_MODEXP_SCAP = 0x17,

    // Operation = Modular Exponentiation using CRT with SCAP
    //              (ModExp = (base^exponent) % modulus)
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Dp                  B_Len                       Input
    //           Dq                  B_Len
    //           E                   A_Len
    //   [1]     p                   B_Len                       Input
    //           q                   B_Len
    //           UnBlind             2 * B_Len
    //           Blind               2 * B_Len
    //   [2]     qInv                B_Len                       Input
    //   [3]     M                   2 * B_Len                   Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = 2 * B_Len
    //
    //  Note: Input and Result Vector share same buffer
    //
    //  Vector Placement Requirements:
    //  Mandatory 3 buffer words after Dp, Dq and mandatory one buffer
    //      word after p, q, Unblind, Blind and M
    //  Unblind, Blind and  phiN must be zero-padded to
    //      next multiple of 64bit (after the mandatory buffer if any), before
    //      placing ExpQ and ModQ respectively.
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_MODEXP_CRT_SCAP = 0x18,

    // Operation = ECC Point Multiplication Projective with SCAP
    //
    //  Handle   Vector               Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Scalar (k)           B_Len                      Input
    //   [1]     p                    B_Len                      Input
    //           a                    B_Len                      Input
    //           b                    B_Len                      Input
    //           n                    B_Len                      Input
    //   [2]     P1.x                 B_Len                      Input
    //           P1.y                 B_Len                      Input
    //
    //  Handle   Vector               Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result - Res.x       B_Len                      Output
    //           Res.y                B_Len                      Output
    //           Res.z                B_Len                      Output
    //
    //  R_Len =  3 * (B_Len + (B_Len & 1) + 2)
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //  Mandatory three buffer words after k and two buffer words after
    //      p, a, b, n, P1_x and P1_y.
    //      a, b, n and P1_y must be zero-padded to next multiple of 64bit.
    //  In the Result vector, Res.x and Res.y are zero-padded to multiple of 64bit before
    //      placing Res.y.
    //  Size of Res.x, Res.y and Res.z can be calculated using PKA_Result_t.MSW, but
    //  only if PKA_Result_t.Result is PKA_RESULT_DONE
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  PKA_Result_t.fIsZero is always TRUE .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    //      PKA_RESULT_ERR_AT_INFINITY
    //      PKA_RESULT_ERR_NO_INV
    PKA_OPCODE_ECC_MULTIPLY_PROJECTIVE_SCAP = 0x19,

    // Operation = ECC Multiply with Montgomery with SCAP
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     k                   B_Len                       Input
    //   [1]     p                   B_Len                       Input
    //           a                   B_Len                       Input
    //           n                   B_Len                       Input
    //   [2]     P1_x                B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result - Res.x      B_Len                      Output
    //
    //  R_Len =  B_Len + 2
    //
    // Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    // Vector Placement Requirements:
    //   Mandatory three buffer words after k and mandatory two buffer words
    //      after p, a, n, P1_x and P1_y
    //  a, b, n and P1_y must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing a.
    //  Size of Res can be calculated using PKA_Result_t.MSW, but
    //  only if PKA_Result_t.Result is PKA_RESULT_DONE
    //  Note:
    //      PKA_Result_t.MSW equals B_LEN - 1
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ECC_MULTIPLY_MONTGOMERY_SCAP = 0x1A,

    // Operation = Prep Blind Msg
    //              Generate a value pair for Message blinding
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     E                   A_Len                       Input
    //   [1]     N                   B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [2]     Result              R_Len                       Output
    //
    //  R_Len = 2 * B_Len
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_PREP_BLIND_MSG = 0x1B,

    // Operation = DSA signature generation with SCAP
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Alpha               B_Len                       Input
    //   [1]     Modulus (p)         A_Len                       Input
    //           g                   B_Len                       Input
    //           n                   B_Len                       Input
    //   [2]     h                   B_Len                       Input
    //   [3]     k                   B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result.r            R_Len                       Output
    //           Result.s            R_Len                       Output
    //
    //  R_Len = 2 * (B_Len + (B_Len & 1) + 2)
    //
    //  Note: Input and Result Vector share same buffer
    //
    //  Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    //  Vector Placement Requirements:
    //  Mandatory two buffer words after p, g, n and k.
    //      g and n must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing g and n.
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_DSA_SIGN_SCAP = 0x1C,


    // Operation = ECC DSA signature generation with SCAP
    //
    //  Handle   Vector              Length                 Input/Output
    //  -------------------------------------------------------------------
    //   [0]     Alpha               B_Len                       Input
    //   [1]     Modulus (p)         B_Len                       Input
    //           a                   B_Len                       Input
    //           b                   B_Len                       Input
    //           n                   B_Len                       Input
    //           G_x                 B_Len                       Input
    //           G_y                 B_Len                       Input
    //   [2]     h                   B_Len                       Input
    //   [3]     k                   B_Len                       Input
    //
    //  Handle   Vector              Size                   Input/Output
    //  -------------------------------------------------------------------
    //   [3]     Result              R_Len                       Output
    //
    //  R_Len = (2 * B_Len + (B_Len & 1) + 2)
    //
    //  Note: Input and Result Vector share same buffer
    //
    //  Note: B_Len should be the length of the largest number.
    //       Effective modulus size (in bits) must be a multiple of 32
    //
    //  Vector Placement Requirements:
    //  Mandatory two buffer words after p, a, b, n, G_x, G_y and k.
    //      a, b, n, G_x and G_y must be zero-padded to next multiple of 64bit
    //      (after the mandatory buffer if any), before placing a, b, n, G_x
    //      and G_y.
    //
    //  Size of the Result vector can be calculated using PKA_Result_t.MSW .
    //
    //  Possible Result/Status (in "PKA_Result_t.Result"):
    //      PKA_RESULT_DONE
    //      PKA_RESULT_PENDING
    PKA_OPCODE_ECDSA_SIGN_SCAP = 0x1D,


} PKA_Opcodes_EIP28_t;


/*----------------------------------------------------------------------------
 * PKA_Result_Internal_t
 *
 * Possible result values when user calls PKA_ResultGet. This includes the
 * state of the operation being performed.
 */
typedef enum
{
    // No error
    PKA_RESULT_DONE = 0x00,

    // PKA RAM too small for operation with given parameters
    PKA_RESULT_ERR_OUT_OF_MEM = 0x01,

    // Result Undefined
    PKA_RESULT_ERR_UNDEFINED = 0x02,

    // Result point of ECC operation is at infinity
    PKA_RESULT_ERR_AT_INFINITY = 0x03,

    // Operand length error
    PKA_RESULT_ERR_LEN = 0x04,

    // Modular inverse does not exist
    PKA_RESULT_ERR_NO_INV = 0x05,

    // Operation started, waiting for operation to complete
    PKA_RESULT_PENDING = 0x06

} PKA_Result_EIP28_t;

#define PKA_COMMAND_BITS_MASK                   0x00077FF7
#define PKA_COMMAND_BITS_NONE                   0x00000000
#define PKA_COMMAND_BITS_MUL                    0x00000001
#define PKA_COMMAND_BITS_ADDSUB                 0x00000002
#define PKA_COMMAND_BITS_ADD                    0x00000010
#define PKA_COMMAND_BITS_SUB                    0x00000020
#define PKA_COMMAND_BITS_RSHIFT                 0x00000040
#define PKA_COMMAND_BITS_LSHIFT                 0x00000080
#define PKA_COMMAND_BITS_DIVIDE                 0x00000100
#define PKA_COMMAND_BITS_MOD                    0x00000200
#define PKA_COMMAND_BITS_CMP                    0x00000400
#define PKA_COMMAND_BITS_COPY                   0x00000800
#define PKA_COMMAND_BITS_MODEXP_CRT             0x00001000
#define PKA_COMMAND_BITS_ECC_MONT_MUL           0x00002000
#define PKA_COMMAND_BITS_ECC_ADD_AFF            0x00003000
#define PKA_COMMAND_BITS_ECC_MUL_AFF            0x00005000
#define PKA_COMMAND_BITS_MODEXP                 0x00006000
#define PKA_COMMAND_BITS_MODINV                 0x00007000
#define PKA_COMMAND_BITS_ECC_ADD_PROJ           0x00010000
#define PKA_COMMAND_BITS_ECC_MUL_PROJ           0x00011000
#define PKA_COMMAND_BITS_ECC_SCALE_PROJ         0x00012000
#define PKA_COMMAND_BITS_EC2M                   0x00014000
#define PKA_COMMAND_BITS_DSA_SIGN               0x00020000
#define PKA_COMMAND_BITS_DSA_VERIFY             0x00021000
#define PKA_COMMAND_BITS_ECCDSA_SIGN            0x00022000
#define PKA_COMMAND_BITS_ECCDSA_VERIFY          0x00023000

#define PKA_COMMAND_BITS_MODEXP_SCAP            0x00045000
#define PKA_COMMAND_BITS_MODEXP_CRT_SCAP        0x00043000
#define PKA_COMMAND_BITS_ECC_MUL_PROJ_SCAP      0x00061000
#define PKA_COMMAND_BITS_ECC_MUL_MONT_SCAP      0x00046000
#define PKA_COMMAND_BITS_DSA_SIGN_SCAP          0x00052000
#define PKA_COMMAND_BITS_ECDSA_SIGN_SCAP        0x00054000
#define PKA_COMMAND_BITS_PREP_BLIND_MSG         0x00051000


#endif /* Include Guard */

/* end of file pka_opcodes_eip28.h */

