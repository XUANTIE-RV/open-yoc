/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#ifndef _SC_DEVICE_H_
#define _SC_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sec_crypto_aes.h>
#include <sec_crypto_errcode.h>

typedef struct {
    const uint8_t	*pub_key;
    uint32_t		pub_key_size;
    const uint8_t	*pubkey_hash;
    uint32_t		pubkey_hash_size;
    const uint8_t	*huk;
    uint32_t		huk_size;
    const uint8_t	*chip_id;
    uint32_t		chip_id_size;
    uint32_t		sap;
    const uint8_t	*sad;
    uint32_t		sad_size;
} sec_device_info_t;

typedef struct {
    sc_aes_t aes;
    uint32_t idx;
} sc_dev_storage_t;

typedef struct {
    const uint8_t 	*data_in;
    uint8_t 		*data_out;
    uint32_t 		data_size;
} sc_dev_data_info_t;

/**
  \brief       The interface of the authentication of secure device
  \param[in]   sec_dev  A pointer to the struct of secure device infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_auth_probe(sec_device_info_t *sec_dev);

/**
  \brief       The interface of the initialization of secure storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_init(sc_dev_storage_t *sec_storage);

/**
  \brief       The interface of the unnitialization of secure storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
void sc_device_storage_uninit(sc_dev_storage_t *sec_storage);

/**
  \brief       The interface of encrypting secure storage data
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_encrypt(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info);

/**
  \brief       The interface of decrypting secure storage data
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_decrypt(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info);


/**
  \brief       The interface of the initialization of secure storage with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_init_with_ks(sc_dev_storage_t *sec_storage);

/**
  \brief       The interface of the unnitialization of secure storage with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \return      0: successfull other: error code
*/
void sc_device_storage_uninit_with_ks(sc_dev_storage_t *sec_storage);

/**
  \brief       The interface of  encrypting secure storage data with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_encrypt_with_ks(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info);

/**
  \brief       The interface of decrypting secure storage data with key storage
  \param[in]   sec_storage  A pointer to the struct of secure storage infomation.
  \param[in]   data_info  A pointer to the struct of data infomation.
  \return      0: successfull other: error code
*/
uint32_t sc_device_storage_decrypt_with_ks(sc_dev_storage_t *sec_storage, 
                                  sc_dev_data_info_t *data_info);

/**
  \brief       Get the capacity the secure storage
  \param[out]   size  A pointer to the buffer which will be stored the size of storage.
  \return      0 success, Non-zero failure
*/
uint32_t sc_get_avail_storage_capacity(uint32_t *size);

/**
  \brief       Get the capacity the secure storage
  \param[in]   data  A pointer to the buffer which stores the data need to be written.
  \param[in]   size  The size of data which will be written.
  \param[in]   pos  The position which the data will be written to.
  \return      0 success, Non-zero failure
*/
uint32_t sc_write_storage_data(const uint8_t *data, uint32_t size, uint32_t pos);

/**
  \brief       Get the capacity the secure storage
  \param[out]   data  A pointer to the buffer which stores the data read out.
  \param[in]   size  The size of data which will be read.
  \param[in]   pos  The position which the data will be read.
  \return      0 success, Non-zero failure
*/
uint32_t sc_read_storage_data(uint8_t *data , uint32_t size, uint32_t pos);

#ifdef __cplusplus
}
#endif

#endif /* _SC_DEVICE_H_ */