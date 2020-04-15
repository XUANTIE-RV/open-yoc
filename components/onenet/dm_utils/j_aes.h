
#ifndef __J_AES_H__
#define __J_AES_H__

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void  hex_print(const char *name, const unsigned char *buf, size_t len);
void  HexStrToByte(const char *source, int sourceLen, unsigned char *dest);
int   EncryptionAES(char *szin, int inlen, unsigned char *key, unsigned char *ciphertext);
char *StrSHA256(const char *str, int length, char *sha256);

#ifdef __cplusplus
}
#endif


#endif //__J_BASE_64_H__

