#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core_804.h"
#include "wm_irq.h"
#include "wm_regs.h"
#include "wm_debug.h"
#include "wm_crypto_hard.h"
#include "wm_internal_flash.h"

//#define TEST_ALL_CRYPTO
#undef	DIGIT_BIT
#define DIGIT_BIT			32 //28

#define SOFT_RESET_RC4    	25
#define SOFT_RESET_AES    	26
#define SOFT_RESET_DES    	27

#define RNG_SWITCH        	28
#define RNG_LOAD_SEED    	29
#define RNG_START         	30

#define PS_ARG_FAIL   -6
//#define CRYPTO_LOG printf
#define CRYPTO_LOG(...)
//extern volatile uint32_t sys_count;
#define sys_count tls_os_get_time()

volatile u8 crypto_complete = 0;
typedef s32 psPool_t;
extern void	pstm_reverse(unsigned char *s, int16 len);
extern int32 pstm_read_unsigned_bin(hstm_int *a, unsigned char *b, int32 c);		
extern void pstm_clear(hstm_int * a);
extern int32 pstm_to_unsigned_bin_nr(psPool_t *pool, hstm_int *a, unsigned char *b);
extern int32 pstm_init(psPool_t *pool, hstm_int * a);
extern int16 pstm_count_bits (hstm_int * a);
extern int32 pstm_2expt(hstm_int *a, int16 b);
extern int32 pstm_mod(psPool_t *pool, hstm_int *a, hstm_int *b, hstm_int *c);
extern int32 pstm_mulmod(psPool_t *pool, hstm_int *a, hstm_int *b, hstm_int *c,hstm_int *d);
extern int32 pstm_copy(hstm_int * a, hstm_int * b);
extern int16 pstm_get_bit (hstm_int * a, int16 idx);
extern void pstm_set(hstm_int *a, hstm_digit b);
extern void pstm_clamp(hstm_int * a);

#define STORE32H(x, y) { \
(y)[0] = (unsigned char)(((x)>>24)&255); \
(y)[1] = (unsigned char)(((x)>>16)&255); \
(y)[2] = (unsigned char)(((x)>>8)&255); \
(y)[3] = (unsigned char)((x)&255); \
}

#define LOAD32H(x, y) { \
x = ((unsigned long)((y)[0] & 255)<<24) | \
((unsigned long)((y)[1] & 255)<<16) | \
((unsigned long)((y)[2] & 255)<<8)  | \
((unsigned long)((y)[3] & 255)); \
}
#define STORE32L(x, y) \
{ unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32L(x, y) \
{ memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }


void RSA_F_IRQHandler(void)
{
	RSACON = 0x00;
	crypto_complete = 1;
}
void CRYPTION_IRQHandler(void)
{
	tls_reg_write32(HR_CRYPTO_SEC_STS, 0x10000);
	crypto_complete = 1;
}

#if 0
static int16 pstm_get_bit (hstm_int * a, int16 idx)
{
	int16     r;
	int16 n = idx / DIGIT_BIT;
	int16 m = idx % DIGIT_BIT;

	if (a->used <= 0) {
		return 0;
	}
	
	r = (a->dp[n] >> m) & 0x01;
	return r;
}
#endif
u32 Reflect(u32 ref,u8 ch)
{	
	int i;
	u32 value = 0;
	for( i = 1; i < ( ch + 1 ); i++ )
	{
		if( ref & 1 )
			value |= 1 << ( ch - i );
		ref >>= 1;
	}
	return value;
}
void tls_crypto_set_key(void* key, int keylen)
{
	uint32_t *key32 = (uint32_t *)key;
	int i = 0;
	for(i = 0; i < keylen / 4 && i < 6; i++)
	{
		M32(HR_CRYPTO_KEY0 + (4 * i)) = key32[i];
	}
	if(keylen == 32)
	{
		M32(HR_CRYPTO_KEY6) = key32[6];
		M32(HR_CRYPTO_KEY7) = key32[7];
	}
}
void tls_crypto_set_iv(void* iv, int ivlen)
{
	uint32_t *IV32 = (uint32_t *)iv;

	if(ivlen >= 8)
	{
		M32(HR_CRYPTO_IV0) = IV32[0];
		M32(HR_CRYPTO_IV0 + 4) = IV32[1];
	}
	if(ivlen == 16)
	{
		M32(HR_CRYPTO_IV1) = IV32[2];
		M32(HR_CRYPTO_IV1 + 4) = IV32[3];
	}
}

/**
 * @brief          	This function is used to stop random produce.
 *
 * @param[in]      	None
 *
 * @retval         	0     		success
 * @retval         	other 	failed
 *
 * @note           	None
 */
int tls_crypto_random_stop(void)
{
	unsigned int sec_cfg, val;
	val = tls_reg_read32(HR_CRYPTO_SEC_CFG);
	sec_cfg = val & ~(1 << RNG_START);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	
	return ERR_CRY_OK;
}

/**
 * @brief          	This function initializes random digit seed and BIT number.
 *
 * @param[in]   	seed 		The random digit seed.
 * @param[in]   	rng_switch 	The random digit bit number.   (0: 16bit    1:32bit)				
 *
 * @retval  		0  			success 
 * @retval  		other   		failed  
 *
 * @note             	None
 */
int tls_crypto_random_init(u32 seed, CRYPTO_RNG_SWITCH rng_switch)
{
	unsigned int sec_cfg;
	tls_crypto_random_stop();
	tls_reg_write32(HR_CRYPTO_KEY0, seed);
	sec_cfg = (rng_switch << RNG_SWITCH) | (1 << RNG_LOAD_SEED) | (1 << RNG_START);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	return ERR_CRY_OK;
}

/**
 * @brief          	This function is used to get random digit content.
 *
 * @param[in]   	out 			Pointer to the output of random digit.
 * @param[in]   	len 			The random digit bit number will output.
 *
 * @retval  		0  			success 
 * @retval  		other   		failed  
 *
 * @note             	None
 */
int tls_crypto_random_bytes(unsigned char *out, u32 len)
{
	unsigned int val;
	uint32 inLen = len;
	int randomBytes = 2;
	val = tls_reg_read32(HR_CRYPTO_SEC_CFG);
	randomBytes = val & (1 << RNG_SWITCH) ? 4 : 2;
	while(inLen > 0)
	{
		val = tls_reg_read32(HR_CRYPTO_RNG_RESULT);
		if(inLen >= randomBytes)
		{
			memcpy(out, (char*)&val, randomBytes);
			out += randomBytes;
			inLen -= randomBytes;
		}
		else
		{
			memcpy(out, (char*)&val, inLen);
			inLen = 0;
		}
	}
	return ERR_CRY_OK;
}

/**
 * @brief          	This function initializes a RC4 encryption algorithm,  
 *				i.e. fills the hsCipherContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]   	ctx 		Pointer to the Cipher Context. 
 * @param[in]   	key 		Pointer to the key. 
 * @param[in]   	keylen 	the length of key. 
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  

 *
 * @note             	The first parameter ctx must be a structure which is allocated externally. 
 *      			And all of Context parameters in the initializing methods should be allocated externally too.
 */
int tls_crypto_rc4_init(hsCipherContext_t * ctx, const unsigned char *key, u32 keylen)
{
	if(keylen != 16 && keylen != 32)
	{
		return ERR_FAILURE;
	}
	memcpy(ctx->arc4.state, key, keylen);
	ctx->arc4.byteCount = keylen;
	return ERR_CRY_OK;
}


/**
 * @brief          	This function encrypts a variable length data stream according to RC4.
 *				The RC4 algorithm it generates a "keystream" which is simply XORed with the plaintext to produce the ciphertext stream. 
 *				Decryption is exactly the same as encryption. This function also decrypts a variable length data stream according to RC4.
 *
 * @param[in]   	ctx 		Pointer to the Cipher Context.
 * @param[in]   	in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]   	out 		Pointer to the resulting ciphertext data stream. 
 * @param[in]		len 		Length of the plaintext data stream in octets.
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  
 *
 * @note             	None
 */
int tls_crypto_rc4(hsCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len)
{
	unsigned int sec_cfg;
	unsigned char *key = ctx->arc4.state;
	u32 keylen = ctx->arc4.byteCount;
	tls_crypto_set_key(key, keylen);
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)in);
	tls_reg_write32(HR_CRYPTO_DEST_ADDR, (unsigned int)out);
	sec_cfg = (CRYPTO_METHOD_RC4 << 16) | (1 << SOFT_RESET_RC4) | (len & 0xFFFF);
	if(keylen == 32){
		sec_cfg |= (1 << 31);
	}
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	CRYPTO_LOG("[%d]:rc4[%d] start\n", sys_count, len);
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{

	}
	crypto_complete = 0;
	CRYPTO_LOG("[%d]:rc4 end status: %x\n", sys_count, tls_reg_read32(HR_CRYPTO_SEC_STS));
	return ERR_CRY_OK;
}


/**
 * @brief          	This function initializes a AES encryption algorithm,  i.e. fills the hsCipherContext_t structure pointed to by ctx with necessary data.
 *
 * @param[in]   	ctx 		Pointer to the Cipher Context. 
 * @param[in]   	IV 		Pointer to the Initialization Vector
 * @param[in]   	key 		Pointer to the key. 
 * @param[in]		keylen 	the length of key.
 * @param[in]   	cbc 		the encryption mode, AES supports ECB/CBC/CTR modes.
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  
 *
 * @note             	None
 */
int tls_crypto_aes_init(hsCipherContext_t * ctx, const unsigned char *IV, const unsigned char *key, u32 keylen, CRYPTO_MODE cbc)
{
	int x = 0;
	if (keylen != 16)
		return ERR_FAILURE;

	memcpy(ctx->aes.key.eK, key, keylen);
	ctx->aes.key.Nr = cbc;
	ctx->aes.blocklen = 16;
	for (x = 0; x < ctx->aes.blocklen; x++) {
		ctx->aes.IV[x] = IV[x];
	}
	return ERR_CRY_OK;
}
 
/**
 * @brief			This function encrypts or decrypts a variable length data stream according to AES.
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]		out 		Pointer to the resulting ciphertext data stream.
 * @param[in]		len 		Length of the plaintext data stream in octets.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_aes_encrypt_decrypt(hsCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len, CRYPTO_WAY dec)
{
	unsigned int sec_cfg;
	u32 keylen = 16;
	unsigned char *key = (unsigned char *)ctx->aes.key.eK;
	unsigned char *IV = ctx->aes.IV;
	CRYPTO_MODE cbc = (CRYPTO_MODE)(ctx->aes.key.Nr & 0xFF);
	tls_crypto_set_key(key, keylen);
	tls_crypto_set_iv(IV, 16);
	
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)in);
	tls_reg_write32(HR_CRYPTO_DEST_ADDR, (unsigned int)out);
	sec_cfg = (CRYPTO_METHOD_AES << 16) | (1 << SOFT_RESET_AES) |(dec << 20) | (cbc << 21) | (len & 0xFFFF);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	CRYPTO_LOG("[%d]:aes[%d] %s %s start\n", sys_count, len, dec == CRYPTO_WAY_ENCRYPT ? "ENCRYPT" : "DECRYPT",
				cbc == CRYPTO_MODE_ECB ? "ECB" : (cbc == CRYPTO_MODE_CBC ? "CBC" : (cbc == CRYPTO_MODE_CTR ? "CTR" : "MAC")));
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{
	
	}
	crypto_complete = 0;
	CRYPTO_LOG("[%d]:aes end %d\n", sys_count, tls_reg_read32(HR_CRYPTO_SEC_STS) & 0xFFFF);
	return ERR_CRY_OK;
}

/**
 * @brief			This function initializes a 3DES encryption algorithm,  i.e. fills the hsCipherContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		IV 		Pointer to the Initialization Vector
 * @param[in]		key 		Pointer to the key. 
 * @param[in]		keylen 	the length of key. 
 * @param[in]		cbc 		the encryption mode, 3DES supports ECB/CBC modes.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_3des_init(hsCipherContext_t * ctx, const unsigned char *IV, const unsigned char *key, u32 keylen, CRYPTO_MODE cbc)
{
	unsigned int x;
	if (keylen != DES3_KEY_LEN)
		return ERR_FAILURE;

	memcpy(ctx->des3.key.ek[0], key, keylen);
	ctx->des3.key.ek[1][0] =  cbc;
	ctx->des3.blocklen = DES3_IV_LEN;
	for (x = 0; x < ctx->des3.blocklen; x++) {
		ctx->des3.IV[x] = IV[x];
	}
	
	return ERR_CRY_OK;
}

/**
 * @brief			This function encrypts or decrypts a variable length data stream according to 3DES.
 *
 * @param[in]		ctx 		Pointer to the Cipher Context.
 * @param[in]		in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]		out 		Pointer to the resulting ciphertext data stream.
 * @param[in]		len 		Length of the plaintext data stream in octets.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_3des_encrypt_decrypt(hsCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len, CRYPTO_WAY dec)
{
	unsigned int sec_cfg;
	u32 keylen = DES3_KEY_LEN;
	unsigned char *key = (unsigned char *)(unsigned char *)ctx->des3.key.ek[0];
	unsigned char *IV = ctx->des3.IV;
	CRYPTO_MODE cbc = (CRYPTO_MODE)(ctx->des3.key.ek[1][0] & 0xFF);
	tls_crypto_set_key(key, keylen);
	tls_crypto_set_iv(IV, DES3_IV_LEN);
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)in);
	tls_reg_write32(HR_CRYPTO_DEST_ADDR, (unsigned int)out);
	sec_cfg = (CRYPTO_METHOD_3DES << 16) | (1 << SOFT_RESET_DES) | (dec << 20) | (cbc << 21) | (len & 0xFFFF);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	CRYPTO_LOG("[%d]:3des[%d] %s %s start\n", sys_count, len, dec == CRYPTO_WAY_ENCRYPT ? "ENCRYPT" : "DECRYPT",
				cbc == CRYPTO_MODE_ECB ? "ECB" : "CBC");
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{

	}
	crypto_complete = 0;
	CRYPTO_LOG("[%d]:3des end %d\n", sys_count, tls_reg_read32(HR_CRYPTO_SEC_STS) & 0xFFFF);
	return ERR_CRY_OK;
}
  

/**
 * @brief			This function initializes a DES encryption algorithm,  i.e. fills the hsCipherContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		IV 		Pointer to the Initialization Vector
 * @param[in]		key 		Pointer to the key. 
 * @param[in]		keylen 	the length of key. 
 * @param[in]		cbc 		the encryption mode, DES supports ECB/CBC modes.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_des_init(hsCipherContext_t * ctx, const unsigned char *IV, const unsigned char *key, u32 keylen, CRYPTO_MODE cbc)
{
	unsigned int x;
	if (keylen != DES_KEY_LEN)
		return ERR_FAILURE;
	memcpy(ctx->des3.key.ek[0], key, keylen);
	ctx->des3.key.ek[1][0] =  cbc;
	ctx->des3.blocklen = DES3_IV_LEN;
	for (x = 0; x < ctx->des3.blocklen; x++) {
		ctx->des3.IV[x] = IV[x];
	}
	return ERR_CRY_OK;
}


/**
 * @brief			This function encrypts or decrypts a variable length data stream according to DES.
 *
 * @param[in]		ctx 		Pointer to the Cipher Context. 
 * @param[in]		in 		Pointer to the input plaintext data stream(or the encrypted text data stream) of variable length.
 * @param[in]		out 		Pointer to the resulting ciphertext data stream.
 * @param[in]		len 		Length of the plaintext data stream in octets.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *
 * @retval		0  		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_des_encrypt_decrypt(hsCipherContext_t * ctx, unsigned char *in, unsigned char *out, u32 len, CRYPTO_WAY dec)
{
	unsigned int sec_cfg;
	u32 keylen = DES_KEY_LEN;
	unsigned char *key = (unsigned char *)ctx->des3.key.ek[0];
	unsigned char *IV = ctx->des3.IV;
	CRYPTO_MODE cbc = (CRYPTO_MODE)(ctx->des3.key.ek[1][0] & 0xFF);
	//uint32_t *IV32 = (uint32_t *)IV;

	tls_crypto_set_key(key, keylen);
	tls_crypto_set_iv(IV, DES3_IV_LEN);
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)in);
	tls_reg_write32(HR_CRYPTO_DEST_ADDR, (unsigned int)out);
	sec_cfg = (CRYPTO_METHOD_DES << 16) | (1 << SOFT_RESET_DES) | (dec << 20) | (cbc << 21) | (len & 0xFFFF);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	CRYPTO_LOG("[%d]:des[%d] %s %s start\n", sys_count, len, dec == CRYPTO_WAY_ENCRYPT ? "ENCRYPT" : "DECRYPT",
				cbc == CRYPTO_MODE_ECB ? "ECB" : "CBC");
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{

	}
	crypto_complete = 0;
	CRYPTO_LOG("[%d]:des end %d\n", sys_count, tls_reg_read32(HR_CRYPTO_SEC_STS) & 0xFFFF);
	return ERR_CRY_OK;
}

 
/**
 * @brief			This function initializes a CRC algorithm,  i.e. fills the psCrcContext_t structure pointed to by ctx with necessary data. 
 *
 * @param[in]		ctx 		Pointer to the CRC Context. 
 * @param[in]		key 		The initialization key. 
 * @param[in]		crc_type 	The CRC type, supports CRC8/CRC16 MODBUS/CRC16 CCITT/CRC32
 * @param[in]		mode 	Set input or outpu reflect.
 * @param[in]		dec 		The cryption way which indicates encryption or decryption.
 *				see OUTPUT_REFLECT
 * 				see INPUT_REFLECT
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_crc_init(psCrcContext_t * ctx, u32 key, CRYPTO_CRC_TYPE crc_type, u8 mode)
{
	ctx->state = key;
	ctx->type = crc_type;
	ctx->mode = mode;
	return ERR_CRY_OK;
}

/**
 * @brief			This function updates the CRC value with a variable length bytes.
 *				This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]		ctx 		Pointer to the CRC Context. 
 * @param[in]		in 		Pointer to a variable length bytes
 * @param[in]		len 		The bytes 's length 
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_crc_update(psCrcContext_t * ctx, unsigned char *in, u32 len)
{
	unsigned int sec_cfg;
	sec_cfg =  (CRYPTO_METHOD_CRC << 16) | (ctx->type << 21) | (ctx->mode << 23) | (len & 0xFFFF);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	if(ctx->mode & OUTPUT_REFLECT)
	{
		u8 ch_crc = 16;
		u32 state = 0;
		switch(ctx->type)
		{
			case CRYPTO_CRC_TYPE_8:
				ch_crc = 8;
				break;
			case CRYPTO_CRC_TYPE_16_MODBUS:
				ch_crc = 16;
				break;
			case CRYPTO_CRC_TYPE_16_CCITT:
				ch_crc = 16;
				break;
			case CRYPTO_CRC_TYPE_32:
				ch_crc = 32;
				break;
			default:
				break;
		}
		state = Reflect(ctx->state, ch_crc);
		tls_reg_write32(HR_CRYPTO_CRC_KEY, state);
	}
	else
		tls_reg_write32(HR_CRYPTO_CRC_KEY, ctx->state);
	
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)in);
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{

	}
	crypto_complete = 0;
	ctx->state = tls_reg_read32(HR_CRYPTO_CRC_RESULT); 
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x4);//clear crc fifo
	return ERR_CRY_OK;
}


/**
 * @brief			This function ends a CRC operation and produces a CRC value.
 *
 * @param[in]		ctx 		Pointer to the CRC Context. 
 * @param[in]		crc_val 	Pointer to the CRC value.
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
int tls_crypto_crc_final(psCrcContext_t * ctx, u32 *crc_val)
{
	*crc_val = ctx->state; 
	return ERR_CRY_OK;
}

static void hd_sha1_compress(hsDigestContext_t *md)
{
	unsigned int sec_cfg, val;
	int i = 0;
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)md->sha1.buf);

	sec_cfg = (CRYPTO_METHOD_SHA1 << 16) | (64 & 0xFFFF); // TODO
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST0, md->sha1.state[0]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST1, md->sha1.state[1]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST2, md->sha1.state[2]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST3, md->sha1.state[3]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST4, md->sha1.state[4]);
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{

	}
	crypto_complete = 0;
	for (i = 0; i < 5; i++) {
		val = tls_reg_read32(HR_CRYPTO_SHA1_DIGEST0 + (4*i));
		md->sha1.state[i] = val;
	}
}


/**
 * @brief			This function initializes Message-Diggest context for usage in SHA1 algorithm, starts a new SHA1 operation and writes a new Digest Context. 
 *
 * @param[in]		md 		Pointer to the SHA1 Digest Context. 
 *
 * @retval		0		success 
 * @retval		other	failed	
 *
 * @note			None
 */
void tls_crypto_sha1_init(hsDigestContext_t * md)
{
	md->sha1.state[0] = 0x67452301UL;
	md->sha1.state[1] = 0xefcdab89UL;
	md->sha1.state[2] = 0x98badcfeUL;
	md->sha1.state[3] = 0x10325476UL;
	md->sha1.state[4] = 0xc3d2e1f0UL;
	md->sha1.curlen = 0;
#ifdef HAVE_NATIVE_INT64
	md->sha1.length = 0;
#else
	md->sha1.lengthHi = 0;
	md->sha1.lengthLo = 0;
#endif /* HAVE_NATIVE_INT64 */
}


/**
 * @brief			Process a message block using SHA1 algorithm.
 *				This function performs a SHA1 block update operation. It continues an SHA1 message-digest operation, 
 *				by processing InputLen-byte length message block pointed to by buf, and by updating the SHA1 context pointed to by md.
 *				This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]		md		Pointer to the SHA1 Digest Context. 
 * @param[in]  	buf 		InputLen-byte length message block
 * @param[in]  	len 		The buf 's length 
 *
 * @returnl		None	
 *
 * @note			None
 */
void tls_crypto_sha1_update(hsDigestContext_t * md, const unsigned char *buf, u32 len)
{
	u32 n;
	while (len > 0) {
		n = min(len, (64 - md->sha1.curlen));
		memcpy(md->sha1.buf + md->sha1.curlen, buf, (size_t)n);
		md->sha1.curlen		+= n;
		buf					+= n;
		len					-= n;

		/* is 64 bytes full? */
		if (md->sha1.curlen == 64) {
			hd_sha1_compress(md);
#ifdef HAVE_NATIVE_INT64
			md->sha1.length += 512;
#else
			n = (md->sha1.lengthLo + 512) & 0xFFFFFFFFL;
			if (n < md->sha1.lengthLo) {
				md->sha1.lengthHi++;
			}
			md->sha1.lengthLo = n;
#endif /* HAVE_NATIVE_INT64 */
			md->sha1.curlen = 0;
		}
	}
}

 
/**
 * @brief			This function ends a SHA1 operation and produces a Message-Digest.
 *				This function finalizes SHA1 algorithm, i.e. ends an SHA1 Message-Digest operation, 
 *				writing the Message-Digest in the 20-byte buffer pointed to by hash in according to the information stored in context. 
 *
 * @param[in]		md		Pointer to the SHA1 Digest Context. 
 * @param[in]		hash 	Pointer to the Message-Digest
 *
 * @retval  		20  		success, return the hash size.
 * @retval  		<0   	failed  

 *
 * @note			None
 */
int tls_crypto_sha1_final(hsDigestContext_t * md, unsigned char *hash)
{
	s32	i;
	u32 val;
#ifndef HAVE_NATIVE_INT64
	u32	n;
#endif
	if (md->sha1.curlen >= sizeof(md->sha1.buf) || hash == NULL) {
		return ERR_ARG_FAIL;
	}

/*
	increase the length of the message
 */
#ifdef HAVE_NATIVE_INT64
	md->sha1.length += md->sha1.curlen << 3;
#else
	n = (md->sha1.lengthLo + (md->sha1.curlen << 3)) & 0xFFFFFFFFL;
	if (n < md->sha1.lengthLo) {
		md->sha1.lengthHi++;
	}
	md->sha1.lengthHi += (md->sha1.curlen >> 29);
	md->sha1.lengthLo = n;
#endif /* HAVE_NATIVE_INT64 */

/*
	append the '1' bit
 */
	md->sha1.buf[md->sha1.curlen++] = (unsigned char)0x80;

/*
	if the length is currently above 56 bytes we append zeros then compress.
	Then we can fall back to padding zeros and length encoding like normal.
 */
	if (md->sha1.curlen > 56) {
		while (md->sha1.curlen < 64) {
			md->sha1.buf[md->sha1.curlen++] = (unsigned char)0;
		}
		hd_sha1_compress(md);
		md->sha1.curlen = 0;
	}

/*
	pad upto 56 bytes of zeroes
 */
	while (md->sha1.curlen < 56) {
		md->sha1.buf[md->sha1.curlen++] = (unsigned char)0;
	}

/*
	store length
 */
#ifdef HAVE_NATIVE_INT64
	STORE64H(md->sha1.length, md->sha1.buf+56);
#else
	STORE32H(md->sha1.lengthHi, md->sha1.buf+56);
	STORE32H(md->sha1.lengthLo, md->sha1.buf+60);
#endif /* HAVE_NATIVE_INT64 */
	hd_sha1_compress(md);

/*
	copy output
 */
	for (i = 0; i < 5; i++) {
		val = tls_reg_read32(HR_CRYPTO_SHA1_DIGEST0 + (4*i));
		STORE32H(val, hash+(4*i));
	}
	memset(md, 0x0, sizeof(hsDigestContext_t));
	return SHA1_HASH_SIZE;
}

static void hd_md5_compress(hsDigestContext_t *md)
{
	unsigned int sec_cfg, val, i;
	tls_reg_write32(HR_CRYPTO_SRC_ADDR, (unsigned int)md->md5.buf);
	sec_cfg = (CRYPTO_METHOD_MD5 << 16) |  (64 & 0xFFFF);
	tls_reg_write32(HR_CRYPTO_SEC_CFG, sec_cfg);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST0, md->md5.state[0]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST1, md->md5.state[1]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST2, md->md5.state[2]);
	tls_reg_write32(HR_CRYPTO_SHA1_DIGEST3, md->md5.state[3]);
	crypto_complete = 0;
	tls_reg_write32(HR_CRYPTO_SEC_CTRL, 0x1);//start crypto
	while (!crypto_complete)
	{

	}
	crypto_complete = 0;
	for (i = 0; i < 4; i++) {
		val = tls_reg_read32(HR_CRYPTO_SHA1_DIGEST0 + (4*i));
		md->md5.state[i] = val;
	}
}

 
/**
 * @brief			This function initializes Message-Diggest context for usage in MD5 algorithm, starts a new MD5 operation and writes a new Digest Context. 
 *				This function begins a MD5 Message-Diggest Algorithm, i.e. fills the hsDigestContext_t structure pointed to by md with necessary data. 
 *				MD5 is the algorithm which takes as input a message of arbitrary length and produces as output a 128-bit "fingerprint" or "message digest" of the input.
 *				It is conjectured that it is computationally infeasible to produce two messages having the same message digest,
 *				or to produce any message having a given prespecified target message digest.
 *
 * @param[in]		md		MD5 Digest Context. 
 *
 * @return		None
 *
 * @note			None
 */
void tls_crypto_md5_init(hsDigestContext_t * md) {
	md->md5.state[0] = 0x67452301UL;
	md->md5.state[1] = 0xefcdab89UL;
	md->md5.state[2] = 0x98badcfeUL;
	md->md5.state[3] = 0x10325476UL;
	md->md5.curlen = 0;
#ifdef HAVE_NATIVE_INT64
	md->md5.length = 0;
#else
	md->md5.lengthHi = 0;
	md->md5.lengthLo = 0;
#endif /* HAVE_NATIVE_INT64 */
}


/**
 * @brief			Process a message block using MD5 algorithm.
 *				This function performs a MD5 block update operation. It continues an MD5 message-digest operation,
 *				by processing InputLen-byte length message block pointed to by buf, and by updating the MD5 context pointed to by md. 
 *				This function may be called as many times as necessary, so the message may be processed in blocks.
 *
 * @param[in]		md		MD5 Digest Context. 
 * @param[in]  	buf 		InputLen-byte length message block
 * @param[in]  	len 		The buf 's length 
 *
 * @return		None
 *
 * @note			None
 */
void tls_crypto_md5_update(hsDigestContext_t *md, const unsigned char *buf, u32 len)
{
	u32 n;

	while (len > 0) {
		n = min(len, (64 - md->md5.curlen));
		memcpy(md->md5.buf + md->md5.curlen, buf, (size_t)n);
		md->md5.curlen	+= n;
		buf				+= n;
		len				-= n;

/*
		is 64 bytes full?
 */
		if (md->md5.curlen == 64) {
			hd_md5_compress(md);
#ifdef HAVE_NATIVE_INT64
			md->md5.length += 512;
#else
			n = (md->md5.lengthLo + 512) & 0xFFFFFFFFL;
			if (n < md->md5.lengthLo) {
				md->md5.lengthHi++;
			}
			md->md5.lengthLo = n;
#endif /* HAVE_NATIVE_INT64 */
			md->md5.curlen = 0;
		}
	}
}

/**
 * @brief			This function ends a MD5 operation and produces a Message-Digest.
 *				This function finalizes MD5 algorithm, i.e. ends an MD5 Message-Digest operation, 
 *				writing the Message-Digest in the 16-byte buffer pointed to by hash in according to the information stored in context. 
 *
 * @param[in]		md		MD5 Digest Context. 
 * @param[in]		hash 	the Message-Digest
 *
 * @retval  		16  		success, return the hash size.
 * @retval  		<0   	failed  
 *
 * @note			None
 */
s32 tls_crypto_md5_final(hsDigestContext_t * md, unsigned char *hash)
{
	s32 i;
	u32 val;
#ifndef HAVE_NATIVE_INT64
	u32	n;
#endif

//	psAssert(md != NULL);
	if (hash == NULL) {
		CRYPTO_LOG("NULL hash storage passed to psMd5Final\n");
		return PS_ARG_FAIL;
	}

/*
	increase the length of the message
 */
#ifdef HAVE_NATIVE_INT64
	md->md5.length += md->md5.curlen << 3;
#else
	n = (md->md5.lengthLo + (md->md5.curlen << 3)) & 0xFFFFFFFFL;
	if (n < md->md5.lengthLo) {
		md->md5.lengthHi++;
	}
	md->md5.lengthHi += (md->md5.curlen >> 29);
	md->md5.lengthLo = n;
#endif /* HAVE_NATIVE_INT64 */

/*
	append the '1' bit
 */
	md->md5.buf[md->md5.curlen++] = (unsigned char)0x80;

/*
	if the length is currently above 56 bytes we append zeros then compress.
	Then we can fall back to padding zeros and length encoding like normal.
 */
	if (md->md5.curlen > 56) {
		while (md->md5.curlen < 64) {
			md->md5.buf[md->md5.curlen++] = (unsigned char)0;
		}
		hd_md5_compress(md);
		md->md5.curlen = 0;
	}

/*
	pad upto 56 bytes of zeroes
 */
	while (md->md5.curlen < 56) {
		md->md5.buf[md->md5.curlen++] = (unsigned char)0;
	}
/*
	store length
 */
#ifdef HAVE_NATIVE_INT64
	STORE64L(md->md5.length, md->md5.buf+56);
#else
	STORE32L(md->md5.lengthLo, md->md5.buf+56);
	STORE32L(md->md5.lengthHi, md->md5.buf+60);
#endif /* HAVE_NATIVE_INT64 */
	hd_md5_compress(md);

/*
	copy output
 */
	for (i = 0; i < 4; i++) {
		val = tls_reg_read32(HR_CRYPTO_SHA1_DIGEST0 + (4*i));
		STORE32L(val, hash+(4*i));
	}
	memset(md, 0x0, sizeof(hsDigestContext_t));
	return MD5_HASH_SIZE;
}

static void rsaMonMulSetLen(const u32 len)
{
    RSAN = len;
}
static void rsaMonMulWriteMc(const u32 mc)
{
	u32 val = 0;
    RSAMC = mc;
	val = RSAMC;
	if(val == mc)
	{
		val = 1;
		return;
	}
}
static void rsaMonMulWriteA(const u32 *const in)
{
    memcpy((u32*)&RSAXBUF, in, RSAN * sizeof(u32));
}
static void rsaMonMulWriteB(const u32 *const in)
{
    memcpy((u32*)&RSAYBUF, in, RSAN * sizeof(u32));
}
static void rsaMonMulWriteM(const u32 *const in)
{
    memcpy((u32*)&RSAMBUF, in, RSAN * sizeof(u32));
}
static void rsaMonMulReadA(u32 *const in)
{
    memcpy(in, (u32*)&RSAXBUF, RSAN * sizeof(u32));
}
static void rsaMonMulReadB(u32 *const in)
{
    memcpy(in, (u32*)&RSAYBUF, RSAN * sizeof(u32));
}
static void rsaMonMulReadD(u32 *const in)
{
    memcpy(in, (u32*)&RSADBUF, RSAN * sizeof(u32));
}
static int rsaMulModRead(unsigned char w, hstm_int * a)
{
	u32 in[64];
	int err = 0;
	memset(in, 0, 64 * sizeof(u32));
	switch(w)
	{
		case 'A':
			rsaMonMulReadA(in);
			break;
		case 'B':
			rsaMonMulReadB(in);
			break;
		case 'D':
			rsaMonMulReadD(in);
			break;
	}
	pstm_reverse((unsigned char *)in, RSAN * sizeof(u32));
	/* this a should be initialized outside. */
	//if ((err = pstm_init_for_read_unsigned_bin(NULL, a, RSAN * sizeof(u32) + sizeof(hstm_int))) != ERR_CRY_OK){
	//	return err;
	//}
	if ((err = pstm_read_unsigned_bin(a, (unsigned char *)in, RSAN * sizeof(u32))) != ERR_CRY_OK) {
		pstm_clear(a);
		return err;
	}
	return 0;
}
static void rsaMulModWrite(unsigned char w, hstm_int * a)
{
	u32 in[64];
	memset(in, 0, 64 * sizeof(u32));
	pstm_to_unsigned_bin_nr(NULL, a, (unsigned char*)in);
	switch(w)
	{
		case 'A':
			rsaMonMulWriteA(in);
			break;
		case 'B':
			rsaMonMulWriteB(in);
			break;
		case 'M':
			rsaMonMulWriteM(in);
			break;
	}
}
static void rsaMonMulAA(void)
{
    RSACON = 0x2c;

    while (!crypto_complete)
    {

    }
    crypto_complete = 0;
}
static void rsaMonMulDD(void)
{
    RSACON = 0x20;

    while (!crypto_complete)
    {

    }
    crypto_complete = 0;
}
static void rsaMonMulAB(void)
{
    RSACON = 0x24;

    while (!crypto_complete)
    {

    }
    crypto_complete = 0;
}
static void rsaMonMulBD(void)
{
    RSACON = 0x28;

    while (!crypto_complete)
    {

    }
    crypto_complete = 0;
}
/******************************************************************************
compute mc, s.t. mc * in = 0xffffffff
******************************************************************************/
static void rsaCalMc(u32 *mc, const u32 in)
{
    u32 y = 1;
	u32 i = 31;
	u32 left = 1;
	u32 right = 0;
    for(i = 31; i != 0; i--)
	{
		left <<= 1;										/* 2^(i-1) */
		right = (in * y) & left;                        /* (n*y) mod 2^i */
		if( right )
		{
			y += left;
		}
	}
    *mc =  ~y + 1;
}


/**
 * @brief			This function implements the large module power multiplication algorithm.
 *				res = a**e (mod n)  
 *
 * @param[in]		a 		Pointer to a bignumber. 
 * @param[in]		e 		Pointer to a bignumber.
 * @param[in]  	n 		Pointer to a bignumber.
 * @param[out]  	res 		Pointer to the result bignumber.
 *
 * @retval  		0  		success 
 * @retval  		other   	failed  
 *
 * @note			None
 */
int tls_crypto_exptmod(hstm_int *a, hstm_int *e, hstm_int *n, hstm_int *res)
{
	int i = 0;
	u32 k = 0, mc = 0, dp0;
	u8 monmulFlag = 0;
	hstm_int R, X, Y;

#ifndef TLS_CONFIG_FPGA
	tls_fls_sem_lock();
#endif
	pstm_init(NULL, &X);
	pstm_init(NULL, &Y);
	pstm_init(NULL, &R);
	k = pstm_count_bits(n);//n->used * DIGIT_BIT;//pstm_count_bits(n);
	k = ((k / 32) + (k % 32 > 0 ? 1 : 0)) * 32;
#if 0
	pstm_set(&Y, k);
	pstm_set(&X, 2);
	pstm_exptmod(NULL, &X, &Y, n, &R); //R = 2^k % n
#else
	pstm_2expt(&X, (int16)k); //X = 2^k
	pstm_mod(NULL, &X, n, &R); //R = 2^k % n
#endif
	//pstm_set(&Y, 1);
	pstm_mulmod(NULL, a, &R, n, &X); //X = A * R
	pstm_copy(&R, &Y);
	if(n->used > 1)
	{
#if (DIGIT_BIT < 32)
		dp0 = 0xFFFFFFFF & ((n->dp[0]) | (u32)(n->dp[1] << DIGIT_BIT));
#else
		dp0 = (n->dp[0]);
#endif
	}
	else
		dp0 = n->dp[0];
	rsaCalMc(&mc, dp0);
	k = pstm_count_bits(n);
	rsaMonMulSetLen(k/32 + (k%32 == 0 ? 0 : 1));
	rsaMonMulWriteMc(mc);
	rsaMulModWrite('M', n);
	rsaMulModWrite('B', &X);
	rsaMulModWrite('A', &Y);
	k = pstm_count_bits(e);
	for(i=k-1;i>=0;i--){
		//montMulMod(&Y, &Y, n, &Y);
		//if(pstm_get_bit(e, i))
		//	montMulMod(&Y, &X, n, &Y);
            if(monmulFlag == 0)
            {
                rsaMonMulAA();
                monmulFlag = 1;
            }
            else
            {
                rsaMonMulDD();
                monmulFlag = 0;
            }

            if(pstm_get_bit(e, i))
            {
                if(monmulFlag == 0)
                {
                    rsaMonMulAB();
                    monmulFlag = 1;
                }
                else
                {
                    rsaMonMulBD();
                    monmulFlag = 0;
                }
            }
	}
	pstm_set(&R, 1);
	rsaMulModWrite('B', &R);
	//montMulMod(&Y, &R, n, res);
	if(monmulFlag == 0)
	{
	    rsaMonMulAB();
	    rsaMulModRead('D', res);
	}
	else
	{
	    rsaMonMulBD();
	    rsaMulModRead('A', res);
	}
	pstm_clamp(res);
	pstm_clear(&X);
	pstm_clear(&Y);
	pstm_clear(&R);
#ifndef TLS_CONFIG_FPGA
	tls_fls_sem_unlock();
#endif

	return 0;
}



/**
 * @brief			This function initializes the encryption module.
 *
 * @param		None
 *
 * @return  		None
 *
 * @note			None
 */
void tls_crypto_init(void)
{
	tls_irq_enable(RSA_IRQn);
	tls_irq_enable(CRYPTION_IRQn);
}


