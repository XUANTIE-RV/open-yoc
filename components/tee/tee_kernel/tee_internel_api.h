/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_INTERNEL_API_H
#define TEE_INTERNEL_API_H

#include "tee_common.h"

#define TEE_CORE_API_MAJOR_VERSION          1
#define TEE_CORE_API_MINOR_VERSION          0
#define TEE_CORE_API_MAINTENANCE_VERSION    0
#define TEE_CORE_API_VERSION                (TEE_CORE_API_MAJOR_VERSION << 24) + \
    (TEE_CORE_API_MINOR_VERSION << 16) + \
    (TEE_CORE_API_MAINTENANCE_VERSION << 8)

/*
 * return code
 */
#define TEE_SUCCESS                         0x00000000
#define TEE_ERROR_GENERIC                   0xFFFF0000
#define TEE_ERROR_ACCESS_DENIED             0xFFFF0001
#define TEE_ERROR_CANCEL                    0xFFFF0002
#define TEE_ERROR_ACCESS_CONFLICT           0xFFFF0003
#define TEE_ERROR_EXCESS_DATA               0xFFFF0004
#define TEE_ERROR_BAD_FORMAT                0xFFFF0005
#define TEE_ERROR_BAD_PARAMETERS            0xFFFF0006
#define TEE_ERROR_BAD_STATE                 0xFFFF0007
#define TEE_ERROR_ITEM_NOT_FOUND            0xFFFF0008
#define TEE_ERROR_NOT_IMPLEMENTED           0xFFFF0009
#define TEE_ERROR_NOT_SUPPORTED             0xFFFF000A
#define TEE_ERROR_NO_DATA                   0xFFFF000B
#define TEE_ERROR_OUT_OF_MEMORY             0xFFFF000C
#define TEE_ERROR_BUSY                      0xFFFF000D
#define TEE_ERROR_COMMUNICATION             0xFFFF000E
#define TEE_ERROR_SECURITY                  0xFFFF000F
#define TEE_ERROR_SHORT_BUFFER              0xFFFF0010
#define TEE_PENDING                         0xFFFF2000
#define TEE_ERROR_TIMEOUT                   0xFFFF3001
#define TEE_ERROR_OVERFLOW                  0xFFFF300F
#define TEE_ERROR_TARGET_DEAD               0xFFFF3024
#define TEE_ERROR_STORAGE_NO_SPACE          0xFFFF3041
#define TEE_ERROR_MAC_INVALID               0xFFFF3071
#define TEE_ERROR_SIGNATURE_INVALID         0xFFFF3072
#define TEE_ERROR_TIME_NOT_SET              0xFFFF5000
#define TEE_ERROR_TIME_NEEDS_RESET          0xFFFF5001

/*
 * return code origins
 */
#define TEE_ORIGIN_API                      0x00000001
#define TEE_ORIGIN_COMMS                    0x00000002
#define TEE_ORIGIN_TEE                      0x00000003
#define TEE_ORIGIN_TRUSTED_APP              0x00000004

/*
 * parameter types
 */
#define TEE_NONE                       0x00000000
#define TEE_VALUE_INPUT                0x00000001
#define TEE_VALUE_OUTPUT               0x00000002
#define TEE_VALUE_INOUT                0x00000003
#define TEE_MEMREF_TEMP_INPUT          0x00000005
#define TEE_MEMREF_TEMP_OUTPUT         0x00000006
#define TEE_MEMREF_TEMP_INOUT          0x00000007

typedef enum {
    TEE_TYPE_AES                = 0xA0000010,
    TEE_TYPE_DES                = 0xA0000011,
    TEE_TYPE_DES3               = 0xA0000013,
    TEE_TYPE_HMAC_MD5           = 0xA0000001,
    TEE_TYPE_HMAC_SHA1          = 0xA0000002,
    TEE_TYPE_HMAC_SHA224        = 0xA0000003,
    TEE_TYPE_HMAC_SHA256        = 0xA0000004,
    TEE_TYPE_HMAC_SHA384        = 0xA0000005,
    TEE_TYPE_HMAC_SHA512        = 0xA0000006,
    TEE_TYPE_RSA_PUBLIC_KEY     = 0xA0000030,
    TEE_TYPE_RSA_KEYPAIR        = 0xA1000030,
    TEE_TYPE_DSA_PUBLIC_KEY     = 0xA0000031,
    TEE_TYPE_DSA_KEYPAIR        = 0xA1000031,
    TEE_TYPE_DH_KEYPAIR         = 0xA1000032,
    TEE_TYPE_ECDSA_PUBLIC_KEY   = 0xA0000041,
    TEE_TYPE_ECDSA_KEYPAIR      = 0xA1000041,
    TEE_TYPE_GENERIC_SECRET     = 0xA0000000,
} objtype;

typedef enum {
    TEE_ATTR_SECRET_VALUE           = 0xC0000000,
    TEE_ATTR_RSA_MODULUS            = 0xD0000130,
    TEE_ATTR_RSA_PUBLIC_EXPONENT    = 0xD0000230,
    TEE_ATTR_RSA_PRIVATE_EXPONENT   = 0xC0000330,
    TEE_ATTR_RSA_PRIME1             = 0xC0000430,
    TEE_ATTR_RSA_PRIME2             = 0xC0000530,
    TEE_ATTR_RSA_EXPONENT1          = 0xC0000630,
    TEE_ATTR_RSA_EXPONENT2          = 0xC0000730,
    TEE_ATTR_RSA_COEFFICIENT        = 0xC0000830,
    TEE_ATTR_DSA_PRIME              = 0xD0001031,
    TEE_ATTR_DSA_SUBPRIME           = 0xD0001131,
    TEE_ATTR_DSA_BASE               = 0xD0001231,
    TEE_ATTR_DSA_PUBLIC_VALUE       = 0xD0000131,
    TEE_ATTR_DSA_PRIVATE_VALUE      = 0xC0000231,
    TEE_ATTR_DH_PRIME               = 0xD0001032,
    TEE_ATTR_DH_SUBPRIME            = 0xD0001132,
    TEE_ATTR_DH_BASE                = 0xD0001232,
    TEE_ATTR_DH_X_BITS              = 0xF0001332,
    TEE_ATTR_DH_PUBLIC_VALUE        = 0xD0000132,
    TEE_ATTR_DH_PRIVATE_VALUE       = 0xC0000232,
    TEE_ATTR_RSA_OAEP_LABEL         = 0xD0000930,
    TEE_ATTR_RSA_PSS_SALT_LENGTH    = 0xF0000A30,
    TEE_ATTR_ECC_PUBLIC_VALUE_X     = 0xD0000141,
    TEE_ATTR_ECC_PUBLIC_VALUE_Y     = 0xD0000241,
    TEE_ATTR_ECC_PRIVATE_VALUE      = 0xC0000341,
    TEE_ATTR_ECC_CURVE              = 0xF0000441
} objfuncattr;

typedef enum {
    TEE_ATTR_FLAG_VALUE = 0x20000000,
    TEE_ATTR_FLAG_PUBLIC = 0x10000000
} attridflag;

typedef enum {
    TEE_ECC_CURVE_NIST_P192 = 0x00000001,
    TEE_ECC_CURVE_NIST_P224 = 0x00000002,
    TEE_ECC_CURVE_NIST_P256 = 0x00000003,
    TEE_ECC_CURVE_NIST_P384 = 0x00000004,
    TEE_ECC_CURVE_NIST_P521 = 0x00000005
} objecccurve;


typedef struct tee_identity_t {
    tee_uuid    uuid;
} tee_identity;

typedef struct tee_instance_t {
    uint32_t dummy;
} tee_instance;

tee_result ta_createentrypoint(void);
void ta_destroyentrypoint(void);
tee_result ta_opensessionentrypoint(uint32_t  paramtypes,
                                    tee_param params[4],
                                    void    **sessioncontext);
void ta_closesessionentrypoint(void *sessioncontext);
tee_result ta_invokecommandentrypoint(void     *sessioncontext,
                                      uint32_t  commandid,
                                      uint32_t  paramtypes,
                                      tee_param params[4]);

void process_ta_command(uint32_t recv_msg);

#endif

