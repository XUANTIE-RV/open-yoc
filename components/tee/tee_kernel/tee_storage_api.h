/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef TEE_STORAGE_API_H
#define TEE_STORAGE_API_H

#include "tee_common.h"

#define TEE_STORAGE_PRIVATE                 0x000

#define TEE_DATA_FLAG_ACCESS_READ           0x00000001
#define TEE_DATA_FLAG_ACCESS_WRITE          0x00000002
#define TEE_DATA_FLAG_ACCESS_WRITE_META     0x00000004
#define TEE_DATA_FLAG_SHARE_READ            0x00000010
#define TEE_DATA_FLAG_SHARE_WRITE           0x00000020
#define TEE_DATA_FLAG_CREATE                0x00000200
#define TEE_DATA_FLAG_OVERWRITE             0x00000400
#define TEE_DATA_FLAG_EXCLUSIVE             0x00000400

#define TEE_USAGE_EXTRACTABLE               0x00000001
#define TEE_USAGE_ENCRYPT                   0x00000002
#define TEE_USAGE_DECRYPT                   0x00000004
#define TEE_USAGE_MAC                       0x00000008
#define TEE_USAGE_SIGN                      0x00000010
#define TEE_USAGE_VERIFY                    0x00000020
#define TEE_USAGE_DERIVE                    0x00000040

#define TEE_DATA_MAX_POSITION               0xFFFFFFFF
#define TEE_OBJECT_ID_MAX_LEN               64

#define TEE_HANDLE_FLAG_PERSISTENT          0x00010000
#define TEE_HANDLE_FLAG_INITIALIZED         0x00020000
#define TEE_HANDLE_FLAG_KEY_SET             0x00040000
#define TEE_HANDLE_FLAG_EXPECT_TWO_KEYS     0x00080000

#define TEE_OPERATION_CIPHER                1
#define TEE_OPERATION_MAC                   3
#define TEE_OPERATION_AE                    4
#define TEE_OPERATION_DIGEST                5
#define TEE_OPERATION_ASYMMETRIC_CIPHER     6
#define TEE_OPERATION_ASYMMETRIC_SIGNATURE  7
#define TEE_OPERATION_KEY_DERIVATION        8

#define TEE_OPERATION_STATE_INITIAL         0x00000000
#define TEE_OPERATION_STATE_ACTIVE          0x00000001

typedef struct tee_attribute_t {
    uint32_t attributeid;

    union {
        struct {
            void *buffer;
            size_t length;
        } ref;

        struct {
            uint32_t a;
            uint32_t b;
        } value;
    } content;
} tee_attribute;

typedef struct tee_objectinfo_t {
    uint32_t    objectype;
    uint32_t    objectsize;
    uint32_t    maxobjectsize;
    uint32_t    objectusage;
    uint32_t    datasize;
    uint32_t    dataposition;
    uint32_t    handleflags;
} tee_objectinfo;

typedef enum tee_whence_e {
    TEE_DATA_SEEK_SET = 0,
    TEE_DATA_SEEK_CUR = 1,
    TEE_DATA_SEEK_END = 2
} tee_whence;

struct tee_objecthandle_t {
    tee_objectinfo  objectinfo;
    tee_attribute   *attr;
    uint32_t        attrcount;
};

typedef struct tee_objecthandle_t *tee_objecthandle;

struct tee_objectenumhandle_t {
    uint32_t id;
};

typedef struct tee_objectenumhandle_t *tee_objectenumhandle;



void tee_getobjectinfo(tee_objecthandle object, tee_objectinfo *objectinfo);
tee_result tee_restrictobjectusage1(tee_objecthandle object, uint32_t objectusage);
tee_result tee_getobjectbufferattribute(tee_objecthandle object, uint32_t attributeid,
                                        void *buffer, size_t *size);
tee_result tee_getobjectvalueattribute(tee_objecthandle object, uint32_t attributeid,
                                       uint32_t *a, uint32_t *b);
void tee_closeobject(tee_objecthandle object);

tee_result tee_allocatetransientobject(uint32_t objecttype, uint32_t maxobjectsize,
                                       tee_objecthandle *object);
void tee_freetransientobject(tee_objecthandle *object);
void tee_resettransientobject(tee_objecthandle object);
tee_result tee_populatetransientobject(tee_objecthandle object, tee_attribute *attrs,
                                       uint32_t attrcount);
void tee_initrefattribute(tee_attribute *attr, uint32_t attributeid, void *buffer,
                          size_t length);
void tee_initvalueattribute(tee_attribute *attr, uint32_t attributeid, uint32_t a,
                            uint32_t b);
void tee_copyobjectattributes(tee_objecthandle destobject, tee_objecthandle srcobject);
tee_result tee_generatekey(tee_objecthandle object, uint32_t keysize, tee_attribute *params,
                           uint32_t paramcount);
tee_result tee_setkey(tee_objecthandle object, uint8_t *key, uint32_t keysize);
tee_result tee_openpersistentobject(uint32_t storageid, void *objectid, size_t objectidlen,
                                    uint32_t flags, tee_objecthandle *object);
tee_result tee_createpersistentobject(uint32_t storageid, void *objectid, size_t objectidlen,
                                      uint32_t flags, tee_objecthandle attributes,
                                      void *initialdata, size_t initialdatalen,
                                      tee_objecthandle *object);
void tee_closeanddeletepersistentobject(tee_objecthandle object);


tee_result tee_readobjectdata(tee_objecthandle object, void *buffer, size_t size, uint32_t *count);
tee_result tee_writeobjectdata(tee_objecthandle object, void *buffer, size_t size);
tee_result tee_truncateobjectdata(tee_objecthandle object, uint32_t size);




#endif

