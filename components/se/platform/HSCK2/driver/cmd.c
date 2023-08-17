#include <string.h>
#include "crc.h"
#include "spi.h"
#include "cmd.h"
#include "transmit.h"
#include "sm4.h"
#include <aos/kernel.h>

#ifndef NULL
#define NULL  ((void *)0)
#endif

#define  DELAY_1MS          (1600UL)
#define  DELAY_2MS  		(3200UL)
#define  DELAY_5MS  		(8000UL)
#define  DELAY_6us  		(6UL)
  
extern void vDelay(signed int  uiTimes);

//十六字节补位,补位后数据变成：len（1字节）+data+800000.。。
void data16padding(unsigned char *data, unsigned int len, unsigned char *result)
{
   unsigned int  i;
   result[0] = len;
   for (i = 1; i < (len+1) ; i++)
    {
        result[i] = data[i-1];
    }
    
   if((++len)%16 == 0) 
        {    
            *result = *data;
            return ;
        }
   else
        {
            result[len++]=0x80;
            while (len%16 != 0)
            {
                result[len++]=0x00;  
            };
        };   
}

//取16字节数的前四字节为mac
void sm4_mac(unsigned char *data, unsigned int len, unsigned char *result)
{
   unsigned int i, a;
   i = len/16;
   for (a=0; a < 4; a++)
    {
        result[a] = data[(i-1)*16+a];
    }
}



//-------------------------------------------------
/*
回二级boot
描述：
    将SE回到二级Boot状态下
参数:
    无
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------

SGD_RV Back_secondary_boot (void)
{
	SGD_UINT32 wRet;
	SGD_UINT8 aucBuf[16];
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[16] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	SGD_UINT8   ucVersion = 0;

	//获取SE状态
	aucBuf[0] = 0x80;
	aucBuf[1] = 0xFC;
	aucBuf[2] = 0x00;
	aucBuf[3] = 0x00;
	aucBuf[4] = 0x00;
	wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
	if(0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}			
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
	ucVersion = tmpOutBuf[0];
    
	//当前是app状态
	if(0x55 == ucVersion)
	{
			//回到BOOT2状态
			aucBuf[0] = 0x80;
			aucBuf[1] = 0xFC;
			aucBuf[2] = 0x00;
			aucBuf[3] = 0xFF;
			aucBuf[4] = 0x00;
            wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
			if(0x00 != wRet)
			{
				return SDR_COMMFAIL;
			}
			if ((tmpOutBuf == 0) || (ulTmpLen == 0))
			{
				return SDR_COMMFAIL;
			}			
			sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
			if (sw != 0x9000)
			{
				return SDR_EXECUTION_ERROR;
			}
           
            aos_msleep(5);   
            
            //获取cos状态
            aucBuf[0] = 0x80;
            aucBuf[1] = 0xFC;
            aucBuf[2] = 0x00;
            aucBuf[3] = 0x00;
            aucBuf[4] = 0x00;
            wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
            if(0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
            if ((tmpOutBuf == 0) || (ulTmpLen == 0))
            {
                return SDR_COMMFAIL;
            }			
            sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
            if (sw != 0x9000)
            {
                return SDR_EXECUTION_ERROR;
            }
            ucVersion = tmpOutBuf[0];   
                
	} 
    
    if(0x00 != ucVersion)
    {
        return SDR_EXECUTION_ERROR;
    }    
    
    return SDR_OK;
}
//-------------------------------------------------
/*
下载固件前的初始化
描述：
    不存在数据。该指令主要是将SE回到二级Boot状态下，并在cos内部做一些算法初始化，
    例如加载AES密钥值和IV值，以及HASH算法SHA256初始化。
参数:
    无
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_Initialization_before_downloading_firmware (void)
{
    
 	SGD_UINT32 wRet;
	SGD_UINT8 aucBuf[16];
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[16] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);

    //回二级boot
    wRet = Back_secondary_boot ();
    if(wRet != 0)
    {
        return SDR_COMMFAIL;
    } 
    
	//更新APP固件初始化
	aucBuf[0] = 0x80;
	aucBuf[1] = 0xFC;
	aucBuf[2] = 0x00;
	aucBuf[3] = 0x01;
	aucBuf[4] = 0x00;
    wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
	if(0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}			
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}   
    
    return SDR_OK;
    
}

//-------------------------------------------------
/*
下载固件
描述：
    数据域为固件内容。每次下载的固件内容是512字节的整数倍。
    所有的固件数据通过这一步进行下载。
    固件升级结束验签
参数:
    EncBin      输入，下载的固件内容，长度为512字节的整数倍  或者 512字节的整数倍+64字节
    EncBinLen   输入，下载的固件内容长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_downloading_firmware (const SGD_UCHAR *EncBin, SGD_UINT32 EncBinLen)
{
    if(EncBin == NULL)
    {
        return SDR_SPACE_ERROR;
    }
	SGD_UINT32 wRet;
	SGD_UINT8 aucBuf[600];
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[16] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    SGD_UINT32 ulSendLen = 0;
    SGD_UINT32 remainder = 0;
    
    remainder = EncBinLen % 512;
    if((remainder != 0x40) && (remainder != 0x00))
	{
		return SDR_UPDATEFM_LEN_ERROR;
	}
    
	while(EncBinLen >= 0x200)
    {
        aucBuf[0] = 0x80;
        aucBuf[1] = 0xFC;
        aucBuf[2] = 0x00;
        aucBuf[3] = 0x02;
        aucBuf[4] = 0x00;
        aucBuf[5] = 0x02;
        aucBuf[6] = 0x00;
 
        memcpy(aucBuf + 7, EncBin + ulSendLen, 0x200);
        wRet = Transmit(aucBuf, 519, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
        if(0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }			
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (sw != 0x9000)
        {
            return SDR_EXECUTION_ERROR;
        }
        EncBinLen -= 0x200;
		
		ulSendLen += 0x200;
    }
    
    
    if (remainder == 0x40)
    {
        //在boot2状态，验证签名，升级结束
        aucBuf[0] = 0x80;
        aucBuf[1] = 0xFC;
        aucBuf[2] = 0x00;
        aucBuf[3] = 0x03;
        aucBuf[4] = 0x00;
        aucBuf[5] = 0x00;
        aucBuf[6] = 0x40;
        memcpy(aucBuf + 7, EncBin + ulSendLen, 0x40);
        
        wRet = Transmit(aucBuf,71, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
        if(0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }			
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (sw != 0x9000)
        {
            return SDR_EXECUTION_ERROR;
        }
        //验签通过，设置APP存在标志，跳到APP中。需要等待2毫秒延迟。
        aos_msleep(2);
    }

    
    return SDR_OK;
}


//-------------------------------------------------
/*
激活APP
描述：
    激活APP。
参数:
    无
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_Activate_the_app (void)
{
  	SGD_UINT32 wRet;
	SGD_UINT8 aucBuf[10];
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	SGD_UINT8   ucVersion = 0;
      
	//激活APP
    aucBuf[0] = 0x80;
    aucBuf[1] = 0xFC;
    aucBuf[2] = 0x00;
    aucBuf[3] = 0x04;
    aucBuf[4] = 0x00;

    wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
    if(0x00 != wRet)
    {
        return SDR_COMMFAIL;
    }
    if ((tmpOutBuf == 0) || (ulTmpLen == 0))
    {
        return SDR_COMMFAIL;
    }			
    sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
    if (sw != 0x9000)
    {
        return SDR_EXECUTION_ERROR;
    }
    
    //获取cos状态

    aucBuf[0] = 0x80;
    aucBuf[1] = 0xFC;
    aucBuf[2] = 0x00;
    aucBuf[3] = 0x00;
    aucBuf[4] = 0x00;
    wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
    if(0x00 != wRet)
    {
        return SDR_COMMFAIL;
    }
    if ((tmpOutBuf == 0) || (ulTmpLen == 0))
    {
        return SDR_COMMFAIL;
    }			
    sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
    if (sw != 0x9000)
    {
        return SDR_EXECUTION_ERROR;
    }
    ucVersion = tmpOutBuf[0];   
          
    if(0x55 != ucVersion)
    {
         return SDR_TIMEOUT;
    }
        
    return SDR_OK; 
}

	
//-------------------------------------------------
/*
读取app固件版本号
描述：
    读取app固件版本号
参数:
     pOutVersion    输出，获取的版本号
	 pulRandLen     输出，获取的版本号长度，2字节
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_GetAppVersion( SGD_UINT8* pOutVersion, SGD_UINT32 *pulVersiondLen)
{
    if(pOutVersion == NULL)
    {
        return SDR_SPACE_ERROR;
    }
	SGD_UINT32 wRet;
	SGD_UINT8 aucBuf[5] = {0x80,0xFC,0x00,0x10,00};

	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[2] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);

    wRet = Transmit(aucBuf, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
	if(0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}			
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
	ulTmpLen -= 2;
	*pulVersiondLen = ulTmpLen;
	//提取版本号
	memmove(pOutVersion, tmpOutBuf, ulTmpLen);

	return SDR_OK;
}

//-------------------------------------------------
/*
获取随机数
描述：
    获取随机数
参数:
    Len                 输入，要获取的随机数长度，取值0x08、0x10
    pucRandom			输出，缓冲区指针，用于存放获取的随机数，分配空间不能小于16字节。
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV GetRandom(SGD_UINT32 Len, SGD_UINT8* pOutRand)
{
	SGD_UINT32 wRet;
	SGD_UINT8 gen_rand[5] = {0x00,0x84,0x00,0x00};
	gen_rand[4] = Len;

	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[1024] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);

    wRet = Transmit(gen_rand, 5, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
	if(0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}			
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
	ulTmpLen -= 2;
	
	//提取随机数
	memmove(pOutRand, tmpOutBuf, ulTmpLen);

	return SDR_OK;
}

//-------------------------------------------------
/*
获取任意长度随机数
描述：
    获取任意长度字节随机数
参数:
    Len                 输入，要获取的随机数长度
    data			    输出，缓冲区指针，用于存放获取的随机数，分配空间不能小于要获取的随机数长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_rng_get_bytes(SGD_UINT32 len, SGD_UCHAR *data)
{
    if(data == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    SGD_UINT8 pOutRand[16] = {0x00}; 
	SGD_UINT16 len1 = 0,len2 = 0; 
    
    len1 = len / 16;
    for (int i = 0; i< len1; i++)
    {
        GetRandom(16, &data[0]+i*16);
    }

    len2 = len % 16;
    GetRandom(16, &pOutRand[0]);
    memcpy(&data[0]+len1*16, &pOutRand[0], len2);
 
    
    return SDR_OK;
}



//-------------------------------------------------
/*
通信通道测试指令
描述：
? ? Spi通信测试
参数:
? ? 无
返回值:
? ? 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_SpiChannelTest(void)
{
    SGD_UINT16 wRet;
    SGD_UCHAR aucBuf[1029] = { 0 };
    SGD_UINT16 sw;
    SGD_UINT32 ulTmpLen = 0;

    hs_rng_get_bytes(1024, &aucBuf[5]);
    aucBuf[0] = 0x80;
    aucBuf[1] = 0x10;
    aucBuf[2] = 0x00;
    aucBuf[3] = 0x04; 
    aucBuf[4] = 0x00;

    wRet = Transmit(aucBuf, 1029, aucBuf, &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
    if (0x00 != wRet)
    {
        return SDR_COMMFAIL;
    }

    sw = aucBuf[ulTmpLen - 2] * 0x100 + aucBuf[ulTmpLen - 1];

    if(ulTmpLen == 0)
    {
        return SDR_EXECUTION_ERROR;
    }

    if (0x9000 != sw)
    {
        return SDR_COMMFAIL;
    }

    return SDR_OK;
}


/*
外部认证
描述：
    通过调用此接口获取相应的权限。
参数:
	rootkey		输入,16字节根密钥
返回值:
	返回0成功,非0表示失败
*/
SGD_RV hs_ExternalAuth(SGD_UCHAR* rootkey)
{
    if(rootkey == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    SGD_UCHAR pOutRand[16] = {0x00};
    sm4_context ctx;
    SGD_UCHAR pucAuthData[16] = {0x00};	
    
	SGD_UINT32 wRet = 0;
	SGD_UCHAR pSendBuf[32] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[2] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 
    
    //使用根密钥对随机数进行SM4 ECB模式加密运算
    sm4_setkey_enc(&ctx,rootkey);  
	sm4_crypt_ecb(&ctx,1,16,pOutRand,pucAuthData); 
    
    //外部认证
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x82;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x10;
	memcpy(pSendBuf + ulSendLen, pucAuthData, 16);
	ulSendLen += 16;
	 
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	 
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_EXECUTION_ERROR;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
    if (0x9000 != sw)
    {
        return SDR_EXECUTION_ERROR;
    }
		
    ulTmpLen -= 2;
		
    return SDR_OK;
}

//-------------------------------------------------
/*
导入根密钥
描述:
    用于修改根密钥，默认密钥类型为SM4
参数:
	rootkey			输入,16字节根密钥
	key		        输入,更新的16字节根密钥
    Len             输入，要导入的密钥长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV Import_rootkey(SGD_UCHAR *rootkey, SGD_UCHAR *Key, SGD_UINT32 Len)
{   
    if(rootkey == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    SGD_UINT32 wRet;
	SGD_UCHAR pSendBuf[50] = { 0 };
	SGD_UINT32 ulSendLen = 0;
    SGD_UINT16 sw;
    SGD_UINT8 tmpOutBuf[2] = {0};
    SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	SGD_UCHAR plaintext[50] = {0x00};
    SGD_UCHAR ciphetext[50] = {0x00};
    SGD_UCHAR pOutRand[16] = {0x00};
    SGD_UCHAR macdata[50] = {0x00};
    SGD_UCHAR mac[4] = {0x00};
 
    if (Len != 16)
    {
        return SDR_COMMOND_ERROR;
    }
    
    //十六字节补位，plaintext = 0x10 + newrootkey + 补位数据（补位时先补0x80，再补0x00至16字节整数倍）
    data16padding(Key, Len, plaintext);
    Len=Len+16;
    
    //采用sm4-ECB模式加密，获得密文ciphetext
    sm4_context ctx;
    sm4_setkey_enc(&ctx,rootkey);  
	sm4_crypt_ecb(&ctx,1,Len,plaintext,ciphetext); 
    
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 

    //将随机数作为初始向量，使用根密钥对“密文”进行SM4 CBC计算，
    sm4_setkey_enc(&ctx,rootkey);
	sm4_crypt_cbc(&ctx,1,Len,pOutRand, ciphetext, macdata); 
    
    //计算结果最后一组的前4字节，为mac
    sm4_mac(macdata, Len, mac);
    
	pSendBuf[ulSendLen++] = 0x04;
	pSendBuf[ulSendLen++] = 0xD2;
	pSendBuf[ulSendLen++] = 0x01; //0x01:SM4;  
	pSendBuf[ulSendLen++] = 0x00; //KID=00的SM4密钥表示根密钥。
	pSendBuf[ulSendLen++] = Len+4;
	memcpy(pSendBuf + ulSendLen, ciphetext, Len);
	ulSendLen += Len;
	memcpy(pSendBuf + ulSendLen, mac, 4);
	ulSendLen += 4;
	
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);

	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
	
	ulTmpLen -= 2;
	
	return SDR_OK;   
    
    
}

//-------------------------------------------------
/*
导入密钥加密密钥
描述:
    用于导入密钥加密密钥，默认密钥类型为SM4,对于已经存在的密钥，此命令直接覆盖旧密钥
参数:
    rootkey             输入,16字节的根密钥
	KeyIndex			输入,指定导入的密钥加密密钥密钥索引,此索引若存在密钥，直接覆盖旧密钥，取值范围0x01~0x10
    key                 输入,要导入的密钥加密密钥数据，默认为16字节的SM4密钥
    Len                 输入，要导入的密钥长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV Import_key_encrypting_key(SGD_UCHAR *rootkey, SGD_UINT32 KeyIndex,SGD_UCHAR *Key, SGD_UINT32 Len)
{
    if(rootkey == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    SGD_UINT32 wRet;
	SGD_UCHAR pSendBuf[50] = { 0 };
	SGD_UINT32 ulSendLen = 0;
    SGD_UINT16 sw;
    SGD_UINT8 tmpOutBuf[2] = {0};
    SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    SGD_UCHAR plaintext[50] = {0x00};
    SGD_UCHAR ciphetext[50] = {0x00};
    SGD_UCHAR pOutRand[16] = {0x00};
    SGD_UCHAR macdata[50] = {0x00};
    SGD_UCHAR mac[4] = {0x00};
    if (Len != 16)
    {
        return SDR_COMMOND_ERROR;
    }    
    //十六字节补位，plaintext = 0x10 + key_encrypting_key + 补位数据（补位时先补0x80，再补0x00至16字节整数倍）
    data16padding(Key, Len, plaintext);
    Len=Len+16;
    
    //采用sm4-ECB模式加密，获得密文ciphetext
    sm4_context ctx;
    sm4_setkey_enc(&ctx,rootkey);  
    sm4_crypt_ecb(&ctx,1,Len,plaintext,ciphetext); 
    
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 

    //将随机数作为初始向量，使用根密钥对“密文”进行SM4 CBC计算，
    sm4_setkey_enc(&ctx,rootkey);
    sm4_crypt_cbc(&ctx,1,Len,pOutRand, ciphetext, macdata); 
    
    //计算结果最后一组的前4字节，为mac
    sm4_mac(macdata, Len, mac);
    
    pSendBuf[ulSendLen++] = 0x04;
    pSendBuf[ulSendLen++] = 0xD2;
    pSendBuf[ulSendLen++] = 0x01; 
    pSendBuf[ulSendLen++] = KeyIndex;
    pSendBuf[ulSendLen++] = 0x24;
    memcpy(pSendBuf + ulSendLen, ciphetext, Len);
    ulSendLen += Len;
    memcpy(pSendBuf + ulSendLen, mac, 4);
    ulSendLen += 4;
   
   
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);

	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
	
	ulTmpLen -= 2;
	
	return SDR_OK;       
}


//-------------------------------------------------
/*
导入ECC密钥
描述:
    用于导入 ECC 密钥对
参数:
    key_encrypting_key_KeyIndex     输入，要使用的sm4密钥加密密钥索引
    key_encrypting_key              输入，要使用的sm4密钥加密密钥
	KeyIndex			            输入，要导入的ECC密钥索引
	Key			                    输入，要导入的ECC密钥,公钥（64 字节）+ 私钥（32 字节）
    Len                             输入，要导入的密钥长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV Import_ECCKey(SGD_UINT32 key_encrypting_key_KeyIndex, SGD_UCHAR *key_encrypting_key, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Len)
{
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    if(key_encrypting_key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[150] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    SGD_UCHAR plaintext[150] = {0x00};
    SGD_UCHAR ciphetext[150] = {0x00};
    SGD_UCHAR pOutRand[16] = {0x00};
    SGD_UCHAR macdata[150] = {0x00};
    SGD_UCHAR mac[4] = {0x00};
    if (Len != 96)
    {
        return SDR_COMMOND_ERROR;
    }    
    //十六字节补位，plaintext = 0x60 + Key + 补位数据（补位时先补0x80，再补0x00至16字节整数倍）
    data16padding(Key, Len, plaintext);
    plaintext[0] = KeyIndex; //plaintext =密钥ID（1字节）+ 公钥（64字节）+ 私钥（32字节）+ 补位数据（0x80...00）
    Len = Len+16;//补位后的明文长度
    
    //采用sm4-ECB模式加密，获得密文ciphetext
    sm4_context ctx;
    sm4_setkey_enc(&ctx,key_encrypting_key);  
    sm4_crypt_ecb(&ctx,1,Len,plaintext,ciphetext); 
    
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 

    //将随机数作为初始向量，使用根密钥对“密文”进行SM4 CBC计算，
    sm4_setkey_enc(&ctx,key_encrypting_key);
    sm4_crypt_cbc(&ctx,1,Len,pOutRand, ciphetext, macdata); 
    
    //计算结果最后一组的前4字节，为mac
    sm4_mac(macdata, Len, mac);
       
	pSendBuf[ulSendLen++] = 0x04;
	pSendBuf[ulSendLen++] = 0xC7;
	pSendBuf[ulSendLen++] = 0X01;
	pSendBuf[ulSendLen++] = key_encrypting_key_KeyIndex;
	pSendBuf[ulSendLen++] = Len + 4;
	memcpy(pSendBuf + ulSendLen, ciphetext, Len);
	ulSendLen += Len;
	memcpy(pSendBuf + ulSendLen, mac, 4);
	ulSendLen += 4;
	  
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];	
    if (0x9000 != sw)
    {
        return SDR_COMMFAIL;
    }
		
	ulTmpLen -= 2;
    return SDR_OK;
}

//-------------------------------------------------
/*
导入RSA1024密钥
描述:
    用于导入RSA1024密钥对
参数:
    key_encrypting_key_KeyIndex     输入，要使用的sm4密钥加密密钥索引
    key_encrypting_key              输入，要使用的sm4密钥加密密钥
	KeyIndex			            输入，要导入的RSA1024密钥索引
	Key			                    输入，要导入的密钥,公钥模（N）+ 私钥（N）
    Len                             输入，要导入的密钥长度
    Keyformat                       输入，要导入的RSA1024密钥格式，0x10 = 仅公钥存在，0x11 = ND格式密钥对，0x12 = CRT格式密钥对，0x01 = 仅私钥D存在，0x02 = 仅CRT格式私钥存在
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV Import_RSA1024Key(SGD_UINT32 key_encrypting_key_KeyIndex, SGD_UCHAR *key_encrypting_key, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Len, SGD_UINT32 Keyformat)
{
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    if(key_encrypting_key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[1000] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    SGD_UCHAR plaintext[1000] = {0x00};
    SGD_UCHAR ciphetext[1000] = {0x00};
    SGD_UCHAR pOutRand[16] = {0x00};
    SGD_UCHAR macdata[1000] = {0x00};
    SGD_UCHAR mac[4] = {0x00};
    
    if ((Len != 128) && (Len != 256) && (Len != 448) && (Len != 320))
    {
        return SDR_COMMOND_ERROR;
    }     
    
    //十六字节补位
    plaintext[0] = 0x09;
    plaintext[1] = KeyIndex;
    data16padding(Key, Len, plaintext+2);
    plaintext[2] = Keyformat; //plaintext =密钥类型（1字节） + 密钥ID（1字节）+ 密钥格式（1字节）+ 公钥模（N）+ 私钥（N）
    Len = Len+16;//补位后的明文长度,公钥模（N）+ 私钥（N）的和一定是16的整数倍，所以补位后的长度是密钥长度+16
    
    //采用sm4-ECB模式加密，获得密文ciphetext
    sm4_context ctx;
    sm4_setkey_enc(&ctx,key_encrypting_key);  
    sm4_crypt_ecb(&ctx,1,Len,plaintext,ciphetext); 
    
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 

    //将随机数作为初始向量，使用根密钥对“密文”进行SM4 CBC计算，
    sm4_setkey_enc(&ctx,key_encrypting_key);
    sm4_crypt_cbc(&ctx,1,Len,pOutRand, ciphetext, macdata); 
    
    //计算结果最后一组的前4字节，为mac
    sm4_mac(macdata, Len, mac);
       
	pSendBuf[ulSendLen++] = 0x04;
	pSendBuf[ulSendLen++] = 0xCC;
	pSendBuf[ulSendLen++] = 0X01;
	pSendBuf[ulSendLen++] = key_encrypting_key_KeyIndex;
	pSendBuf[ulSendLen++] = 0X00;
	memcpy(pSendBuf + ulSendLen, ciphetext, Len);
	ulSendLen += Len;
	memcpy(pSendBuf + ulSendLen, mac, 4);
	ulSendLen += 4;
	  
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];	
    if (0x9000 != sw)
    {
        return SDR_COMMFAIL;
    }
		
	ulTmpLen -= 2;
    return SDR_OK;    
}


//-------------------------------------------------
/*
导入RSA2048密钥
描述:
    用于导入RSA2048密钥对
参数:
    key_encrypting_key_KeyIndex     输入，要使用的sm4密钥加密密钥索引
    key_encrypting_key              输入，要使用的sm4密钥加密密钥
	KeyIndex			            输入，要导入的RSA2048密钥索引
	Key			                    输入，要导入的密钥,公钥模（N）+ 私钥（N）
    Len                             输入，要导入的密钥长度
    Keyformat                       输入，要导入的RSA2048密钥格式，0x10 = 仅公钥存在，0x11 = ND格式密钥对，0x12 = CRT格式密钥对，0x01 = 仅私钥D存在，0x02 = 仅CRT格式私钥存在
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV Import_RSA2048Key(SGD_UINT32 key_encrypting_key_KeyIndex, SGD_UCHAR *key_encrypting_key, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Len, SGD_UINT32 Keyformat)
{
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    if(key_encrypting_key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[1000] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    SGD_UCHAR plaintext[1000] = {0x00};
    SGD_UCHAR ciphetext[1000] = {0x00};
    SGD_UCHAR pOutRand[16] = {0x00};
    SGD_UCHAR macdata[1000] = {0x00};
    SGD_UCHAR mac[4] = {0x00};
    
    if ((Len != 256) && (Len != 512) && (Len != 896) && (Len != 640))
    {
        return SDR_COMMOND_ERROR;
    }         
    
    //十六字节补位
    plaintext[0] = 0x0A; //密钥类型（1字节）
    plaintext[1] = KeyIndex;//密钥ID（1字节）
    data16padding(Key, Len, plaintext+2);
    plaintext[2] = Keyformat; //plaintext =密钥类型（1字节） + 密钥ID（1字节）+ 密钥格式（1字节）+ 公钥模（N）+ 私钥（N）
    Len = Len+16;//补位后的明文长度,公钥模（N）+ 私钥（N）的和一定是16的整数倍，所以补位后的长度是密钥长度+16
    
    //采用sm4-ECB模式加密，获得密文ciphetext
    sm4_context ctx;
    sm4_setkey_enc(&ctx,key_encrypting_key);  
    sm4_crypt_ecb(&ctx,1,Len,plaintext,ciphetext); 
    
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 

    //将随机数作为初始向量，使用根密钥对“密文”进行SM4 CBC计算，
    sm4_setkey_enc(&ctx,key_encrypting_key);
    sm4_crypt_cbc(&ctx,1,Len,pOutRand, ciphetext, macdata); 
    
    //计算结果最后一组的前4字节，为mac
    sm4_mac(macdata, Len, mac);
       
	pSendBuf[ulSendLen++] = 0x04;
	pSendBuf[ulSendLen++] = 0xCC;
	pSendBuf[ulSendLen++] = 0X01;
	pSendBuf[ulSendLen++] = key_encrypting_key_KeyIndex;
	pSendBuf[ulSendLen++] = 0X00;
	memcpy(pSendBuf + ulSendLen, ciphetext, Len);
	ulSendLen += Len;
	memcpy(pSendBuf + ulSendLen, mac, 4);
	ulSendLen += 4;
	  
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];	
    if (0x9000 != sw)
    {
        return SDR_COMMFAIL;
    }
		
	ulTmpLen -= 2;
    return SDR_OK;    
}

//-------------------------------------------------
/*
导入AES密钥
描述:
    用于导入AES密钥
参数:
    key_encrypting_key_KeyIndex     输入，要使用的sm4密钥加密密钥索引
    key_encrypting_key              输入，要使用的sm4密钥加密密钥
	KeyIndex			            输入，要导入的AES密钥索引
	Key			                    输入，要导入的密钥,
    Len                             输入，要导入的密钥长度,长度为16/24/32
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV Import_AESKey( SGD_UINT32 key_encrypting_key_KeyIndex, SGD_UCHAR *key_encrypting_key, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Len)
{
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
    if(key_encrypting_key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
    
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[100] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    SGD_UCHAR plaintext[100] = {0x00};
    SGD_UCHAR ciphetext[100] = {0x00};
    SGD_UCHAR pOutRand[16] = {0x00};
    SGD_UCHAR macdata[100] = {0x00};
    SGD_UCHAR mac[4] = {0x00};
     
    if ((Len != 16) &&  (Len != 24) &&  (Len != 32))
    {
       return SDR_COMMOND_ERROR ;
    }
    
    if (Len == 32)
    {
        //十六字节补位
        plaintext[0] = 0x02;
        plaintext[1] = KeyIndex;
        data16padding(Key, Len, plaintext+2);
         //plaintext =密钥类型（1字节）+ 密钥ID（1字节）+ 密钥长度（1字节）+ 密钥值（N）+ 补位数据（0x80...00）
        Len = 48;//补位后的明文长度是密钥长度+16        
    }
    else if ((Len == 16) || (Len == 24))
    {
        //十六字节补位
        plaintext[0] = 0x02;
        plaintext[1] = KeyIndex;
        data16padding(Key, Len, plaintext+2);
         //plaintext =密钥类型（1字节）+ 密钥ID（1字节）+ 密钥长度（1字节）+ 密钥值（N）+ 补位数据（0x80...00）
        Len = 32;//补位后的明文长度是密钥长度+16
    }
    
    //采用sm4-ECB模式加密，获得密文ciphetext
    sm4_context ctx;
    sm4_setkey_enc(&ctx,key_encrypting_key);  
    sm4_crypt_ecb(&ctx,1,Len,plaintext,ciphetext); 
    
    //获取16字节随机数
    GetRandom(0x10, pOutRand); 

    //将随机数作为初始向量，对“密文”进行SM4 CBC计算，
    sm4_setkey_enc(&ctx,key_encrypting_key);
    sm4_crypt_cbc(&ctx,1,Len,pOutRand, ciphetext, macdata); 
    
    //计算结果最后一组的前4字节，为mac
    sm4_mac(macdata, Len, mac);
       
    pSendBuf[ulSendLen++] = 0x04;
    pSendBuf[ulSendLen++] = 0xd4;
    pSendBuf[ulSendLen++] = 0X01;
    pSendBuf[ulSendLen++] = key_encrypting_key_KeyIndex;
    pSendBuf[ulSendLen++] = Len+4;
    memcpy(pSendBuf + ulSendLen, ciphetext, Len);
    ulSendLen += Len;
    memcpy(pSendBuf + ulSendLen, mac, 4);
    ulSendLen += 4;
      
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];	
    if (0x9000 != sw)
    {
        return SDR_COMMFAIL;
    }
		
	ulTmpLen -= 2;
    return SDR_OK;    
}


//-------------------------------------------------
/*
导入密钥
描述:
    用于导入根密钥、密钥加密密钥、ECC密钥、RSA2048密钥对
参数:
    key_type                        输入，取值：0x01 = 导入根密钥，0x02 = 导入密钥加密密钥，0x03 = 导入ECC密钥，
                                                0x04 = 导入RSA1024密钥，0x05 = 导入RSA2048密钥，0x06 = 导入AES密钥
    rootkey                         输入，16字节的根密钥。导入ECC密钥、RSA1024密钥、RSA2048密钥、AES密钥时，此参数忽略.
    key_encrypting_key_KeyIndex     输入，要使用的sm4密钥加密密钥索引，导入根密钥和密钥加密密钥时，此参数忽略。
    key_encrypting_key              输入，要使用的sm4密钥加密密钥，导入根密钥和密钥加密密钥时，此参数忽略。


	KeyIndex			            输入，要导入的密钥索引。导入根密钥时，此参数忽略。
    Key			                    输入，要导入的密钥
    Len                             输入，要导入的密钥长度
    Keyformat                       输入，要导入的RSA1024密钥和RSA2048密钥格式，导入除RSA1024密钥、RSA2048密钥外的其他密钥时，此参数忽略。
                                            0x10 = 仅公钥存在，0x11 = ND格式密钥对，0x12 = CRT格式密钥对，0x01 = 仅私钥D存在，0x02 = 仅CRT格式私钥存在

返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_Import_Key(SGD_UINT32 key_type, SGD_UCHAR *rootkey, SGD_UINT32 key_encrypting_key_KeyIndex, SGD_UCHAR *key_encrypting_key, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Len, SGD_UINT32 Keyformat)
{    
    SGD_UINT16 wRet;
    	
	switch(key_type)
	{
		case 0x01:           
			wRet = Import_rootkey(rootkey, Key, Len);
        	if (0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
			break;
		case 0x02:
			wRet = Import_key_encrypting_key(rootkey, KeyIndex, Key, Len);
        	if (0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
			break;
		case 0x03:
			wRet = Import_ECCKey(key_encrypting_key_KeyIndex, key_encrypting_key, KeyIndex, Key, Len);
        	if (0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
			break;
		case 0x04:
			wRet = Import_RSA1024Key(key_encrypting_key_KeyIndex, key_encrypting_key, KeyIndex, Key, Len, Keyformat);
        	if (0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
			break;
        case 0x05:
			wRet = Import_RSA2048Key(key_encrypting_key_KeyIndex, key_encrypting_key, KeyIndex, Key, Len, Keyformat);
        	if (0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
			break;
        case 0x06:
			wRet = Import_AESKey(key_encrypting_key_KeyIndex, key_encrypting_key, KeyIndex, Key, Len);
        	if (0x00 != wRet)
            {
                return SDR_COMMFAIL;
            }
			break;
		default:
			return SDR_COMMOND_ERROR;
	}
    
    return SDR_OK;   
    
}

/*
导出密钥
描述:
    用于导出ECC公钥、RSA1024公钥、RSA2048公钥
参数:
    key_type                        输入，取值：0x01 = 导出ECC密钥，0x02 = 导出RSA1024密钥，0x03 = 导出RSA2048密钥
	KeyIndex			            输入，要导出的密钥索引
    Key			                    输出，导出的密钥
    Len                             输出，导出的密钥长度

返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_export_Key(SGD_UINT32 key_type, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 *Len)
{
    if(Key == NULL)
    {
        return SDR_SPACE_ERROR;
    }
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[5] = { 0 };
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[300] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);

    switch(key_type)
    {
        case 0x01:
            	pSendBuf[0] = 0x00;
                pSendBuf[1] = 0xC9;
                pSendBuf[2] = 0x00;
                pSendBuf[3] = KeyIndex;
                pSendBuf[4] = 0x40;
                break;
        case 0x02:
            	pSendBuf[0] = 0x00;
                pSendBuf[1] = 0xCD;
                pSendBuf[2] = 0x09;
                pSendBuf[3] = KeyIndex;
                pSendBuf[4] = 0x00;  
                break;        
        case 0x03:
            	pSendBuf[0] = 0x00;
                pSendBuf[1] = 0xCD;
                pSendBuf[2] = 0x0A;
                pSendBuf[3] = KeyIndex;
                pSendBuf[4] = 0x00;   
                break;  
 		default:
			return SDR_COMMOND_ERROR;          
    }        
	
	wRet = Transmit(pSendBuf, 5, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
    *Len = ulTmpLen;
    memcpy(Key, tmpOutBuf, ulTmpLen);
    return SDR_OK;       
}


//-------------------------------------------------
/*
AES加密
参数:
    mode                输入,加密模式，取值：0x01 = ECB模式，0x02 = CBC模式
    key_flag            输入,使用内部密钥，还是随加密指令导入密钥，取值：0x01 = 使用内部密钥，0x02 = 随指令导入密钥
    ivdata              输入,mode = 0x02 ，CBC模式下，需要导入的初始向量iv，iv长度为16字节。ECB模式时此参数可忽略
	KeyIndex			输入,AES密钥索引值.取值0x01~0x10。当随加密指令导入密钥时，此参数可忽略。
    Key                 输入,随加密指令导入的密钥值。
    Keylen              输入,随加密指令导入的密钥长度，取值为16，24，32。当使用内部密钥时，此参数可忽略
	Data			    输入,待加密的数据。当使用内部密钥时，此参数可忽略
    DataLen		        输入,待加密的数据长度
	EncData				输出,加密后的数据
	EncDataLen		    输出,加密后的数据长度.

返回值:
	 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_AES_Encrypt(SGD_UINT32 mode,SGD_UINT32 key_flag, SGD_UCHAR *ivdata, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Keylen, SGD_UCHAR *Data,SGD_UINT32 DataLen, SGD_UCHAR *EncData, SGD_UINT32 *EncDataLen)
{
    if (DataLen > 2048)
    {
       return SDR_COMMOND_ERROR ;
    }    
    if ((mode != 0x01) &&  (mode != 0x02))
    {
       return SDR_COMMOND_ERROR ;
    }
    if ((key_flag != 0x01) &&  (key_flag != 0x02))
    {
       return SDR_COMMOND_ERROR ;
    }
    
    if(Data == NULL)
    {
        return SDR_SPACE_ERROR;
    }  
    if(EncData == NULL)
    {
        return SDR_SPACE_ERROR;
    }  
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[2100] = { 0 };
    SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[2100] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
     
    //CBC模式时，先导入初始向量iv    
    if(mode == 0x02)    
    {
        if(ivdata == NULL)
        {
            return SDR_SPACE_ERROR;
        }
        //先导入初始向量iv
        pSendBuf[0] = 0x80;
        pSendBuf[1] = 0x48;
        pSendBuf[2] = 0x00;
        pSendBuf[3] = 0x00;
        pSendBuf[4] = 0x10;       
        memcpy(pSendBuf + 5, ivdata, 16);
        ulSendLen= 21;
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }

        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }

        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (0x9000 != sw)
        {
            return SDR_COMMFAIL;
        }
    }      
    
    //开始加密
    if (mode == 0x01)  //ECB模式
    {
        pSendBuf[0] = 0x00;
    }
    else if (mode == 0x02) //CBC模式
    {
        pSendBuf[0] = 0x80;    
    }
    
    pSendBuf[1] = 0x48;
    
    if (key_flag == 0x01) //使用内部密钥
    {
        pSendBuf[2] = KeyIndex;
        pSendBuf[3] = (DataLen >> 8) & 0xFF;
        pSendBuf[4] = DataLen& 0xFF;
        memcpy(pSendBuf + 5, Data, DataLen);
        ulSendLen= DataLen + 5;
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }
        
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (0x9000 != sw)
        {
            return SDR_COMMFAIL;
        }
        ulTmpLen -= 2;
        *EncDataLen = ulTmpLen;
        memcpy(EncData, tmpOutBuf, ulTmpLen);
        
    }
    else if(key_flag == 0x02)//随指令导入密钥
    {
        if(Key == NULL)
        {
            return SDR_SPACE_ERROR;
        }  
        pSendBuf[2] = 0x11;
        pSendBuf[3] = ((DataLen+Keylen+1) >> 8) & 0xFF;
        pSendBuf[4] = (DataLen+Keylen+1)& 0xFF;
        pSendBuf[5] = Keylen;
        memcpy(pSendBuf + 6, Key, Keylen);
        ulSendLen= Keylen + 6;
        memcpy(pSendBuf + ulSendLen, Data, DataLen);
        ulSendLen += DataLen;            
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }
        
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (0x9000 != sw)
        {
            return SDR_COMMFAIL;
        }
        ulTmpLen -= 2;
        *EncDataLen = ulTmpLen;
        memcpy(EncData, tmpOutBuf, ulTmpLen);
    }
                
	return SDR_OK;   
}

//-------------------------------------------------
/*
AES解密
参数:
	参数:
    mode                输入,解密模式，取值：0x01 = ECB模式，0x02 = CBC模式
    key_flag            输入,使用内部密钥，还是随解密指令导入密钥，取值：0x01 = 使用内部密钥，0x02 = 随指令导入密钥
    ivdata              输入,mode = 0x02 ，CBC模式下，需要导入的初始向量iv，iv长度为16字节，ECB模式时此参数可忽略
	KeyIndex			输入,AES密钥索引值.取值0x01~0x10，当随解密指令导入密钥时，此参数可忽略。
    Key                 输入,随解密指令导入的密钥值。当使用内部密钥时，此参数可忽略
    Keylen              输入,随解密指令导入的密钥长度，取值为16，24，32。当使用内部密钥时，此参数可忽略
	EncData			    输入,待解密的数据
    EncDataLen		    输入,待解密的数据长度
	DecData				输出,解密后的数据
	DecDataLen		    输出,解密后的数据长度.

返回值:
	 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_AES_Decrypt(SGD_UINT32 mode, SGD_UINT32 key_flag, SGD_UCHAR *ivdata, SGD_UINT32 KeyIndex, SGD_UCHAR *Key, SGD_UINT32 Keylen, SGD_UCHAR *EncData,SGD_UINT32 EncDataLen, SGD_UCHAR *DecData, SGD_UINT32 *DecDataLen)
{
    if (EncDataLen > 2048)
    {
       return SDR_COMMOND_ERROR ;
    }    
    if ((mode != 0x01) &&  (mode != 0x02))
    {
       return SDR_COMMOND_ERROR ;
    }
    if ((key_flag != 0x01) &&  (key_flag != 0x02))
    {
       return SDR_COMMOND_ERROR ;
    }
    
    if(EncData == NULL)
    {
        return SDR_SPACE_ERROR;
    }  
    if(DecData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[2100] = { 0 };
    SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[2100] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);   
   
    //CBC模式时，先导入初始向量iv    
    if(mode == 0x02)    
    {
        if(ivdata == NULL)
        {
            return SDR_SPACE_ERROR;
        } 
        //先导入初始向量iv
        pSendBuf[0] = 0x80;
        pSendBuf[1] = 0x49;
        pSendBuf[2] = 0x00;
        pSendBuf[3] = 0x00;
        pSendBuf[4] = 0x10;       
        memcpy(pSendBuf + 5, ivdata, 16);
        ulSendLen= 21;
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }

        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }

        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (0x9000 != sw)
        {
            return SDR_COMMFAIL;
        }
    }      
    
    //开始解密
    if (mode == 0x01)  //ECB模式
    {
        pSendBuf[0] = 0x00;
    }
    else if (mode == 0x02) //CBC模式
    {
        pSendBuf[0] = 0x80;    
    }
    
    pSendBuf[1] = 0x49;
    
    if (key_flag == 0x01) //使用内部密钥
    {
        pSendBuf[2] = KeyIndex;
        pSendBuf[3] = (EncDataLen >> 8) & 0xFF;
        pSendBuf[4] = EncDataLen& 0xFF;
        memcpy(pSendBuf + 5, EncData, EncDataLen);
        ulSendLen= EncDataLen + 5;
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }
        
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (0x9000 != sw)
        {
            return SDR_COMMFAIL;
        }
        ulTmpLen -= 2;
        *DecDataLen = ulTmpLen;
        memcpy(DecData, tmpOutBuf, ulTmpLen);
        
    }
    else if(key_flag == 0x02)//随指令导入密钥
    {
        if(Key == NULL)
        {
            return SDR_SPACE_ERROR;
        } 
        pSendBuf[2] = 0x11;
        pSendBuf[3] = ((EncDataLen+Keylen+1) >> 8) & 0xFF;
        pSendBuf[4] = (EncDataLen+Keylen+1)& 0xFF;
        pSendBuf[5] = Keylen;
        memcpy(pSendBuf + 6, Key, Keylen);
        ulSendLen= Keylen + 6;
        memcpy(pSendBuf + ulSendLen, EncData, EncDataLen);
        ulSendLen += EncDataLen;            
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }
        
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (0x9000 != sw)
        {
            return SDR_COMMFAIL;
        }
        ulTmpLen -= 2;
        *DecDataLen = ulTmpLen;
        memcpy(DecData, tmpOutBuf, ulTmpLen);
    }
                
	return SDR_OK;       
}
	
    
    
    
/*
生成ECC密钥对
描述:
    用于生成 ECC 密钥对。 
    命令执行时，若指定 KID 的密钥对存在，则生成的密钥对内容将覆盖已存在密钥对的值。
参数:
	KeyIndex			输入,指定产生ECC密钥对的密钥索引,取值范围1~16。 
返回值:
	返回0成功,其他值见错误码.
*/
SGD_RV hs_GenerateECCKeyPair(SGD_UINT32 KeyIndex)
{
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[10] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xC5;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = KeyIndex;
	pSendBuf[ulSendLen++] = 0x00;
	
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}

    return SDR_OK;
}



//-------------------------------------------------
/*
ECC加密
参数:
	KeyIndex			输入,ECC密钥索引值.取值0x01~0x10.需保证索引值对应位置已存在密钥.
	Data				输入,要加密的数据，必须为16字节整数倍，长度范围16-1024（十进制）字节。
	DataLength		    输入,要加密的数据的长度
	EncData			    输出,加密后的数据.
    EncDataLength		输出,加密后的数据长度
返回值:
	 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_ECC_Encrypt(SGD_UINT32 KeyIndex, SGD_UCHAR *Data, SGD_UINT32 DataLength, SGD_UCHAR *EncData, SGD_UINT32 *EncDataLength)
{
    if(Data == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(EncData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[1200] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[1200] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xE4;
	pSendBuf[ulSendLen++] = KeyIndex;
	pSendBuf[ulSendLen++] = (DataLength >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = DataLength & 0xFF;
	memcpy(pSendBuf + ulSendLen, Data, DataLength);
	ulSendLen += DataLength;
	
	 
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
	*EncDataLength= ulTmpLen;
    memcpy(EncData, tmpOutBuf, ulTmpLen);
    
    return SDR_OK;
}


//-------------------------------------------------
/*
ECC解密
参数:
	参数:
	KeyIndex			输入,ECC密钥索引值.取值0x01~0x10.需保证索引值对应位置已存在密钥.
	EncData			    输入,要解密的数据
    EncDataLength		输入,要解密的数据长度
	DecData				输出,解密后的数据
	DecDataLength		输出,解密后的数据长度.

返回值:
	 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_ECC_Decrypt(SGD_UINT32 KeyIndex, SGD_UCHAR *EncData, SGD_UINT32 EncDataLength, SGD_UCHAR *DecData, SGD_UINT32 *DecDataLength)
{
    if(EncData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(DecData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[1200] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[1200] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xE5;
	pSendBuf[ulSendLen++] = KeyIndex;
	pSendBuf[ulSendLen++] = (EncDataLength >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = EncDataLength & 0xFF;
    memcpy(pSendBuf + ulSendLen, EncData, EncDataLength);
    ulSendLen += EncDataLength;
	 
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT); 
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
        return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
    *DecDataLength = ulTmpLen;
    memcpy(DecData,tmpOutBuf,ulTmpLen);
	
	return SDR_OK;
}

//-------------------------------------------------
/*
ECC签名
描述:使用内部ECC私钥对数据进行签名运算
参数:
	uiKeyIndex			输入,ECC密钥索引值，取值0x01~0x10.需保证索引值对应位置已存在密钥.
	HashFlag			输入,Hash标志，取值0x01或0x00; 为0x01表示输入数据pucDataInput为待签名数据的sha256哈希结果；
                                                        为0x00表示输入数据pucDataInput为待签名数据原文。
	InputData		    输入,用于存放外部输入的数据，当ucHashFlag等于0x01时，内容为待签名数据的sha256哈希结果；
                                                        当ucHashFlag等于0x00时，内容为待签名数据原文.
	InputLength		    输入,输入的数据长度，当输入内容为sha256哈希结果时为32；当输入内容为签名原文时最大取值2048.
	Signature		    输出,用于存放输出的签名值数据.
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_ECC_Sign(SGD_UINT32 KeyIndex, SGD_UINT32 HashFlag, SGD_UCHAR *InputData, SGD_UINT32 InputLength, SGD_UCHAR *Signature)
{
    if(InputData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(Signature == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[3000] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[100] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = HashFlag ? 0x00 : 0x80;
	pSendBuf[ulSendLen++] = 0xE6;
	pSendBuf[ulSendLen++] = KeyIndex;
	pSendBuf[ulSendLen++] = (InputLength >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = InputLength & 0xFF;
	memcpy(pSendBuf + ulSendLen, InputData, InputLength);
	ulSendLen += InputLength;
	
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
			return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;	
	memcpy(Signature,tmpOutBuf,64);

    return SDR_OK;	
}

//-------------------------------------------------
/*
ECC验签
描述:使用内部ECC公钥对ECC签名值进行验证运算
参数:
	KeyIndex			输入,ECC密钥索引值，取值0x01~0x10.需保证索引值对应位置已存在密钥.
	HashFlag			输入,Hash标志，取值0x01或0x00; 为0x01表示输入数据pucDataInput为待签名数据的sha256哈希结果；
                                                        为0x00表示输入数据pucDataInput为待签名数据原文。
	InputData		    输入,用于存放外部输入的数据，当ucHashFlag等于0x01时，内容为待签名数据的sha256哈希结果；
                                                        当ucHashFlag等于0x00时，内容为待签名数据原文.
	InputLength		    输入,输入的数据长度，当输入内容为sha256哈希结果时为32；当输入内容为签名原文时最大取值2048..
	Signature		    输入,用于存放输入的签名值数据.
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_ECC_Verify(SGD_UINT32 KeyIndex, SGD_UCHAR HashFlag, SGD_UCHAR *InputData, SGD_UINT32 InputLength, SGD_UCHAR *Signature)
{
    if(InputData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(Signature == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[3000] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = HashFlag ? 0x00 : 0x80;
	pSendBuf[ulSendLen++] = 0xE7;
	pSendBuf[ulSendLen++] = KeyIndex;
	pSendBuf[ulSendLen++] = ((InputLength + 64) >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = (InputLength + 64) & 0xFF;
    memcpy(pSendBuf + ulSendLen,Signature,64);
    ulSendLen += 64;
	memcpy(pSendBuf + ulSendLen,InputData,InputLength);
	ulSendLen += InputLength;

		  
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);	 
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
			return SDR_COMMFAIL;
	}
	
    return SDR_OK;	
}


/*
生成RSA密钥对
描述:
    用于生成 RSA密钥对。 
    命令执行时，若指定 KID 的密钥对存在，则生成的密钥对内容将覆盖已存在密钥对的值。
参数:
    type                输入，密钥类型，取值：0x01 = RSA1024密钥，0x02 = RSA2048密钥，
	KeyIndex			输入,指定产生RSA密钥对的密钥索引,取值范围1~16。 
返回值:
	返回0成功,其他值见错误码.
*/
SGD_RV hs_GenerateRSAKeyPair(SGD_UINT32 type, SGD_UINT32 KeyIndex)
{
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[10] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	   
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xCE;
    
    if (type == 0x01)
    {
       pSendBuf[ulSendLen++] = 0x09;
    }
	else if (type == 0x02)
    {
       pSendBuf[ulSendLen++] = 0x0A;
    }
       
	pSendBuf[ulSendLen++] = KeyIndex;
	pSendBuf[ulSendLen++] = 0x00;
	
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
    return SDR_OK;
}	

//-------------------------------------------------
/*
RSA加密
参数:
    type                输入，密钥类型，取值：0x01 = RSA1024密钥，0x02 = RSA2048密钥，
	KeyIndex			输入,RSA密钥索引值.取值0x01~0x10.需保证索引值对应位置已存在密钥.
	Data				输入,要加密的数据，必须为16字节整数倍，长度范围16-1024（十进制）字节。
	DataLength		    输入,要加密的数据的长度
	EncData			    输出,加密后的数据.
    EncDataLength		输出,加密后的数据长度
返回值:
	 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_RSA_Encrypt(SGD_UINT32 type, SGD_UINT32 KeyIndex, SGD_UCHAR *Data, SGD_UINT32 DataLength, SGD_UCHAR *EncData, SGD_UINT32 *EncDataLength)
{
    if(Data == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(EncData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[512] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[300] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xEA;
    
    if (type == 0x01)
    {
       pSendBuf[ulSendLen++] = 0x09;
    }
	else if (type == 0x02)
    {
       pSendBuf[ulSendLen++] = 0x0A;
    }
    
	pSendBuf[ulSendLen++] = KeyIndex;
    pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = (DataLength >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = DataLength & 0xFF;
	memcpy(pSendBuf + ulSendLen, Data, DataLength);
	ulSendLen += DataLength;
		 
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
	*EncDataLength= ulTmpLen;
    memcpy(EncData, tmpOutBuf, ulTmpLen);
    
    return SDR_OK;    
}


//-------------------------------------------------
/*
RSA解密
参数:
	type                输入，密钥类型，取值：0x01 = RSA1024密钥，0x02 = RSA2048密钥，
	KeyIndex			输入,RSA密钥索引值.取值0x01~0x10.需保证索引值对应位置已存在密钥.
	EncData			    输入,要解密的数据
    EncDataLength		输入,要解密的数据长度
	DecData				输出,解密后的数据
	DecDataLength		输出,解密后的数据长度.

返回值:
	 返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_RSA_Decrypt(SGD_UINT32 type, SGD_UINT32 KeyIndex, SGD_UCHAR *EncData, SGD_UINT32 EncDataLength, SGD_UCHAR *DecData, SGD_UINT32 *DecDataLength)
{
    if(EncData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(DecData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[512] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[300] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xEB;
    
    if (type == 0x01)
    {
       pSendBuf[ulSendLen++] = 0x09;
    }
	else if (type == 0x02)
    {
       pSendBuf[ulSendLen++] = 0x0A;
    }
    
	pSendBuf[ulSendLen++] = KeyIndex;
    pSendBuf[ulSendLen++] = 0x00;
    memcpy(pSendBuf + ulSendLen, EncData, EncDataLength);
    ulSendLen += EncDataLength;
	 
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT); 
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
        return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
    *DecDataLength = ulTmpLen;
    memcpy(DecData,tmpOutBuf,ulTmpLen);
	
	return SDR_OK;
}


//-------------------------------------------------
/*
RSA签名
描述:使用内部RSA私钥对数据进行签名运算
参数:
    type                输入，密钥类型，取值：0x01 = RSA1024密钥，0x02 = RSA2048密钥，
	KeyIndex			输入,RSA密钥索引值，取值0x01~0x10.需保证索引值对应位置已存在密钥.
	InputData		    输入,用于存放外部输入的数据
	InputLength		    输入,输入的数据长度
	Signature		    输出,用于存放输出的签名值数据.
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_RSA_Sign(SGD_UINT32 type, SGD_UINT32 KeyIndex, SGD_UCHAR *InputData, SGD_UINT32 InputLength, SGD_UCHAR *Signature)
{
    if(InputData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(Signature == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[512] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[300] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xEB;
    
    if (type == 0x01)
    {
       pSendBuf[ulSendLen++] = 0x09;
    }
	else if (type == 0x02)
    {
       pSendBuf[ulSendLen++] = 0x0A;
    }
    
	pSendBuf[ulSendLen++] = KeyIndex;
    pSendBuf[ulSendLen++] = 0x00;
	memcpy(pSendBuf + ulSendLen, InputData, InputLength);
	ulSendLen += InputLength;
	
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
			return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;	
	memcpy(Signature,tmpOutBuf,ulTmpLen);

    return SDR_OK;	
}


//-------------------------------------------------
/*
RSA验签
描述:使用内部RSA公钥对RSA签名值进行验证运算
参数:
    type                输入，密钥类型，取值：0x01 = RSA1024密钥，0x02 = RSA2048密钥，
	KeyIndex			输入,RSA密钥索引值，取值0x01~0x10.需保证索引值对应位置已存在密钥.
	hashData		    输入,输入的哈希数据
	hashLength		    输入,输入的哈希长度
	Signature		    输入,用于存放输入的签名值数据.
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_RSA_Verify(SGD_UINT32 type, SGD_UINT32 KeyIndex, SGD_UCHAR *hashData, SGD_UINT32 hashLength, SGD_UCHAR *Signature)
{
    if(hashData == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    if(Signature == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
    SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[768] = { 0 };
	SGD_UINT32 ulSendLen = 0;
    SGD_UINT32 SignLen =0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	pSendBuf[ulSendLen++] = 0x80;
	pSendBuf[ulSendLen++] = 0xEA;
    
    if (type == 0x01)
    {
       pSendBuf[ulSendLen++] = 0x09;
       SignLen =128;
    }
	else if (type == 0x02)
    {
       pSendBuf[ulSendLen++] = 0x0A;
       SignLen =256;
    }
    
	pSendBuf[ulSendLen++] = KeyIndex;
    pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = ((hashLength + SignLen) >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = (hashLength + SignLen) & 0xFF;
    memcpy(pSendBuf + ulSendLen,Signature,SignLen);
    ulSendLen += SignLen;
	memcpy(pSendBuf + ulSendLen,hashData,hashLength);
	ulSendLen += hashLength;
		  
    wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);	 
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}

	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}

	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}

    return SDR_OK;	
}




//-------------------------------------------------
//----------------hash运算----------------------
//-------------------------------------------------
//-------------------------------------------------
/*
hash运算初始化
描述:三步式数据杂凑运算第一步，默认为算法为sha256
参数:
	type 输入，hash算法类型，取值为：0x00表示SM3，
                                    0x01表示SHA1，
                                    0x02表示SHA224，
                                    0x03表示SHA256
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_hash_start(SGD_UINT32 type)
{
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[10] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xE8;
    switch(type)
	{
		case 0x00:
			pSendBuf[ulSendLen++] = 0x00;
			break;
		case 0x01:
			pSendBuf[ulSendLen++] = 0x01;
			break;
		case 0x02:
			pSendBuf[ulSendLen++] = 0x02;
			break;
		case 0x03:
			pSendBuf[ulSendLen++] = 0x03;
			break;
		default:
			return SDR_COMMOND_ERROR;
	}
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x00;
	
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	 
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
	
	return SDR_OK;
}

//-------------------------------------------------
/*
hash运算
描述:三步式数据杂凑运算第二步,对输入的明文进行杂凑运算.
参数:
	input				输入,缓冲区指针,用于存放输入的数据明文.
	Len		            输入,输入的数据明文长度,最大支持0x800
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_hash_update(SGD_UCHAR *input, SGD_UINT32 Len)
{
    if(input == NULL)
    {
        return SDR_SPACE_ERROR;
    } 

	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[2100] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
    if (Len > 2048)
    {
       return SDR_COMMOND_ERROR;
    }	
    
	pSendBuf[ulSendLen++] = 0x40;
	pSendBuf[ulSendLen++] = 0xE8;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = (Len >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = Len & 0xFF;	
	memcpy(pSendBuf + ulSendLen, input, Len);
	ulSendLen += Len;

	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
	
	return SDR_OK;
}

	
//-------------------------------------------------  
/*
hash运算结束
描述:三步式数据杂凑运算第三步,杂凑运算结束返回杂凑数据并清除中间数据
参数:
    output				输出,用于存放输出的结果
	Len		            输出,存放输出的结果长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_hash_finish(SGD_UCHAR *output, SGD_UINT32 *len)
{
    if(output == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[5] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[50] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
	
	
	pSendBuf[ulSendLen++] = 0x80;
	pSendBuf[ulSendLen++] = 0xE8;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x00;	
	 
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	 	
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
    *len = ulTmpLen;
	memcpy(output, tmpOutBuf, ulTmpLen);
	
	return SDR_OK;
}

/*
hmac运算初始化,设置密钥
描述:hmac运算初始化，传入密钥，默认为算法为sha256
参数:
	key				输入,需要传入外部密钥,不能超过64字节
    key_len				输入,传入外部密钥的长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_hmac_set_key(SGD_UCHAR *key, SGD_UINT32 key_len)
{
    if(key == NULL)
    {
        return SDR_SPACE_ERROR;
    } 
   	SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[100] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[10] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    if (key_len > 64)
    {
       return SDR_COMMOND_ERROR;
    }		
	
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0x11;
	pSendBuf[ulSendLen++] = 0xFF;
	pSendBuf[ulSendLen++] = (key_len >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = key_len & 0xFF;	
	memcpy(pSendBuf + ulSendLen, key, key_len);
	ulSendLen += key_len;

	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
		
	return SDR_OK;
}


//-------------------------------------------------
/*
hmac运算
描述:hmac运算
参数:
	input				输入,用于存放输入的数据
	input_Len		    输入,输入的数据长度,最大支持0x800
    output				输出,用于存放计算的结果
	output_Len		    输出,计算的结果长度
返回值:
	返回0成功,非0表示失败
*/
//-------------------------------------------------
SGD_RV hs_hmac_calc(SGD_UCHAR *input, SGD_UINT32 input_Len, SGD_UCHAR *output, SGD_UINT32 *output_Len)
{
    if(input == NULL)
    {
        return SDR_SPACE_ERROR;
    }   
    if(output == NULL)
    {
        return SDR_SPACE_ERROR;
    }  
    if (input_Len > 2048)
    {
       return SDR_COMMOND_ERROR;
    }	    
    SGD_UINT16 wRet;
	SGD_UCHAR pSendBuf[2100] = { 0 };
	SGD_UINT32 ulSendLen = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[50] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);	
	
	pSendBuf[ulSendLen++] = 0x80;
	pSendBuf[ulSendLen++] = 0x11;
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = (input_Len >> 8) & 0xFF;
	pSendBuf[ulSendLen++] = input_Len & 0xFF;	
	memcpy(pSendBuf + ulSendLen, input, input_Len);
	ulSendLen += input_Len;

	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen,READ_OVERLAPPED_TIMEOUT);
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (0x9000 != sw)
	{
		return SDR_COMMFAIL;
	}
	
	ulTmpLen -= 2;
	*output_Len = ulTmpLen;
    memcpy(output, tmpOutBuf, ulTmpLen);
    
	return SDR_OK;
}


//-------------------------------------------------
/*
获取安全存储空间大小
描述:
    获取安全存储空间大小
参数:
    Len			    输出，安全存储空间大小

返回值:
	返回0成功,其他值见错误码.
*/
//-------------------------------------------------
SGD_RV  hs_user_info_size( SGD_UINT32 *Len)
{
    *Len = 1024;
    return SDR_OK;
}


//-------------------------------------------------
/*
写用户信息命令
描述:
     COS预留1024字节空间，供用户保存自定义信息。此命令用于写入用户信息。
        
参数:
    offset          输入，偏移字节
    Len			    输入，要写入的命令数据长度
	data	        输入，要写入的命令数据

返回值:
	返回0成功,其他值见错误码.
*/
//-------------------------------------------------
SGD_RV  hs_Write_user_info(SGD_UINT32 offset, SGD_UINT32 Len, SGD_UCHAR *data)
{
    if (offset+Len > 1024)
    {
       return SDR_COMMOND_ERROR;
    }	

    if(data == NULL)
    {
        return SDR_SPACE_ERROR;
    }   
	SGD_UINT32 wRet;
	SGD_UCHAR pSendBuf[1100] = { 0 };
	SGD_UINT32 ulSendLen = 0;
    SGD_UINT32 BufOff = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[5] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    
    while (Len > 255)
    {
        ulSendLen = 0;
        pSendBuf[ulSendLen++] = 0x00;
        pSendBuf[ulSendLen++] = 0xD6;
        pSendBuf[ulSendLen++] = (offset >> 8) & 0xFF;
        pSendBuf[ulSendLen++] =offset & 0xFF;
        pSendBuf[ulSendLen++] = 0xFF;
        memcpy(pSendBuf + ulSendLen, data+BufOff, 255);
        ulSendLen += 255;
        
        wRet = Transmit(pSendBuf, 260, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);      
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (sw != 0x9000)
        {
            return SDR_EXECUTION_ERROR;
        }  
        
      	offset += 255;
		Len -= 255;
		BufOff += 255;  
        ulSendLen = 0;
    }  
    
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xD6;
	pSendBuf[ulSendLen++] = (offset >> 8) & 0xFF;
	pSendBuf[ulSendLen++] =offset & 0xFF;
	pSendBuf[ulSendLen++] = Len;
	memcpy(pSendBuf + ulSendLen, data+BufOff, Len);
	ulSendLen += Len;
	
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
	
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
	
	return SDR_OK; 

}



//-------------------------------------------------
/*
读用户信息命令
描述:
     此命令用于从 COS 预留空间中读取用户写入的信息。
        
参数:
    offset          输入，偏移字节
    Len			    输入，要读取的命令数据长度， 当Len=00时，读取指定偏移到结束位置的全部内容
	data	        输出，存放读取的命令数据

返回值:
	返回0成功,其他值见错误码.
*/
//-------------------------------------------------
SGD_RV  hs_Read_user_info(SGD_UINT32 offset, SGD_UINT32 Len, SGD_UCHAR *data)
{
    if (offset+Len > 1024)
    {
       return SDR_COMMOND_ERROR;
    }	

    if(data == NULL)
    {
        return SDR_SPACE_ERROR;
    }    
	SGD_UINT32 wRet;
	SGD_UCHAR pSendBuf[5] = { 0 };
	SGD_UINT32 ulSendLen = 0;
    SGD_UINT32 BufOff = 0;
	SGD_UINT16 sw;
	SGD_UINT8 tmpOutBuf[260] = {0};
	SGD_UINT32 ulTmpLen = sizeof(tmpOutBuf);
    if (offset+Len > 1024)
    {
       return SDR_COMMOND_ERROR;
    }	
    
    while (Len > 255)
    {
        
        pSendBuf[ulSendLen++] = 0x00;
        pSendBuf[ulSendLen++] = 0xB0;
        pSendBuf[ulSendLen++] = (offset >> 8) & 0xFF;
        pSendBuf[ulSendLen++] =offset & 0xFF;
        pSendBuf[ulSendLen++] = 0xFF;
        
        wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
        
        if (0x00 != wRet)
        {
            return SDR_COMMFAIL;
        }
        if ((tmpOutBuf == 0) || (ulTmpLen == 0))
        {
            return SDR_COMMFAIL;
        }
        sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
        if (sw != 0x9000)
        {
            return SDR_EXECUTION_ERROR;
        }
        
        memcpy(data+BufOff, tmpOutBuf, 255);
        
        offset += 255;
		Len -= 255;
		BufOff += 255;  
        ulSendLen = 0;
    }   
    
	pSendBuf[ulSendLen++] = 0x00;
	pSendBuf[ulSendLen++] = 0xB0;
	pSendBuf[ulSendLen++] = (offset >> 8) & 0xFF;
	pSendBuf[ulSendLen++] =offset & 0xFF;
	pSendBuf[ulSendLen++] = Len;
	
	wRet = Transmit(pSendBuf, ulSendLen, tmpOutBuf , &ulTmpLen, READ_OVERLAPPED_TIMEOUT);
	
	if (0x00 != wRet)
	{
		return SDR_COMMFAIL;
	}
	if ((tmpOutBuf == 0) || (ulTmpLen == 0))
	{
		return SDR_COMMFAIL;
	}
	sw = tmpOutBuf[ulTmpLen - 2] * 0x100 + tmpOutBuf[ulTmpLen - 1];
	if (sw != 0x9000)
	{
		return SDR_EXECUTION_ERROR;
	}
    
	memcpy(data+BufOff, tmpOutBuf, Len);	
	
	return SDR_OK; 

}


