#include <stdio.h>
#include <stdlib.h>
#include <csi_core.h>
#include "aos/cli.h"

#include "drv/sha.h"
#include "drv/aes.h"

#if 0
void cvi_security_base64_encode_test(int32_t argc, char **argv)
{
    __aligned(32) uint32_t dma_descriptor[22] = { 0 };

    char src[8] = {"abcdefgh"};
    uint32_t len = 8;

    uint32_t status;
    uint32_t ts;
    int i;

    void *dst = aos_calloc(1, 256);

    // Prepare descriptor
    dma_descriptor[CRYPTODMA_CTRL] = DES_USE_DESCRIPTOR_KEY | DES_USE_BASE64 | 0xF;
    dma_descriptor[CRYPTODMA_CIPHER] = 0x1;

    dma_descriptor[CRYPTODMA_SRC_ADDR_L] = (uint32_t)((uintptr_t)src & 0xFFFFFFFF);
    dma_descriptor[CRYPTODMA_SRC_ADDR_H] = (uint32_t)((uintptr_t)src >> 32);

    dma_descriptor[CRYPTODMA_DST_ADDR_L] = (uint32_t)((uintptr_t)dst & 0xFFFFFFFF);
    dma_descriptor[CRYPTODMA_DST_ADDR_H] = (uint32_t)((uintptr_t)dst >> 32);

    dma_descriptor[CRYPTODMA_DATA_AMOUNT_L] = (uint32_t)(len & 0xFFFFFFFF);
    dma_descriptor[CRYPTODMA_DATA_AMOUNT_H] = (uint32_t)(len >> 32);

    dma_descriptor[CRYPTODMA_KEY] = (len + (3 - 1))/3 * 4;
    
    // Set cryptodma control
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_INT_MASK, 0x3);
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_L,
                    (uint32_t)((uintptr_t)dma_descriptor & 0xFFFFFFFF));
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_H,
                    (uint32_t)((uintptr_t)dma_descriptor >> 32));

    status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL);

    csi_dcache_clean_range((unsigned long)dma_descriptor, sizeof(dma_descriptor));
    csi_dcache_clean_range((uintptr_t)src, len);

    // Clear interrupt
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT, 0x3);
    // Trigger cryptodma engine
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
                    DMA_WRITE_MAX_BURST << 24 | DMA_READ_MAX_BURST << 16 | DMA_DESCRIPTOR_MODE << 1 | DMA_ENABLE);

    //ts = get_timer(0);
    do {
        status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT);
        //aos_cli_printf("INT status 0x%x\n", status);
        //if (get_timer(ts) >= 1000) {
        //    aos_cli_printf("Decryption timeout\n");
        //    return -EIO;
        //}
    } while (status == 0);
    
    csi_dcache_invalid_range((uintptr_t)dst, 256);
    aos_cli_printf("%s\n", dst);
}

void cvi_security_base64_decode_test(int32_t argc, char **argv)
{
    __aligned(32) uint32_t dma_descriptor[22] = { 0 };

    char src[12] = {"YWJjZGVmZ2g="};
    uint32_t len = 12;

    uint32_t status;
    uint32_t ts;
    int i;

    char *dst = aos_calloc(1, 9);

    // Prepare descriptor
    dma_descriptor[CRYPTODMA_CTRL] = DES_USE_DESCRIPTOR_KEY | DES_USE_BASE64 | 0xF;
    dma_descriptor[CRYPTODMA_CIPHER] = 0x0;

    dma_descriptor[CRYPTODMA_SRC_ADDR_L] = (uint32_t)((uintptr_t)src & 0xFFFFFFFF);
    dma_descriptor[CRYPTODMA_SRC_ADDR_H] = (uint32_t)((uintptr_t)src >> 32);

    dma_descriptor[CRYPTODMA_DST_ADDR_L] = (uint32_t)((uintptr_t)dst & 0xFFFFFFFF);
    dma_descriptor[CRYPTODMA_DST_ADDR_H] = (uint32_t)((uintptr_t)dst >> 32);

    dma_descriptor[CRYPTODMA_DATA_AMOUNT_L] = (uint32_t)(len & 0xFFFFFFFF);
    dma_descriptor[CRYPTODMA_DATA_AMOUNT_H] = (uint32_t)(len >> 32);

    dma_descriptor[CRYPTODMA_KEY] = 8;
    
    // Set cryptodma control
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_INT_MASK, 0x3);
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_L,
                    (uint32_t)((uintptr_t)dma_descriptor & 0xFFFFFFFF));
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DES_BASE_H,
                    (uint32_t)((uintptr_t)dma_descriptor >> 32));

    status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL);

    csi_dcache_clean_range((unsigned long)dma_descriptor, sizeof(dma_descriptor));
    csi_dcache_clean_range((uintptr_t)src, len);

    // Clear interrupt
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT, 0x3);
    // Trigger cryptodma engine
    mmio_write_32(SEC_CRYPTODMA_BASE + CRYPTODMA_DMA_CTRL,
                    DMA_WRITE_MAX_BURST << 24 | DMA_READ_MAX_BURST << 16 | DMA_DESCRIPTOR_MODE << 1 | DMA_ENABLE);

    //ts = get_timer(0);
    do {
        status = mmio_read_32(SEC_CRYPTODMA_BASE + CRYPTODMA_WR_INT);
        //aos_cli_printf("INT status 0x%x\n", status);
        //if (get_timer(ts) >= 1000) {
        //    aos_cli_printf("Decryption timeout\n");
        //    return -EIO;
        //}
    } while (status == 0);
    
    csi_dcache_invalid_range((uintptr_t)dst, 9);
    for (i = 0; i < 8; i++)
        aos_cli_printf("%c", (char)dst[i]);
}
#endif

void cvi_security_sha1_test(int32_t argc, char **argv)
{
    csi_sha_t sha = {0};
    csi_sha_context_t context = {0};
    csi_sha_mode_t mode = SHA_MODE_1;
    uint8_t output[20] = {0};
    uint32_t out_size = 0;

    csi_sha_init(&sha, 0);
    csi_sha_start(&sha, &context, mode);
    csi_sha_update(&sha, &context, "helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhell", 64);
    csi_sha_finish(&sha, &context, output, &out_size);

    int i;
    printf("sha1 result :\n");
    for (i = 0; i < out_size; i++) {
        aos_cli_printf("i : %x, 0x%x \n", i, output[i]);
    }
    printf("\n");
}

void cvi_security_sha256_test(int32_t argc, char **argv)
{
    csi_sha_t sha = {0};
    csi_sha_context_t context = {0};
    csi_sha_mode_t mode = SHA_MODE_256;
    uint8_t output[32] = {0};
    uint32_t out_size = 0;

    csi_sha_init(&sha, 0);
    csi_sha_start(&sha, &context, mode);
    csi_sha_update(&sha, &context, "helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhell", 64);
    csi_sha_finish(&sha, &context, output, &out_size);

    int i;
    printf("sha256 result:\n");
    for (i = 0; i < out_size; i++) {
        aos_cli_printf("i : %x, 0x%x \n", i, output[i]);
    }
    printf("\n");
}

void cvi_security_aes_test(int32_t argc, char **argv)
{
    csi_aes_t aes = {0};
    char key[16] = {"abcdefghabcdefgh"};
    char word[16] = {"abcdef"};
    char iv[16] = {"a"};
    char encrypt_output[256];
    char decrypt_output[256];

    int i;

    //Padding data
    for (i = 6; i < 16; i++){
        word[i] = 0xa;
    }

    csi_aes_init(&aes, 0);
    csi_aes_set_encrypt_key(&aes, key, AES_KEY_LEN_BITS_128);
    csi_aes_set_decrypt_key(&aes, key, AES_KEY_LEN_BITS_128);
    
    csi_aes_ecb_encrypt(&aes, word, encrypt_output, 16);
    printf("aes ecb encrypt :\n");
    for (i = 0; i < 16; i++) {
        printf("0x%x ", encrypt_output[i]);
    }
    printf("\n");

    csi_aes_ecb_decrypt(&aes, encrypt_output, decrypt_output, 16);
    printf("aes ecb decrypt :\n");
    for (i = 0; i < 16; i++) {
        printf("0x%x ", decrypt_output[i]);
    }
    printf("\n");

    csi_aes_cbc_encrypt(&aes, word, encrypt_output, 16, iv);
    printf("aes cbc encrypt :\n");
    for (i = 0; i < 16; i++) {
        printf("0x%x ", encrypt_output[i]);
    }
    printf("\n");

    csi_aes_cbc_decrypt(&aes, encrypt_output, decrypt_output, 16, iv);
    printf("aes cbc decrypt :\n");
    for (i = 0; i < 16; i++) {
        printf("0x%x ", decrypt_output[i]);
    }
    printf("\n");
}

ALIOS_CLI_CMD_REGISTER(cvi_security_sha1_test, sha1, sha1 test);
ALIOS_CLI_CMD_REGISTER(cvi_security_sha256_test, sha256, sha256 test);
