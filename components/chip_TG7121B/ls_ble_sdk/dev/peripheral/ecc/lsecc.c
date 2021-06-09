#include "string.h"
#include "stdlib.h"
#include "le501x.h"
#include "stdbool.h"
#include "field_manipulate.h"
#include "reg_lsecc.h"
#include "reg_rcc.h"
#include "lsecc.h"

#define MAX_OCTETS256  36
#define MAX_DIGITS_256  72 // ( (9+9) * 4 )
#define ELEMENTS_BIG_HEX256  (MAX_OCTETS256/4)  // 9
#define HIGHEST_INDEX_BIG_HEX256  (MAX_OCTETS256/4 )- 1 // 8

typedef struct bigHex256
{
    u_int32 num[MAX_OCTETS256/4];
    u_int32  len;
    u_int32  sign;
} bigHex256;

typedef struct veryBigHex256
{
    u_int32 num[MAX_OCTETS256/2];
    u_int32  len;
    u_int32  sign;
} veryBigHex256;

typedef struct ECC_Point256
{
    bigHex256 x;
    bigHex256 y;
} ECC_Point256;

typedef struct ECC_Jacobian_Point256
{
    bigHex256 x;
    bigHex256 y;
    bigHex256 z;
} ECC_Jacobian_Point256;

struct ecc_elt_tag
{
    u_int32 Point_Mul_Word256;                      

    // ECC points Q and R in Jacobian format.
    ECC_Jacobian_Point256 Jacobian_PointQ256;     
    ECC_Jacobian_Point256 Jacobian_PointR256;     

    // The Private/Secret Key used during calculations
    bigHex256 Pk256;
};

// Debug Public Keys - used when SSP debug mode is enabled - LSB first
const u_int8 DebugE256PublicKey_x[32] = {0xE6,0x9D,0x35,0x0E,0x48,0x01,0x03,0xCC,0xDB,0xFD,0xF4,0xAC,0x11,0x91,0xF4,0xEF,0xB9,0xA5,0xF9,0xE9,0xA7,0x83,0x2C,0x5E,0x2C,0xBE,0x97,0xF2,0xD2,0x03,0xB0,0x20};
const u_int8 DebugE256PublicKey_y[32] = {0x8B,0xD2,0x89,0x15,0xD0,0x8E,0x1C,0x74,0x24,0x30,0xED,0x8F,0xC2,0x45,0x63,0x76,0x5C,0x15,0x52,0x5A,0xBF,0x9A,0x32,0x63,0x6D,0xEB,0x2A,0x65,0x49,0x9C,0x80,0xDC};
// Debug Private Key - used when SSP debug mode is enabled - LSB first
const u_int8 DebugE256SecretKey0[32] =   {0xBD,0x1A,0x3C,0xCD,0xA6,0xB8,0x99,0x58,0x99,0xB7,0x40,0xEB,0x7B,0x60,0xFF,0x4A,0x50,0x3F,0x10,0xD2,0xE3,0xB3,0xC9,0x74,0x38,0x5F,0xC5,0xA3,0xD4,0xF6,0x49,0x3F};

/*********************************************************************************
 *  The length of P256 numbers stored.
 *  P256 numbers are stored in uint_32 arrays, with one more element than required.
 *  The extra element is used to store overflow's and thus simplify addition, subtraction
 *
 *********************************************************************************/
bool FirstRun = 1;													
const uint8_t DatLen = 32;			
const uint32_t t[8] = {0,8,16,24,32,40,48,56};  																											 
																													 
bigHex256 *GX1;
bigHex256 *GX2;
bigHex256 *GY1;
bigHex256 *GY2;
bigHex256 *GC;																											 
bigHex256 *CoZ;	

void setBigNumberLength256( bigHex256 *BigHexResult);
void copyBigHex256(const bigHex256 *source,bigHex256 *destination);
void GF_Setup_Jacobian_Infinity_Point256(ECC_Jacobian_Point256 *infinity);
void GF_Affine_To_Jacobian_Point_Copy256(const ECC_Point256 *source,ECC_Jacobian_Point256 *destination);
void GF_Jacobian_Point_Copy256(const ECC_Jacobian_Point256 *source,ECC_Jacobian_Point256 *destination);

void copyBigHex256(const bigHex256 *source, bigHex256 *destination)
{
    memcpy(destination->num,source->num,sizeof(destination->num)); //for P256 max_octets = 36

    destination->len = source->len;
    destination->sign = source->sign;
}

void setBigNumberLength256(bigHex256 *BigHexResult)
{
    int i;

    for(i=0;i<(ELEMENTS_BIG_HEX256);i++)
    {
        if(BigHexResult->num[i] != 0x00)
           break;
    }
    BigHexResult->len = (ELEMENTS_BIG_HEX256)-i;
}

 void GF_Jacobian_Point_Copy256(const ECC_Jacobian_Point256 *source,ECC_Jacobian_Point256 *destination)
{
    copyBigHex256(&source->x,&destination->x);
    copyBigHex256(&source->y,&destination->y);
    copyBigHex256(&source->z,&destination->z);
}

 void GF_Affine_To_Jacobian_Point_Copy256(const ECC_Point256 *source,ECC_Jacobian_Point256 *destination)
{
    bigHex256 BigHex_1;

    BigHex_1.len = 0x01;
    BigHex_1.sign = 0; // Positive Number
    memset(BigHex_1.num,0, sizeof(BigHex_1.num) );
    BigHex_1.num[HIGHEST_INDEX_BIG_HEX256] = 0x01;
    copyBigHex256(&source->x,&destination->x);
    copyBigHex256(&source->y,&destination->y);
    copyBigHex256(&BigHex_1,&destination->z);
}

void GF_Setup_Jacobian_Infinity_Point256(ECC_Jacobian_Point256 *infinity)
{
    bigHex256 BigHex_1;

    BigHex_1.len = 0x01;
    BigHex_1.sign = 0; // Positive Number
    memset(BigHex_1.num,0, sizeof(BigHex_1.num) );
    BigHex_1.num[HIGHEST_INDEX_BIG_HEX256] = 0x00;
    copyBigHex256(&BigHex_1,&infinity->x);
    BigHex_1.num[HIGHEST_INDEX_BIG_HEX256] = 0x01;
    copyBigHex256(&BigHex_1,&infinity->y);
    BigHex_1.num[HIGHEST_INDEX_BIG_HEX256] = 0x00;
    copyBigHex256(&BigHex_1,&infinity->z);

}

int CoZ_GF_Jacobian_DBLU256_HW(bool remap)
{
	if(remap) 
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xE8000000);
	else
         REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xC8000000);

     while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
	return 1;
}

int CoZ_GF_Jacobian_ZADDC256_HW(bool remap)
{
	if(remap) 
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xF0000000);
	else
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xD0000000);

	while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
	return 1;
}

int CoZ_GF_Jacobian_ZACAU256_HW(bool remap)
{
  if(remap)
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xE0000000);
	else
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xC0000000); 
	
    while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
	return 1;
}

 int CoZ_GF_Jacobian_ZADDU256_HW(bool remap)
{
	if(remap) 
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xF8000000);
	else
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0xD8000000);
	
     while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
	return 1;
}

void bigHexInversion256_HW()
{
   REG_FIELD_WR(LSECC->ARAM[7+t[0]], ECC_ARAMBIT, 0XFFFFFFFF);
   REG_FIELD_WR(LSECC->ARAM[6+t[0]], ECC_ARAMBIT, 0X00000001);
   REG_FIELD_WR(LSECC->ARAM[5+t[0]], ECC_ARAMBIT, 0X00000000);
   REG_FIELD_WR(LSECC->ARAM[4+t[0]], ECC_ARAMBIT, 0X00000000);
   REG_FIELD_WR(LSECC->ARAM[3+t[0]], ECC_ARAMBIT, 0X00000000);
   REG_FIELD_WR(LSECC->ARAM[2+t[0]], ECC_ARAMBIT, 0XFFFFFFFF);
   REG_FIELD_WR(LSECC->ARAM[1+t[0]], ECC_ARAMBIT, 0XFFFFFFFF);
   REG_FIELD_WR(LSECC->ARAM[0+t[0]], ECC_ARAMBIT, 0XFFFFFFFF);

	//A
   REG_FIELD_WR(LSECC->ARAM[7+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[6+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[5+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[4+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[3+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[2+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[1+t[6]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[0+t[6]], ECC_ARAMBIT, 1);
	//C
   REG_FIELD_WR(LSECC->ARAM[7+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[6+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[5+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[4+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[3+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[2+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[1+t[7]], ECC_ARAMBIT, 0);
   REG_FIELD_WR(LSECC->ARAM[0+t[7]], ECC_ARAMBIT, 0);
	
   REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X80000038);

   while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
}

//bool CoZM_ecc_pm_ZACAU_uint8_256(void)
bool CoZM_ecc_pm_ZACAU_uint8_256(struct ecc_elt_tag* ecc_elt)
{
	if(ecc_elt->Point_Mul_Word256 > 0)
	{
		uint32_t j;
		uint32_t bitVal;
		uint8_t  byte=0;
		uint8_t  byteOffset;

		//kn-2...k1
		bitVal = ecc_elt->Pk256.num[(ecc_elt->Point_Mul_Word256/4)];
		byteOffset = (ecc_elt->Point_Mul_Word256)%4;
			
		switch(byteOffset){
			case 3:
					byte = bitVal & 0xFF;
					break;
			case 2:
					byte = (bitVal & 0xFF00)>>8;
					break;
			case 1:
					byte = (bitVal & 0xFF0000)>>16;
					break;
			case 0:
					byte = (bitVal & 0xFF000000)>>24;
					break;
			default:
					break;
		}		
		
		for (j=0;j<8;j++)
		{ 					
			if((ecc_elt->Point_Mul_Word256 == (DatLen+3))&&(j==7))
				continue;
			bitVal = (byte >> (7 - j)) & 0x0001;
			
			if(FirstRun){
				if(!bitVal){
					continue;
				}
				else{
					FirstRun = 0;
					continue;
				}
			}
			
			if (bitVal)
			 CoZ_GF_Jacobian_ZACAU256_HW(1);						
			else
			 CoZ_GF_Jacobian_ZACAU256_HW(0);
		}
    // Increment the counter
	  ecc_elt->Point_Mul_Word256++;
	}
	return (ecc_elt->Point_Mul_Word256 == (DatLen + 4));
}

void  ecc_calc_start(const u_int8* secret_key, const u_int8* public_key_x, const u_int8* public_key_y, uint8_t* result_x,uint8_t* result_y,void (*cb)(void *),void *param)
{
    u_int32 big_num_offset=1;
    int32_t i, j;
	bool remap;  
    bigHex256 PrivateKey256;
    ECC_Point256 PublicKey256;

    // Now Copy the Public Key and Secret Key coordinates to ECC point format.
    PrivateKey256.num[0] = 0;
    PublicKey256.x.num[0] = 0;
    PublicKey256.y.num[0] = 0;

     for (i=31, j=big_num_offset;i>=0;) // Public and Private Keys Are LSB - make it in MSB
    {
         PrivateKey256.num[j] = (u_int32)
                                            ((((*(secret_key+i    )) << 24) & 0xFF000000) +
                                           (((*(secret_key+(i-1))) << 16) & 0x00FF0000) +
                                           (((*(secret_key+(i-2))) <<  8) & 0x0000FF00) +
                                           (( *(secret_key+(i-3)))        & 0x000000FF));

        PublicKey256.x.num[j] = (u_int32)
                                            ((((*(public_key_x+i    )) << 24) & 0xFF000000) +
                                           (((*(public_key_x+(i-1))) << 16) & 0x00FF0000) +
                                           (((*(public_key_x+(i-2))) <<  8) & 0x0000FF00) +
                                           (( *(public_key_x+(i-3)))        & 0x000000FF));

        PublicKey256.y.num[j] = (u_int32)
                                           ((((*(public_key_y+i    )) << 24) & 0xFF000000) +
                                           (((*(public_key_y+(i-1))) << 16) & 0x00FF0000) +
                                           (((*(public_key_y+(i-2))) <<  8) & 0x0000FF00) +
                                           (( *(public_key_y+(i-3)))        & 0x000000FF));
        i -= 4;
        j++;
    }

    setBigNumberLength256(&PrivateKey256);
    setBigNumberLength256(&PublicKey256.x);
    setBigNumberLength256(&PublicKey256.y);
    PublicKey256.x.sign = 0;
    PublicKey256.y.sign = 0;

     
    ECC_Jacobian_Point256 PointP_Jacobian;
    ECC_Jacobian_Point256* pPointP_Jacobian = &PointP_Jacobian;
    // Allocate Memory for Jacobian Points P, Q, R
 //   struct ecc_elt_tag* ecc_elt = (struct ecc_elt_tag*)malloc(sizeof(struct ecc_elt_tag));
    uint8_t ecc_elt_buffer[400]={0};
    struct ecc_elt_tag* ecc_elt = (struct ecc_elt_tag*)(&ecc_elt_buffer[0]);

    // Need to map from Affine Point to Jacobian Point
    GF_Affine_To_Jacobian_Point_Copy256(&PublicKey256, pPointP_Jacobian);
    GF_Jacobian_Point_Copy256(pPointP_Jacobian, &(ecc_elt->Jacobian_PointR256));
    copyBigHex256(&PrivateKey256, &(ecc_elt->Pk256));
    GF_Setup_Jacobian_Infinity_Point256(&(ecc_elt->Jacobian_PointQ256));

     ecc_elt->Point_Mul_Word256 = 4;
		GX1 = &(&(ecc_elt->Jacobian_PointR256))->x;
	    GY1 = &(&(ecc_elt->Jacobian_PointR256))->y;
		GX2 = &(&(ecc_elt->Jacobian_PointQ256))->x;
		GY2 = &(&(ecc_elt->Jacobian_PointQ256))->y;   
		GC  = &(&(ecc_elt->Jacobian_PointR256))->z;
		CoZ = &(&(ecc_elt->Jacobian_PointQ256))->z;
		
		for(j = 0; j < 8; j++)
		{
            REG_FIELD_WR(LSECC->ARAM[(7-j)+0], ECC_ARAMBIT,  (PublicKey256.x).num[j+1]);
            REG_FIELD_WR(LSECC->ARAM[(7-j)+8], ECC_ARAMBIT,   GX1->num[j+1]);
            REG_FIELD_WR(LSECC->ARAM[(7-j)+16], ECC_ARAMBIT,  GY1->num[j+1]);
            REG_FIELD_WR(LSECC->ARAM[(7-j)+24], ECC_ARAMBIT,  GC->num[j+1]);
            REG_FIELD_WR(LSECC->ARAM[(7-j)+32], ECC_ARAMBIT,  GX2->num[j+1]);
            REG_FIELD_WR(LSECC->ARAM[(7-j)+40], ECC_ARAMBIT,  GY2->num[j+1]);
            REG_FIELD_WR(LSECC->ARAM[(7-j)+48], ECC_ARAMBIT,  0);


	    }		
         REG_FIELD_WR(LSECC->ARAM[48], ECC_ARAMBIT, 3);
		 

        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000000C);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000009C);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

         REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000012C);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X800001BC);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000024C);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X800002DC);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
		 
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000036C);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

		 CoZ_GF_Jacobian_DBLU256_HW(0);
			 
		FirstRun = 1;

        if (1)
		 {
		   while (!CoZM_ecc_pm_ZACAU_uint8_256(ecc_elt)); 
		 }
	   else
		 {
		    CoZM_ecc_pm_ZACAU_uint8_256(ecc_elt); 
		 }
		 
		 if(ecc_elt->Pk256.num[8] & 0x1)
		 {
		  remap = 1;
		 }
	   else
		 {
		  remap = 0;
		 }
		 
		 CoZ_GF_Jacobian_ZADDC256_HW(remap);
		 
        REG_FIELD_WR(LSECC->CON, ECC_CONBIT, 0X800001BA);
        while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
      
		bigHexInversion256_HW();	 

        REG_FIELD_WR(LSECC->ARAM[63], ECC_ARAMBIT, 0X4);
        REG_FIELD_WR(LSECC->ARAM[62], ECC_ARAMBIT, 0Xfffffffd);
        REG_FIELD_WR(LSECC->ARAM[61], ECC_ARAMBIT, 0Xffffffff);
        REG_FIELD_WR(LSECC->ARAM[60], ECC_ARAMBIT, 0Xfffffffe);
        REG_FIELD_WR(LSECC->ARAM[59], ECC_ARAMBIT, 0Xfffffffb);
        REG_FIELD_WR(LSECC->ARAM[58], ECC_ARAMBIT, 0Xffffffff);
        REG_FIELD_WR(LSECC->ARAM[57], ECC_ARAMBIT, 0X00000000);
        REG_FIELD_WR(LSECC->ARAM[56], ECC_ARAMBIT, 0X00000003);

      REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X800001BC);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

	  for(j = 0; j < 8; j++)
	  {
          REG_FIELD_WR(LSECC->ARAM[(7-j)+0], ECC_ARAMBIT, (PublicKey256.y).num[j+1]);	   
	  }			

      REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000000C);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

	  CoZ_GF_Jacobian_ZADDU256_HW(remap);
      REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000009A);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

      REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000012A);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

      REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X800001BA);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

     REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X8000024A);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}

      REG_FIELD_WR(LSECC->CON, ECC_CONBIT,0X800002DA);
      while (REG_FIELD_RD(LSECC->STAT,ECC_DONE) == 0){;}
	  
	  for(j=0;j<8;j++)
	  {
	  	GX2->num[j+1] = REG_FIELD_RD(LSECC->ARAM[(7-j)+8],ECC_ARAMBIT); 
	  	GY2->num[j+1] = REG_FIELD_RD(LSECC->ARAM[(7-j)+16],ECC_ARAMBIT);   
	  	GC->num[j+1]  = REG_FIELD_RD(LSECC->ARAM[(7-j)+24],ECC_ARAMBIT);   
	  	GX1->num[j+1] = REG_FIELD_RD(LSECC->ARAM[(7-j)+32],ECC_ARAMBIT); 
	  	GY1->num[j+1] = REG_FIELD_RD(LSECC->ARAM[(7-j)+40],ECC_ARAMBIT);   
	  }	 


	  for(i = 31, j = 1; i >= 0;) // Keys Res is MSB - make it in LSB
	  {				
	  		result_x[i]   = (((ecc_elt->Jacobian_PointQ256).x.num[j] & 0xFF000000) >> 24);
	  		result_x[i-1] = (((ecc_elt->Jacobian_PointQ256).x.num[j] & 0x00FF0000) >> 16);
	  		result_x[i-2] = (((ecc_elt->Jacobian_PointQ256).x.num[j] & 0x0000FF00) >> 8);
	  		result_x[i-3] = ( (ecc_elt->Jacobian_PointQ256).x.num[j] & 0x000000FF);
	  		i-=4;
	  		j++;
	  }
	     
	   // Copy result keys Y coordinate
	   
		 for(i = 31, j=1; i>=0;)
		 { // Keys Res is MSB - make it in LSB
				result_y[i]   = (((ecc_elt->Jacobian_PointQ256).y.num[j] & 0xFF000000) >> 24);
				result_y[i-1] = (((ecc_elt->Jacobian_PointQ256).y.num[j] & 0x00FF0000) >> 16);
				result_y[i-2] = (((ecc_elt->Jacobian_PointQ256).y.num[j] & 0x0000FF00) >> 8);
				result_y[i-3] = ( (ecc_elt->Jacobian_PointQ256).y.num[j] & 0x000000FF);
				i-=4;
				j++;
		 }  
    cb(param);
}    

void lsecc_init(void)
{		
    REG_FIELD_WR(RCC->APB1EN, RCC_ECC,1);   //enable ecc_clk

    REG_FIELD_WR(LSECC->ENG[0], ECC_ENGBIT, 0XFFFFFFFF);
    REG_FIELD_WR(LSECC->ENG[1], ECC_ENGBIT, 0XFFFFFFFF);
    REG_FIELD_WR(LSECC->ENG[2], ECC_ENGBIT, 0XFFFFFFFF);
    REG_FIELD_WR(LSECC->ENG[3], ECC_ENGBIT, 0X00000000);
    REG_FIELD_WR(LSECC->ENG[4], ECC_ENGBIT, 0X00000000);
    REG_FIELD_WR(LSECC->ENG[5], ECC_ENGBIT, 0X00000000);
    REG_FIELD_WR(LSECC->ENG[6], ECC_ENGBIT, 0X00000001);
    REG_FIELD_WR(LSECC->ENG[7], ECC_ENGBIT, 0XFFFFFFFF);

    REG_FIELD_WR(LSECC->CON, ECC_CONBIT, 0X80000000);      //Calculate H 
    while(!(REG_FIELD_RD(LSECC->STAT,ECC_DONE))){;}
} 


void lsecc_Deinit(void)
{		
    REG_FIELD_WR(RCC->APB1EN, RCC_ECC, 0);   //disable ecc_clk
} 

