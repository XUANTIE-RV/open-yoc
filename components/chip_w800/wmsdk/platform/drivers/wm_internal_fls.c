/**
 * @file    wm_internal_fls.c
 *
 * @brief   flash Driver Module
 *
 * @author  dave
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_dbg.h"
#include "wm_mem.h"
#include "wm_list.h"
#include "wm_regs.h"
#include "wm_internal_flash.h"
#include "wm_flash_map.h"


/**System parameter, default for 2M flash*/
unsigned int  TLS_FLASH_PARAM_DEFAULT        =		  (0x81FB000UL);
unsigned int  TLS_FLASH_PARAM1_ADDR          =		  (0x81FC000UL);
unsigned int  TLS_FLASH_PARAM2_ADDR          =		  (0x81FD000UL);
unsigned int  TLS_FLASH_PARAM_RESTORE_ADDR   =	      (0x81FE000UL);
unsigned int  TLS_FLASH_OTA_FLAG_ADDR        =	      (0x81FF000UL);
unsigned int  TLS_FLASH_END_ADDR             =		  (0x81FFFFFUL);

unsigned char       internalflsID = 0xFF;
static unsigned int flsdensity    = 0;

static vu32 read_first_value(void)
{
    return M32(RSA_BASE_ADDRESS);
}

static void writeEnable(void)
{
    M32(HR_FLASH_CMD_ADDR) = 0x6;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
}

unsigned char readRID(void)
{
    M32(HR_FLASH_CMD_ADDR) = 0x2c09F;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    return read_first_value() & 0xFF;
}

static void writeBpBit_for_1wreg(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
    int status = 0;
    int bpstatus = 0;

    M32(HR_FLASH_CMD_ADDR) = 0x0C005;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    status =  read_first_value() & 0xFF;

    M32(HR_FLASH_CMD_ADDR) = 0x0C035;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    status  |=  (read_first_value() & 0xFF) << 8;

    /*Write Enable*/
    M32(HR_FLASH_CMD_ADDR) = 0x6;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    status      = (status & 0xBF83) | bpstatus | (cmp << 14);

    M32(RSA_BASE_ADDRESS)  = status;
    M32(HR_FLASH_CMD_ADDR) = 0x1A001;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
}

static void writeBpBit_for_2wreg(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
    int status = 0;
    int bpstatus = 0;

    M32(HR_FLASH_CMD_ADDR) = 0x0C005;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    status =  read_first_value() & 0xFF;

    M32(HR_FLASH_CMD_ADDR) = 0x0C035;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    status  |=  (read_first_value() & 0xFF) << 8;

    /*Write Enable*/
    M32(HR_FLASH_CMD_ADDR) = 0x6;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    bpstatus      = (status & 0x83) | bpstatus;

    M32(RSA_BASE_ADDRESS)  = bpstatus;
    M32(HR_FLASH_CMD_ADDR) = 0xA001;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;


    M32(HR_FLASH_CMD_ADDR) = 0x6;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    status      = ((status>>8) & 0xBF) | (cmp << 6);
    M32(RSA_BASE_ADDRESS)   = status;
    M32(HR_FLASH_CMD_ADDR)  = 0xA031;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;	
}


static void writeESMTBpBit(char cmp, char bp4, char bp3, char bp2, char bp1, char bp0)
{
    int status = 0;
    int bpstatus = 0;

    M32(HR_FLASH_CMD_ADDR) = 0x0C005;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    status =  read_first_value() & 0xFF;
    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    status      = (status & 0x83) | bpstatus;

    /*Write Enable*/
    M32(HR_FLASH_CMD_ADDR) = 0x6;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    bpstatus  = (bp4 << 6) | (bp3 << 5) | (bp2 << 4) | (bp1 << 3) | (bp0 << 2);
    status      = (status & 0x83) | bpstatus | (cmp << 14);

    M32(RSA_BASE_ADDRESS)  = status;
    M32(HR_FLASH_CMD_ADDR) = 0x0A001;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;


    M32(HR_FLASH_CMD_ADDR) = 0x0C085;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    status  =  read_first_value() & 0xFF;

    /*Write Enable*/
    M32(HR_FLASH_CMD_ADDR) = 0x6;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    status		= (status & 0xBF) | (cmp << 6);
    M32(RSA_BASE_ADDRESS)  = status;
    M32(HR_FLASH_CMD_ADDR) = 0x0A0C1;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;
}

static int flashunlock(void)
{
    switch(readRID())
    {
    case SPIFLASH_MID_GD:
	case SPIFLASH_MID_TSINGTENG:
        writeBpBit_for_1wreg(0, 0, 0, 0, 0, 0);
        break;
    case SPIFLASH_MID_PUYA:
	case SPIFLASH_MID_XTX:
	case SPIFLASH_MID_BOYA:
	case SPIFLASH_MID_FUDANMICRO:
	case SPIFLASH_MID_XMC:
		writeBpBit_for_2wreg(0, 0, 0, 0, 0, 0);
		break;
    case SPIFLASH_MID_ESMT:
        writeESMTBpBit(0, 0, 0, 0, 0, 0);
        break;
    default:
        return -1;
    }
    return 0;
}

static int flashlock(void)
{
    switch(readRID())
    {
    case SPIFLASH_MID_GD:
	case SPIFLASH_MID_TSINGTENG:		
        writeBpBit_for_1wreg(0, 1, 1, 0, 1, 0);
		break;
    case SPIFLASH_MID_PUYA:
	case SPIFLASH_MID_XTX:
	case SPIFLASH_MID_BOYA:
	case SPIFLASH_MID_FUDANMICRO:
	case SPIFLASH_MID_XMC:
		writeBpBit_for_2wreg(0, 1, 1, 0, 1, 0);
        break;
    case SPIFLASH_MID_ESMT:
        writeESMTBpBit(0, 1, 1, 0, 1, 0);
        break;
    default:
        return -1;/*do not clear QIO Mode*/
    }
    return 0;
}

static int programSR(unsigned int  cmd, unsigned long addr, unsigned char *buf,  unsigned int sz)
{
    unsigned long base_addr = 0;
    unsigned int size = 0;


    if (sz > INSIDE_FLS_PAGE_SIZE)
    {
        sz = INSIDE_FLS_PAGE_SIZE;
    }

    base_addr = RSA_BASE_ADDRESS;
    size = sz;
    while(size)
    {
        M32(base_addr) = *((unsigned long *)buf);
        base_addr += 4;
        buf += 4;
        size -= 4;
    }

    writeEnable();
    M32(HR_FLASH_CMD_ADDR) = cmd | ((sz - 1) << 16);
    M32(HR_FLASH_ADDR) = (addr & 0x1FFFFFF);
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    return 0;
}


static int programPage (unsigned long adr, unsigned long sz, unsigned char *buf)
{
    programSR(0x80009002, adr, buf, sz);
    return(0);
}

static int eraseSR(unsigned int cmd, unsigned long addr)
{
    /*Write Enable*/
    writeEnable();
    M32(HR_FLASH_CMD_ADDR) = cmd;
    M32(HR_FLASH_ADDR) = (addr & 0x1FFFFFF);
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    return 0;
}

static int eraseSector(unsigned long adr)
{
    eraseSR(0x80000820, adr);
    return (0); // Finished without Errors
}

static int erasePage(unsigned long adr) //for puya flash
{
    eraseSR(0x80000881, adr);
    return (0); // Finished without Errors
}

static unsigned int getFlashDensity(void)
{
    unsigned char density = 0;

    M32(HR_FLASH_CMD_ADDR) = 0x2c09F;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    density = ((read_first_value() & 0xFFFFFF) >> 16) & 0xFF;
    //	printf("density %x\n", density);
    if (density && (density <= 0x21))  /*just limit to (1<<33UL) Byte*/
    {
        return (1 << density);
    }

    return 0;
}

/*sr start*/
/************************************************************
addr:
For GD,		      address:0x000,0x100
For ESMT,		address:0xFF000,0xFF100
For WINBOND,	address:0x1000
*************************************************************/
int readSR(unsigned int cmd, unsigned long addr, unsigned char *buf, unsigned long sz)
{
    int i = 0;
    int word = sz / 4;
    int byte = sz % 4;
    unsigned long addr_read;

    M32(HR_FLASH_CMD_ADDR) = cmd | (((sz - 1) & 0x3FF) << 16);
    M32(HR_FLASH_ADDR) = addr & 0x1FFFFFF;
    M32(HR_FLASH_CMD_START) = CMD_START_Msk;

    addr_read = RSA_BASE_ADDRESS;
    for(i = 0; i < word; i ++)
    {
        M32(buf) = M32(addr_read);
        buf += 4;
        addr_read += 4;
    }

    if(byte > 0)
    {
        M32(buf) = M32(addr_read);
        buf += 3;							//point last byte
        while(byte)
        {
            *buf = 0;
            buf --;
            byte --;
        }
    }

    return 0;
}

void flashSRRW(unsigned long offset, unsigned char *buf, unsigned long sz, unsigned char *backbuf, unsigned int backlen, unsigned int rd)
{
#define SR_TOTAL_SZ (512)
#define SR_PROGRAM_SZE (256)
    unsigned int i;
    unsigned int j;
    unsigned int baseaddr = 0;
    unsigned int sectoroffset = 0;
    unsigned int sectorsize = 0;
    unsigned int sectornum = 0;
    unsigned int remainsz;
    unsigned int erasecmd = 0;
    unsigned int readcmd  = 0;
    unsigned int writecmd = 0;

    unsigned char flashid = 0;
    if (!buf || ((rd == 0) && ( !backbuf || (backlen < 512))))
    {
        return;
    }

    flashid = readRID();
    switch(flashid)
    {
    case SPIFLASH_MID_GD:
        baseaddr = 0x0;
        sectoroffset = 256;
        sectorsize = 256;
        sectornum = 2;
        erasecmd	 = 0x80000844;
        readcmd 	 = 0xBC00C048;
        writecmd	 = 0x80009042;
        break;

    case SPIFLASH_MID_ESMT:
    {
        baseaddr = 0xFF000;
        sectoroffset = 0;
        sectorsize = 512;
        sectornum = 1;
        erasecmd	 = 0x80000820;
        readcmd 	 = 0xBC00C00B;
        writecmd	 = 0x80009002;

        M32(HR_FLASH_CMD_ADDR) = 0x3A;             /*enter OTP*/
        M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    }
    break;

    case SPIFLASH_MID_PUYA:
        baseaddr = 0x1000;
        sectoroffset = 0;
        sectorsize = 512;
        sectornum = 1;
        erasecmd	 = 0x80000844;
        readcmd 	 = 0xBC00C048;
        writecmd	 = 0x80009042;

        break;

    default:
    {
    }
    break;
    }

    for (i = 0 ; i < sectornum; i++)
    {
        readSR(readcmd, baseaddr + sectoroffset * i, backbuf + i * sectorsize, sectorsize);
    }

    if (rd)
    {
        for(i = 0; i < sz; i++)				//Copy
        {
            buf[i] = backbuf[i + offset];
        }
    }
    else
    {
        for (i = 0; i < sectornum ; i++)
        {
            eraseSR(erasecmd, baseaddr + sectoroffset * i);
        }

        remainsz = (sz < (SR_TOTAL_SZ - offset)) ? sz : (SR_TOTAL_SZ - offset);
        for(i = 0; i < remainsz; i++)
        {
            backbuf[i + offset] = buf[i];
        }

        for ( i = 0; i < sectornum; i++)
        {
            for (j = 0; j < (sectorsize / SR_PROGRAM_SZE); j++)
            {
                programSR(writecmd, baseaddr + sectoroffset * i + j * SR_PROGRAM_SZE,  backbuf, SR_PROGRAM_SZE);
                backbuf += SR_PROGRAM_SZE;
            }
        }
    }



    if (SPIFLASH_MID_ESMT == flashid)
    {
        /*Write Disable*/
        M32(HR_FLASH_CMD_ADDR) = 0x4;
        M32(HR_FLASH_CMD_START) = CMD_START_Msk;
    }
}

/*sr end*/

int readByCMD(unsigned char cmd, unsigned long addr, unsigned char *buf, unsigned long sz)
{
    int i = 0;
    int word = sz / 4;
    int byte = sz % 4;
    unsigned long addr_read;

    switch (cmd)
    {
    case 0x03:
        M32(HR_FLASH_CMD_ADDR) = 0x8000C003 | (((sz - 1) & 0x3FF) << 16);
        M32(HR_FLASH_ADDR) = addr & 0x1FFFFFF;
        M32(HR_FLASH_CMD_START) = CMD_START_Msk;
        break;
    case 0x0B:
        if((M32(HR_FLASH_CR) & 0x2) == 0x2)
        {
            M32(HR_FLASH_CMD_ADDR) = 0xB400C00B | (((sz - 1) & 0x3FF) << 16);
        }
        else
        {
            M32(HR_FLASH_CMD_ADDR) = 0xBC00C00B | (((sz - 1) & 0x3FF) << 16);
        }
        M32(HR_FLASH_ADDR) = addr & 0x1FFFFFF;
        M32(HR_FLASH_CMD_START) = CMD_START_Msk;
        break;
    case 0xBB:
        M32(HR_FLASH_CMD_ADDR) = 0xE400C0BB | (((sz - 1) & 0x3FF) << 16);
        M32(HR_FLASH_ADDR) = addr & 0x1FFFFFF;
        M32(HR_FLASH_CMD_START) = CMD_START_Msk;
        break;

    case 0xEB:
        M32(HR_FLASH_CMD_ADDR) = 0xEC00C0EB | (((sz - 1) & 0x3FF) << 16);
        M32(HR_FLASH_ADDR) = addr & 0x1FFFFFF;
        M32(HR_FLASH_CMD_START) = CMD_START_Msk;
        break;


    default:
        return -1;
    }

    //	printf("delay delay delay delay\n");
    //	dumpUint32("readByCMD RSA_BASE_ADDRESS", RSA_BASE_ADDRESS, sz/4);
    addr_read = RSA_BASE_ADDRESS;
    for(i = 0; i < word; i ++)
    {
        M32(buf) = M32(addr_read);
        buf += 4;
        addr_read += 4;
    }

    if(byte > 0)
    {
        M32(buf) = M32(addr_read);
        buf += 3;							//point last byte
        byte = 4 - byte;
        while(byte)
        {
            *buf = 0;
            buf --;
            byte --;
        }
    }
    return 0;
}


int flashRead(unsigned long addr, unsigned char *buf, unsigned long sz)
{

    unsigned int flash_addr;
    unsigned int sz_pagenum = 0;
    unsigned int sz_remain = 0;
    int i = 0;
    int page_offset = addr & (INSIDE_FLS_PAGE_SIZE - 1);

    char *cache = NULL;

    cache = tls_mem_alloc(INSIDE_FLS_PAGE_SIZE);
    if (cache == NULL)
    {
        TLS_DBGPRT_ERR("allocate sector cache memory fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }
    flash_addr = addr & ~(INSIDE_FLS_PAGE_SIZE - 1);
    readByCMD(0x0B, flash_addr, (unsigned char *)cache, INSIDE_FLS_PAGE_SIZE);
    if (sz > INSIDE_FLS_PAGE_SIZE - page_offset)
    {
        MEMCPY(buf, cache + page_offset, INSIDE_FLS_PAGE_SIZE - page_offset);
        buf += INSIDE_FLS_PAGE_SIZE - page_offset;
        flash_addr 	+= INSIDE_FLS_PAGE_SIZE;

        sz_pagenum = (sz - (INSIDE_FLS_PAGE_SIZE - page_offset)) / INSIDE_FLS_PAGE_SIZE;
        sz_remain = (sz - (INSIDE_FLS_PAGE_SIZE - page_offset)) % INSIDE_FLS_PAGE_SIZE;
        for (i = 0; i < sz_pagenum; i++)
        {

            readByCMD(0x0B, flash_addr, (unsigned char *)cache, INSIDE_FLS_PAGE_SIZE);
            MEMCPY(buf, cache, INSIDE_FLS_PAGE_SIZE);
            buf 		+= INSIDE_FLS_PAGE_SIZE;
            flash_addr 	+= INSIDE_FLS_PAGE_SIZE;
        }

        if (sz_remain)
        {
            readByCMD(0x0B, flash_addr, (unsigned char *)cache, sz_remain);
            MEMCPY(buf, cache, sz_remain);
        }
    }
    else
    {
        MEMCPY(buf, cache + page_offset, sz);
    }
    tls_mem_free(cache);

    return 0;
}

/**
 * @brief           This function is used to unlock flash protect area [0x0~0x2000].
 *
 * @param	   None
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_unlock(void)
{
    return flashunlock();
}

/**
 * @brief           This function is used to lock flash protect area [0x0~0x2000].
 *
 * @param	   None
 *
 * @return         None
 *
 * @note           None
 */
int tls_flash_lock(void)
{
    return flashlock();
}


/**
 * @brief           This function is used to semaphore protect.
 *
 * @param	   None
 *
 * @return         None
 *
 * @note           None
 */
void tls_fls_sem_lock(void)
{

}


/**
 * @brief           This function is used to semaphore protect cancel.
 *
 * @param	   None
 *
 * @return         None
 *
 * @note           None
 */
void tls_fls_sem_unlock(void)
{

}


/**
 * @brief          This function is used to read data from the flash.
 *
 * @param[in]      addr                 is byte offset addr for read from the flash.
 * @param[in]      buf                   is user for data buffer of flash read
 * @param[in]      len                   is byte length for read.
 *
 * @retval         TLS_FLS_STATUS_OK	    if read sucsess
 * @retval         TLS_FLS_STATUS_EIO	    if read fail
 *
 * @note           None
 */
int tls_fls_read(u32 addr, u8 *buf, u32 len)
{
    int err = TLS_FLS_STATUS_OK;

    if (((addr & (INSIDE_FLS_BASE_ADDR - 1)) >= flsdensity) || (len == 0) || (buf == NULL)) {
        TLS_DBGPRT_ERR("addr[0x%x], len[%d], buf is NULL?[%d], getFlashDensity[0x%x]\n", addr, len,
                       buf == NULL, flsdensity);
        return TLS_FLS_STATUS_EINVAL;
    }

    err = flashRead(addr, buf, len);
    if (err != 0) {
        TLS_DBGPRT_ERR("flashRead failed, err[%d], addr[0x%x], len[%d]\n", err, addr, len);
    }

    return err;
}

/**
 * @brief          This function is used to write data to the flash.
 *
 * @param[in]      addr     is byte offset addr for write to the flash
 * @param[in]      buf       is the data buffer want to write to flash
 * @param[in]      len       is the byte length want to write
 *
 * @retval         TLS_FLS_STATUS_OK	           if write flash success
 * @retval         TLS_FLS_STATUS_EPERM	    if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	    if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	    if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO           if io error
 *
 * @note           None
 */
static int __tls_fls_write(u32 addr, u8 *buf, u32 len, u32 flag)
{
    u8 *         cache;
    unsigned int secpos;
    unsigned int secoff;
    unsigned int secremain;
    unsigned int i;
    unsigned int offaddr;
    unsigned int writepage;
    unsigned int samecnt = 0;

    if (((addr & (INSIDE_FLS_BASE_ADDR - 1)) >= flsdensity) || (len == 0) || (buf == NULL)) {
        TLS_DBGPRT_ERR("addr[0x%x], len[%d], buf is NULL?[%d], getFlashDensity[0x%x]\n", addr, len,
                       buf == NULL, flsdensity);
        return TLS_FLS_STATUS_EINVAL;
    }

    cache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
    if (cache == NULL) {
        TLS_DBGPRT_ERR("allocate sector cache memory fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }

    offaddr   = addr & (INSIDE_FLS_BASE_ADDR - 1);  //Offset of 0X08000000
    secpos    = offaddr / INSIDE_FLS_SECTOR_SIZE;   //Section addr
    secoff    = (offaddr % INSIDE_FLS_SECTOR_SIZE); //Offset in section
    secremain = INSIDE_FLS_SECTOR_SIZE - secoff;
    if (len <= secremain) {
        secremain = len; //Not bigger with remain size in section
    }
    while (1) {
        flashRead(secpos * INSIDE_FLS_SECTOR_SIZE, cache, INSIDE_FLS_SECTOR_SIZE);

        samecnt = 0;
        for (i = 0; i < secremain; i++) {
            if (cache[i + secoff] != buf[i]) {
                cache[i + secoff] = buf[i];
            } else {
                samecnt++;
            }
        }

        if (samecnt != secremain) {
            if (flag) {
                eraseSector(secpos * INSIDE_FLS_SECTOR_SIZE);
                for (i = 0; i < (INSIDE_FLS_SECTOR_SIZE / INSIDE_FLS_PAGE_SIZE); i++) {
                    programPage(secpos * INSIDE_FLS_SECTOR_SIZE + i * INSIDE_FLS_PAGE_SIZE,
                                INSIDE_FLS_PAGE_SIZE, &cache[i * INSIDE_FLS_PAGE_SIZE]); //Write
                }
            } else {
                if ((secoff + secremain) % INSIDE_FLS_PAGE_SIZE) {
                    writepage = (secoff + secremain) / INSIDE_FLS_PAGE_SIZE + 1;
                } else {
                    writepage = (secoff + secremain) / INSIDE_FLS_PAGE_SIZE;
                }

                if (internalflsID == 0x85) //for puya flash
                {
                    for (i = (secoff / INSIDE_FLS_PAGE_SIZE); i < writepage; i++) {
                        erasePage(secpos * INSIDE_FLS_SECTOR_SIZE + i * INSIDE_FLS_PAGE_SIZE);
                        programPage(secpos * INSIDE_FLS_SECTOR_SIZE + i * INSIDE_FLS_PAGE_SIZE,
                                    INSIDE_FLS_PAGE_SIZE, &cache[i * INSIDE_FLS_PAGE_SIZE]); //Write
                    }
                } else {
                    for (i = (secoff / INSIDE_FLS_PAGE_SIZE); i < writepage; i++) {
                        programPage(secpos * INSIDE_FLS_SECTOR_SIZE + i * INSIDE_FLS_PAGE_SIZE,
                                    INSIDE_FLS_PAGE_SIZE, &cache[i * INSIDE_FLS_PAGE_SIZE]); //Write
                    }
                }
            }
        }
        if (len == secremain) {
            break;
        } else {
            secpos++;
            secoff = 0;
            buf += secremain;
            len -= secremain;
            if (len > (INSIDE_FLS_SECTOR_SIZE))
                secremain = INSIDE_FLS_SECTOR_SIZE;
            else
                secremain = len; //Next section will finish
        }
    }

    tls_mem_free(cache);
    return TLS_FLS_STATUS_OK;
}

/**
 * @brief          This function is used to write data to the flash.
 *
 * @param[in]      addr     is byte offset addr for write to the flash
 * @param[in]      buf       is the data buffer want to write to flash
 * @param[in]      len       is the byte length want to write
 *
 * @retval         TLS_FLS_STATUS_OK	           if write flash success
 * @retval         TLS_FLS_STATUS_EPERM	    if flash struct point is null
 * @retval         TLS_FLS_STATUS_ENODRV	    if flash driver is not installed
 * @retval         TLS_FLS_STATUS_EINVAL	    if argument is invalid
 * @retval         TLS_FLS_STATUS_EIO           if io error
 *
 * @note           None
 */
int tls_fls_write(u32 addr, u8 *buf, u32 len)
{
    return __tls_fls_write(addr, buf, len, 1);
}

int tls_fls_write_without_erase(u32 addr, u8 *buf, u32 len)
{
    return __tls_fls_write(addr, buf, len, 0);
}

/**
 * @brief          	This function is used to erase the appoint sector
 *
 * @param[in]      	sector 	sector num of the flash, 4K byte a sector
 *
 * @retval         	TLS_FLS_STATUS_OK	    	if read sucsess
 * @retval         	other	    				if read fail
 *
 * @note           	None
 */
int tls_fls_erase(u32 sector)
{
    u32 addr;

    if (sector >= (flsdensity / INSIDE_FLS_SECTOR_SIZE + INSIDE_FLS_BASE_ADDR / INSIDE_FLS_SECTOR_SIZE))
    {
        TLS_DBGPRT_ERR("the sector to be erase overflow!\n");
        return TLS_FLS_STATUS_EINVAL;
    }
    addr = sector * INSIDE_FLS_SECTOR_SIZE;

    eraseSector(addr);
    return TLS_FLS_STATUS_OK;
}


static u8 *gsflscache = NULL;
//static u32 gsSecOffset = 0;
static u32 gsSector = 0;


/**
 * @brief          	This function is used to flush the appoint sector
 *
 * @param      	None
 *
 * @return         	None
 *
 * @note           	None
 */
static void tls_fls_flush_sector(void)
{
    int i;
    u32 addr;
    if (gsSector < (flsdensity / INSIDE_FLS_SECTOR_SIZE + INSIDE_FLS_BASE_ADDR / INSIDE_FLS_SECTOR_SIZE))
    {
        addr = gsSector * INSIDE_FLS_SECTOR_SIZE;

        eraseSector(addr);
        for (i = 0; i < INSIDE_FLS_SECTOR_SIZE / INSIDE_FLS_PAGE_SIZE; i++)
        {
            programPage(gsSector * INSIDE_FLS_SECTOR_SIZE +
                        i * INSIDE_FLS_PAGE_SIZE, INSIDE_FLS_PAGE_SIZE,
                        &gsflscache[i * INSIDE_FLS_PAGE_SIZE]);
        }
    }
    //gsSecOffset = 0;
}


/**
 * @brief          	This function is used to fast write flash initialize
 *
 * @param      	None
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail
 *
 * @note           	None
 */
int tls_fls_fast_write_init(void)
{

    if (NULL != gsflscache)
    {
        TLS_DBGPRT_ERR("tls_fls_fast_write_init installed!\n");
        return -1;
    }
    gsflscache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
    if (NULL == gsflscache)
    {
        TLS_DBGPRT_ERR("tls_fls_fast_write_init malloc err!\n");
        return -1;
    }
    return TLS_FLS_STATUS_OK;
}

/**
 * @brief          	This function is used to destroy fast write flash
 *
 * @param      	None
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_fls_fast_write_destroy(void)
{
    if (NULL != gsflscache)
    {
        tls_fls_flush_sector();
        tls_mem_free(gsflscache);
        gsflscache = NULL;
    }
}

/**
 * @brief          	This function is used to fast write data to the flash.
 *
 * @param[in]      	addr     	is byte offset addr for write to the flash
 * @param[in]      	buf       	is the data buffer want to write to flash
 * @param[in]      	length  	is the byte length want to write
 *
 * @retval         	TLS_FLS_STATUS_OK	success
 * @retval        	other				fail
 *
 * @note           	None
 */
int tls_fls_fast_write(u32 addr, u8 *buf, u32 length)
{

    u32 sector, offset, maxlen, len;

    if(((addr & (INSIDE_FLS_BASE_ADDR - 1)) >=  flsdensity) || (length == 0) || (buf == NULL))
    {
        return TLS_FLS_STATUS_EINVAL;
    }

    sector = addr / INSIDE_FLS_SECTOR_SIZE;
    offset = addr % INSIDE_FLS_SECTOR_SIZE;
    maxlen = INSIDE_FLS_SECTOR_SIZE;

    if ((sector != gsSector) && (gsSector != 0))
    {
        tls_fls_flush_sector();
    }
    gsSector = sector;
    if (offset > 0)
    {
        maxlen -= offset;
    }
    while (length > 0)
    {
        len = (length > maxlen) ? maxlen : length;
        MEMCPY(gsflscache + offset, buf, len);
        if (offset + len >= INSIDE_FLS_SECTOR_SIZE)
        {
            tls_fls_flush_sector();
            gsSector++;
        }
        offset = 0;
        maxlen = INSIDE_FLS_SECTOR_SIZE;
        sector++;
        buf += len;
        length -= len;
    }


    return TLS_FLS_STATUS_OK;
}


/**
 * @brief          	This function is used to erase flash all chip
 *
 * @param      	None
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail
 *
 * @note           	None
 */
int tls_fls_chip_erase(void)
{
    int i, j;
    u8 *cache;



    cache = tls_mem_alloc(INSIDE_FLS_SECTOR_SIZE);
    if (cache == NULL)
    {
        TLS_DBGPRT_ERR("allocate sector cache memory fail!\n");
        return TLS_FLS_STATUS_ENOMEM;
    }


    for( i = 0; i < ( flsdensity - (INSIDE_FLS_SECBOOT_ADDR & 0xFFFFFF)) / INSIDE_FLS_SECTOR_SIZE; i ++)
    {
        flashRead(INSIDE_FLS_SECBOOT_ADDR + i * INSIDE_FLS_SECTOR_SIZE, cache, INSIDE_FLS_SECTOR_SIZE);
        for (j = 0; j < INSIDE_FLS_SECTOR_SIZE; j++)
        {
            if (cache[j] != 0xFF)
            {
                eraseSector(INSIDE_FLS_SECBOOT_ADDR + i * INSIDE_FLS_SECTOR_SIZE);
                break;
            }
        }
    }

    tls_mem_free(cache);

    return TLS_FLS_STATUS_OK;
}


/**
 * @brief          	This function is used to get flash param
 *
 * @param[in]      	type		the type of the param need to get
 * @param[out]     	param	point to addr of out param
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail
 *
 * @note           	None
 */
int tls_fls_get_param(u8 type, void *param)
{
    int err;

    if (param == NULL)
    {
        return TLS_FLS_STATUS_EINVAL;
    }
    err = TLS_FLS_STATUS_OK;
    switch (type)
    {
    case TLS_FLS_PARAM_TYPE_ID:
        *((u32 *) param) = 0x2013;
        break;

    case TLS_FLS_PARAM_TYPE_SIZE:
        *((u32 *) param) = flsdensity;
        break;

    case TLS_FLS_PARAM_TYPE_PAGE_SIZE:
        *((u32 *) param) = INSIDE_FLS_PAGE_SIZE;
        break;

    case TLS_FLS_PARAM_TYPE_PROG_SIZE:
        *((u32 *) param) = INSIDE_FLS_PAGE_SIZE;
        break;

    case TLS_FLS_PARAM_TYPE_SECTOR_SIZE:
        *((u32 *) param) = INSIDE_FLS_SECTOR_SIZE;
        break;

    default:
        TLS_DBGPRT_WARNING("invalid parameter ID!\n");
        err = TLS_FLS_STATUS_EINVAL;
        break;
    }
    return err;
}

/**
 * @brief          	This function is used to initialize the flash module
 *
 * @param      	None
 *
 * @retval         	TLS_FLS_STATUS_OK	    	sucsess
 * @retval         	other	    				fail
 *
 * @note           	None
 */
int tls_fls_init(void)
{
    internalflsID = readRID();
    flsdensity    = getFlashDensity();
    return TLS_FLS_STATUS_OK;
}

int tls_fls_exit(void)
{
    TLS_DBGPRT_FLASH_INFO("Not support flash driver module uninstalled!\n");
    return TLS_FLS_STATUS_EPERM;
}

/**
 * @brief          	This function is used to initialize system parameter postion by flash density
 *
 * @param      	None
 *
 * @retval         	None
 *
 * @note           	must be called before function tls_param_init
 */
void tls_fls_sys_param_postion_init(void)
{
    unsigned int density = 0;
    int err;
    err = tls_fls_get_param(TLS_FLS_PARAM_TYPE_SIZE, (void *)&density);
    if (TLS_FLS_STATUS_OK == err)
    {
        TLS_FLASH_END_ADDR            = (FLASH_BASE_ADDR|density) - 1;
        TLS_FLASH_OTA_FLAG_ADDR       = (FLASH_BASE_ADDR|density) - 0x1000;
        TLS_FLASH_PARAM_RESTORE_ADDR  =	(FLASH_BASE_ADDR|density) - 0x2000;
        TLS_FLASH_PARAM2_ADDR 		  =	(FLASH_BASE_ADDR|density) - 0x3000;
        TLS_FLASH_PARAM1_ADDR 		  =	(FLASH_BASE_ADDR|density) - 0x4000;
        TLS_FLASH_PARAM_DEFAULT	      =	(FLASH_BASE_ADDR|density) - 0x5000;
    }
    else
    {
        TLS_DBGPRT_ERR("system parameter postion use default!\n");
    }
}

