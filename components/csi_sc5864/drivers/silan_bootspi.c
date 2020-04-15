#include "silan_types.h"
#include "silan_bootspi_regs.h"
#include "silan_bootspi.h"
#include "silan_errno.h"
#include "silan_printf.h"
#include "silan_pmu.h"
#include "silan_irq.h"
#include <drv/spiflash.h>
#include <soc.h>
#include <string.h>
#include <csi_core.h>

#if defined(__CC_ARM)
#include "silan_m0_cache.h"
#endif

#ifdef BOOTSPI_SUPPORT_CACHELOCK
extern unsigned char Image$$BOOTSPI_ROM$$Base;
extern unsigned char Image$$BOOTSPI_ROM$$Length;

static uint32_t restored;
#endif

#define ERR_SPIFLASH(errno) (CSI_DRV_ERRNO_SPIFLASH_BASE | errno)

#define SPIFLASH_NULL_PARAM_CHK(para)           \
    do {                                        \
        if (para == NULL) {                     \
            return ERR_SPIFLASH(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

#define SPIFLASH_ZERO_PARAM_CHK(para)           \
    do {                                        \
        if (para == 0) {                     \
            return ERR_SPIFLASH(DRV_ERROR_PARAMETER);   \
        }                                       \
    } while (0)

spi_struct_t *spi_st = NULL;
bootspi_t bootspi;
extern int32_t target_get_spiflash(int32_t idx, spiflash_info_t *info);
extern int32_t target_spiflash_init(int32_t idx, uint32_t *base, uint32_t *irq);

static const spiflash_capabilities_t driver_capabilities = {
    1, /* event_ready */
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};
static uint8_t spiflash_init_flag = 0;

typedef struct {
    spiflash_info_t spiflashinfo;
    spiflash_event_cb_t cb;
    spiflash_status_t status;
} ck_spiflash_priv_t;

static ck_spiflash_priv_t s_spiflash_handle[CONFIG_SPIFLASH_NUM];

void ex_spiflash_power_recover_enable(uint8_t flash_recover_en)
{
    silan_spiflash_power_enable();
    spi_st->cache_sub_config &= ~CACHE_CRC_LIMIT_ERR_NUM(0x3f);
    spi_st->cache_sub_config |=CACHE_CRC_LIMIT_ERR_NUM(0xf);
    spi_st->cache_sub_config |=CACHE_CRC_ERR_OUT_EN;
    spi_st->flash_power_down =  0xffff;  //don't write 0
    spi_st->flash_power_up  = 0xffff;  //!0
    spi_st->cache_config |=CACHE_SET_CFG;
    if(flash_recover_en)
        spi_st->main_ctrl |= (FLASH_PWD_RESUME_EN|SPICACHE_SYSRST_EN);
    else
        spi_st->main_ctrl &= ~(FLASH_PWD_RESUME_EN|SPICACHE_SYSRST_EN);
}

void ex_spi_crc_enable(uint32_t addr)
{
    spi_st->cache_crc_addr = addr;
    spi_st->cache_sub_config |=CACHE_CRC_ENABLE;
    spi_st->cache_config |=CACHE_SET_CFG;
}

static void cache_xwire_sel(uint8_t type,uint8_t wire)
{
    switch(type)
    {
            case Cache_Cmd:
                if(wire == 1) {
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_CMD_WIREMODE(3);
                }else if(wire == 2){
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_CMD_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->cache_config |=  CACHE_CMD_WIREMODE(1);
                } else if(wire == 4) {
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_CMD_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->cache_config |=  CACHE_CMD_WIREMODE(2);
                }
                break;
            case Cache_Addr:
                if(wire == 1) {
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_ADDR_WIREMODE(3);
                }else if(wire == 2){
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_ADDR_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->cache_config |=  CACHE_ADDR_WIREMODE(1);
                }else if(wire ==4 ) {
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_ADDR_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->cache_config |=  CACHE_ADDR_WIREMODE(2);
                }
                break;
            case Cache_Data:
                if(wire ==1 ) {
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_DATA_WIREMODE(3);
                }else if(wire == 2){
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_DATA_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->cache_config |= CACHE_DATA_WIREMODE(1);
                }else if(wire == 4){
                        spi_st->key=KEY2;
                        spi_st->cache_config &= ~CACHE_DATA_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->cache_config |= CACHE_DATA_WIREMODE(2);
                }
                break;
    }
}

void cache_spi_cmd_sel(uint8_t spi_cmd)
{
    unsigned int tmp;
/*
test
*/
    spi_st->cache_config &=~CACHE_DUMMY_CLK(0xff);
    spi_st->cache_config &=~CAHCE_DTR_MODE;
    switch(spi_cmd)
    {
        case 0x0d:
                spi_st->cache_config |=CACHE_DUMMY_CLK(6);

              tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;

                spi_st->cache_config |=CAHCE_DTR_MODE;
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,1);
                cache_xwire_sel(Cache_Data,1);
            break;
        case 0xbd:
                spi_st->cache_config |=CACHE_DUMMY_CLK(6);

                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;

                spi_st->cache_config |=CAHCE_DTR_MODE;
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,2);
                cache_xwire_sel(Cache_Data,2);
            break;
        case 0xed:
                spi_st->cache_config |=CACHE_DUMMY_CLK(8);

                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;

                spi_st->cache_config |=CAHCE_DTR_MODE;
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,4);
                cache_xwire_sel(Cache_Data,4);
            break;
        case 0x03:

                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,1);
                cache_xwire_sel(Cache_Data,1);
            break;
        case 0x0b:
                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;

                spi_st->cache_config |=CACHE_DUMMY_CLK(8);
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,1);
                cache_xwire_sel(Cache_Data,1);
            break;
        case 0x3b:
                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;

                spi_st->cache_config |=CACHE_DUMMY_CLK(8);
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,1);
                cache_xwire_sel(Cache_Data,2);
            break;
        case 0x6b:
                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;

                spi_st->cache_config |=CACHE_DUMMY_CLK(8);
                cache_xwire_sel(Cache_Cmd,1);
                cache_xwire_sel(Cache_Addr,1);
                cache_xwire_sel(Cache_Data,4);
            break;
        case  0xeb:
                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;
#if 0
                spi_st->cache_sub_config &= ~CACHE_CRM_DATA(0xff);
                spi_st->cache_sub_config |= CACHE_CRM_DATA(0x20);
            spi_st->cache_config |= CACHE_CRM_EXIST|CACHE_CRM_NOCMD;
                spi_st->cache_config |= CACHE_DUMMY_CLK(0x4);//2*2
#else
            spi_st->cache_config |= CACHE_DUMMY_CLK(0x6);//2*2
#endif
                cache_xwire_sel(Cache_Addr,4);
                cache_xwire_sel(Cache_Data,4);
                break;
        case 0xe3:
                tmp = spi_st->cache_sub_config;
                tmp &=~CACHE_CMD(0xff);
                tmp |=CACHE_CMD(spi_cmd);
                spi_st->cache_sub_config = tmp;
#if 0
                spi_st->cache_sub_config &= ~CACHE_CRM_DATA(0xff);
                spi_st->cache_sub_config |= CACHE_CRM_DATA(0x20);
                spi_st->cache_config |= CACHE_CRM_EXIST|CACHE_CRM_NOCMD;;
#else
                spi_st->cache_config |= CACHE_DUMMY_CLK(0x2);
#endif
                cache_xwire_sel(Cache_Addr,4);
                cache_xwire_sel(Cache_Data,4);
                break;
        default:
            break;
    }
    spi_st->cache_config |=CACHE_SET_CFG;
    tmp = spi_st->spi_sub_config;
    tmp &= ~SPI_CLK_DIV(7);
    tmp |= SPI_CLK_DIV(1);
    if(!((spi_cmd ==0x0d)||(spi_cmd ==0xbd)||(spi_cmd ==0xed))){
        if(!(tmp&SPI_CLK_DIV(0))) {
            tmp |= SPI_RCV_CPOL;  //can't open in dtr mode
        }
    }
    tmp |= SPI_SET_CFG;
    tmp &= ~SPI_IDLE_NUM(0x3ff);
    tmp |= SPI_IDLE_NUM(0x10);
    spi_st-> spi_sub_config = tmp;

}

static void spi_xwire_sel(unsigned char Type,unsigned char Wire)
{
    switch(Type)
    {
            case Host_Cmd:
                    if(Wire==1) {
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_CMD_WIREMODE(3);}
                    else if(Wire==2){
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_CMD_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->spi_config |=  SPI_CMD_WIREMODE(1);}
                    else if(Wire==4) {
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_CMD_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->spi_config |=  SPI_CMD_WIREMODE(2); }
                    break;
            case Host_Addr:
                    if(Wire==1) {
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_ADDR_WIREMODE(3);}
                    else if(Wire==2){
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_ADDR_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->spi_config |=  SPI_ADDR_WIREMODE(1);}
                    else if(Wire==4) {
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_ADDR_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->spi_config |=  SPI_ADDR_WIREMODE(2); }
                    break;
            case Host_Data:
                    if(Wire==1) {
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_DATA_WIREMODE(3);}
                    else if(Wire==2){
                        spi_st->key=KEY2;
                        spi_st->spi_config &= ~SPI_DATA_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->spi_config |= SPI_DATA_WIREMODE(1);}
                    else if(Wire==4){
                        spi_st->key=KEY2;
                         spi_st->spi_config &= ~SPI_DATA_WIREMODE(3);
                        spi_st->key=KEY2;
                        spi_st->spi_config |= SPI_DATA_WIREMODE(2); }
                    break;
    }
}
void spi_flash_init(uint8_t spiclk_sel,uint8_t if_preread,uint8_t if_rcv_cpol)
{
        spi_st->spi_sub_config &= ~SPI_CLK_DIV(0x7);
        spi_st->spi_sub_config |= SPI_CLK_DIV(spiclk_sel);

        if(if_preread)
            spi_st->spi_sub_config &= ~NO_PROREADING;
        else
            spi_st->spi_sub_config |= NO_PROREADING;
        if(if_rcv_cpol)
            spi_st->spi_sub_config |= SPI_RCV_CPOL;
        else
            spi_st->spi_sub_config &= ~SPI_RCV_CPOL;

        spi_st->spi_sub_config |= SPI_SET_CFG;
        spi_st->key = KEY1;
        spi_st->main_ctrl |= HOST_NOT_CACHE;
}

void spi_flash_uninit(void)
{
        spi_st->spi_sub_config &= ~NO_PROREADING;
        spi_st->spi_sub_config &= ~SPI_RCV_CPOL;
        spi_st->spi_sub_config |= SPI_SET_CFG;
        spi_st->main_ctrl &= ~HOST_NOT_CACHE;
}


static void  spi_read_mode_sel(uint8_t read_cmd,uint32_t addr)
{
    switch (read_cmd)
        {
             case READ:
                    spi_st->spi_cmd = SPI_OP_CMD(0x3);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0);
//                  spi_st->spi_config |= SPI_ADDR_XBYTE(3);
                    break;
            case 0x13:
                    spi_st->spi_cmd = SPI_OP_CMD(0x13);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0);
                    break;
            case FASTREAD:
                    spi_st->spi_cmd = SPI_OP_CMD(0xb);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x8);
                    break;
            case 0x8b:
                    spi_st->spi_cmd = SPI_OP_CMD(0xb);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x2);
                    spi_xwire_sel(Host_Addr,4);
                    spi_xwire_sel(Host_Data,4);
                    break;
            case DUALREAD:
                    spi_st->spi_cmd = SPI_OP_CMD(0x3b);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x8);
                    spi_xwire_sel(Host_Data,2);
                    break;
            case QUADREAD:
//                  SPI_FLASH_write_status_reg2(0x02);
                    spi_st->spi_cmd = SPI_OP_CMD(0x6b);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x8);
                    spi_xwire_sel(Host_Data,4);
                    break;
            case DUALIOREAD:
                    spi_st->spi_cmd = SPI_OP_CMD(0xbb);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x4);//4*1
                    spi_xwire_sel(Host_Addr,2);
                    spi_xwire_sel(Host_Data,2);
                    break;
            case  QUADIOREAD:
                    spi_st->spi_cmd = SPI_OP_CMD(0xeb);
                //  spi_st->spi_config &= ~HOST_CRM_DATA(0xff);
                //  spi_st->spi_config |= HOST_CRM_DATA(0x20);
                //  spi_st->spi_config |= HOST_CRM_EXIST;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x6);//3*2
                //  spi_st->spi_config |= SPI_DUMMY_CLK(0x4);//2*2
                    spi_xwire_sel(Host_Addr,4);
                    spi_xwire_sel(Host_Data,4);
                    break;
            case  WORDQUADREAD:
                    spi_st->spi_cmd = SPI_OP_CMD(0xe7);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x4);//2*2
                    spi_xwire_sel(Host_Addr,4);
                    spi_xwire_sel(Host_Data,4);

                    break;
            case  OCTALWORDQUADREAD:
                    spi_st->spi_cmd = SPI_OP_CMD(0xe3);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x2);//2*1
                    spi_xwire_sel(Host_Addr,4);
                    spi_xwire_sel(Host_Data,4);
                    break;
            case  0x92:
                    spi_st->spi_cmd = SPI_OP_CMD(0x92);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x4);//??? 4*2
                    spi_xwire_sel(Host_Addr,2);
                    spi_xwire_sel(Host_Data,2);
                    break;
            case  0x94:
                    spi_st->spi_cmd = SPI_OP_CMD(0x94);
                    spi_st->key = KEY2;
                    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
                    spi_st->spi_config |= SPI_DUMMY_CLK(0x6);//??? 2*3
                    spi_xwire_sel(Host_Addr,4);
                    spi_xwire_sel(Host_Data,4);
                    break;

            default:
                    break;
    }
    spi_st->spi_addr = SPI_OP_ADDR(addr);
    spi_st->spi_subctrl &= ~HOST_DATA_NUM(3);
    spi_st->spi_subctrl |= HOST_DATA_NUM(3);


}
#if 1
static int spi_flash_read_u32(uint32_t addr, uint32_t *data, uint32_t num, uint8_t read_cmd,uint32_t fast_mode)
{
    uint32_t i;
    spi_flash_init(bootspi.clk,bootspi.cfg&BOOTSPI_CFG_PROREAD,bootspi.cfg&BOOTSPI_CFG_RCV_CPOL);
    spi_read_mode_sel(read_cmd,addr);
        while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    if(fast_mode){
        spi_st->spi_ctrl |= FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_WREQ;
        spi_st->spi_ctrl |= HOST_RREQ;
    }
    else{
        spi_st->spi_ctrl &= ~FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_WREQ;
        spi_st->spi_ctrl |= HOST_RREQ;
    }
    /*8bit传输*/
/*
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->spi_subctrl &= ~HOST_DATA_NUM(3);
    spi_st->spi_subctrl |= HOST_DATA_NUM(0);
*/

    if(fast_mode)
            spi_st->spi_subctrl |= HOST_RREADY;

    for (i=0; i<num; i++){
        if(fast_mode==0x00){
            spi_st->spi_subctrl |= HOST_RREADY;
            while ((spi_st->spi_subctrl & HOST_RVALID) == 0);
            while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
        }
            data[i] = spi_st->spi_rdata;
    }
    while ((spi_st->spi_subctrl & HOST_RVALID) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);

    spi_xwire_sel(Host_Data,1);
    spi_xwire_sel(Host_Addr,1);
    spi_xwire_sel(Host_Cmd,1);

    spi_st->key = KEY2;
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);

    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_RREQ;
    spi_flash_uninit();
    return 0;
}
#endif
int spi_flash_read_u8(uint32_t addr, uint8_t *data, uint32_t num, uint8_t read_cmd,uint32_t fast_mode)
{
    unsigned int i;

    spi_flash_init(bootspi.clk,bootspi.cfg&BOOTSPI_CFG_PROREAD,bootspi.cfg&BOOTSPI_CFG_RCV_CPOL);
    spi_read_mode_sel(read_cmd,addr);
    if(fast_mode){
        spi_st->spi_ctrl |= FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_WREQ;
        spi_st->spi_ctrl |= HOST_RREQ;
    }
    else{
        spi_st->spi_ctrl &= ~FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_WREQ;
        spi_st->spi_ctrl |= HOST_RREQ;
    }

    /*8bit传输*/
        while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->spi_subctrl &= ~HOST_DATA_NUM(3);
    spi_st->spi_subctrl |= HOST_DATA_NUM(0);
    if(fast_mode)
        spi_st->spi_subctrl |= HOST_RREADY;

    for (i=0; i<num; i++){
        if(fast_mode==0x00){
             spi_st->spi_subctrl |= HOST_RREADY;
             while ((spi_st->spi_subctrl & HOST_RVALID) == 0);
             while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
        }

        data[i] = spi_st->spi_rdata;
    }
    while ((spi_st->spi_subctrl & HOST_RVALID) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);

    spi_xwire_sel(Host_Data,1);
    spi_xwire_sel(Host_Addr,1);
    spi_xwire_sel(Host_Cmd,1);
    spi_st->key = KEY2;
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);

    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_RREQ;
    spi_flash_uninit();
    return 0;
}


static uint32_t spiflash_read_status_low(void)
{
    unsigned int rdata = 0;
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x05);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_RREQ;
    spi_st->spi_subctrl |= HOST_RREADY;
    while ((spi_st->spi_subctrl & HOST_RVALID) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    rdata = spi_st->spi_rdata;
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_RREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return rdata;
}



static uint32_t spiflash_read_status_high(void)
{
    uint32_t rdata = 0;
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x35);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_RREQ;
    spi_st->spi_subctrl |= HOST_RREADY;
    while ((spi_st->spi_subctrl & HOST_RVALID) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    rdata = spi_st->spi_rdata;
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_RREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return rdata;
}
static int spi_status_done(void)
{
    uint32_t flash_status =0;
    uint32_t retry = 100000;
    flash_status = spiflash_read_status_low();
    while(retry){
        if((flash_status & 0x1)  == 0){
            return 0;
        }
        flash_status = spiflash_read_status_low();
        retry --;
    }
    return -EAGAIN;
}
static int spi_write_en_done(void)
{
    uint32_t flash_status =0;
    uint32_t retry = 10000;
    flash_status = spiflash_read_status_low();
    while(retry){
            if((flash_status & 0x2)  == 0x2){
                     return 0;
            }
            flash_status = spiflash_read_status_low();
            retry --;
    }
    return -EAGAIN;
}

static int spiflash_write_enable(void)
{
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(6);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return (spi_write_en_done());
}

int spiflash_otp_mode_set(void)
{
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x3a);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return 0;
}
int spiflash_otp_mode_exit(void)
{
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x04);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return 0;
}

void spiflash_write_disable(void)
{
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(4);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
}
void spiflash_write_sr_enable(void)
{
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x50);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;
    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
}
#if 0
static int spi_flash_write_status_reg1(uint8_t reg_val)
{
    int g_result;
    spiflash_write_enable();
    spiflash_write_sr_enable();
    g_result =spi_status_done();
    if(g_result)
            return g_result;

    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x01);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->spi_wdata = reg_val;
    spi_st->spi_subctrl |= HOST_WVALID;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return 0;
}
#endif
static int spi_flash_write_status_reg2(uint8_t reg_val)
{
    int g_result;
    spiflash_write_enable();
    spiflash_write_sr_enable();
    g_result = spi_status_done();
    if(g_result)
        return g_result;

    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x31);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->spi_wdata = (unsigned char)(reg_val);
    spi_st->spi_subctrl |= HOST_WVALID;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return 0;
}

int spi_quad_status_done(void)
{
    unsigned int flash_status =0;
      unsigned int retry = 10;
    //spiflash_otp_mode_set();
    flash_status = spiflash_read_status_high();
    SL_DBGLOG("status_reg2=%x", flash_status);

    while(retry){
        if((flash_status & 0x2)  == 0x2){
             return 0;
        }
        spi_flash_write_status_reg2(0x2);
        flash_status = spiflash_read_status_high();
        SL_DBGLOG("status_reg2=%x", flash_status);
        retry--;
    }
//    spiflash_otp_mode_exit();
    return -EAGAIN;
}

int spi_flash_enter_qpi_mode(void)
{
    int g_result;
    spiflash_write_disable();
    g_result = spi_status_done();
    if(g_result)
        return g_result;

    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0x38);
    spi_st->spi_subctrl &= ~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_xwire_sel(Host_Addr,4);
    spi_xwire_sel(Host_Data,4);
    spi_xwire_sel(Host_Cmd,4);
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    return 0;
}
void spi_flash_exit_qpi_mode(void)
{
    spiflash_write_disable();

    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(0xFF);
    spi_st->spi_subctrl &=~HOST_DATA_NUM(3);
    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
}

static int spi_flash_pagewrite_u32(uint32_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite,uint32_t fast_mode)
{
    uint32_t i;
    int g_result;

    spi_flash_init(bootspi.clk,bootspi.cfg&BOOTSPI_CFG_PROREAD,bootspi.cfg&BOOTSPI_CFG_RCV_CPOL);
    spiflash_write_enable();
    g_result = spi_status_done();
    if(g_result){
            spi_flash_uninit();
            return g_result;
    }
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(2) ;
    spi_st->spi_addr =  SPI_OP_ADDR(WriteAddr);
    spi_st->spi_subctrl |= HOST_DATA_NUM(3);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    if(fast_mode){
        spi_st->spi_ctrl = FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_RREQ;
        spi_st->spi_ctrl |= HOST_WREQ;
    }else{
        spi_st->spi_ctrl &= ~FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_RREQ;
        spi_st->spi_ctrl |= HOST_WREQ;
        while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
        while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    }
    for (i=0; i<NumByteToWrite; i++){
            spi_st->spi_wdata = pBuffer[i];
            if(fast_mode==0x00){
                spi_st->spi_subctrl |= HOST_WVALID;
                while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
                while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
            }
    }
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    g_result = spi_status_done();
    spi_flash_uninit();
    if(g_result)
            return g_result;
    return 0;
}

int spi_flash_pagewrite_u8(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite,uint32_t fast_mode)
{
    int g_result;
    unsigned int i;
    spi_flash_init(bootspi.clk,bootspi.cfg&BOOTSPI_CFG_PROREAD,bootspi.cfg&BOOTSPI_CFG_RCV_CPOL);

    spiflash_write_enable();
    g_result = spi_status_done();
    if(g_result)
    {
        spi_flash_uninit();
        return g_result;
    }
    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(2) ;
    spi_st->spi_addr =  SPI_OP_ADDR(WriteAddr);
    spi_st->spi_subctrl |= HOST_DATA_NUM(3);

    spi_st->key = KEY2;

    if(fast_mode){
        spi_st->spi_ctrl = FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_RREQ;
        spi_st->spi_ctrl |= HOST_WREQ;
    }else{
        spi_st->spi_ctrl &= ~FAST_READ_WRITE;
        spi_st->spi_ctrl &= ~HOST_RREQ;
        spi_st->spi_ctrl |= HOST_WREQ;
        while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
        while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    }
    spi_st->spi_subctrl &= ~HOST_DATA_NUM(3);
    spi_st->spi_subctrl |= HOST_DATA_NUM(0);
    for (i=0; i<NumByteToWrite - 10; i++){
        spi_st->spi_wdata = pBuffer[i];
        if(fast_mode==0x00){
        spi_st->spi_subctrl |= HOST_WVALID;
            while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
            while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
        }
    }
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    g_result = spi_status_done();
    spi_flash_uninit();
    if(g_result)
        return g_result;
    return 0;
}
static int spi_flash_sectorerase(uint32_t SectorAddr)
{
    int g_result;
    spi_flash_init(bootspi.clk,bootspi.cfg&BOOTSPI_CFG_PROREAD,bootspi.cfg&BOOTSPI_CFG_RCV_CPOL);
    spiflash_write_enable();
    g_result = spi_status_done();
    if(g_result){
        spi_flash_uninit();
        return g_result;
    }

    spi_st->main_ctrl |= HOST_NOT_CACHE;
    spi_st->spi_cmd = SPI_OP_CMD(SE);
    spi_st->spi_addr = SPI_OP_ADDR(SectorAddr);
    spi_st->spi_subctrl |= HOST_DATA_NUM(3);

    spi_st->spi_config &= ~SPI_DUMMY_CLK(0xff);
    spi_st->key = KEY2;

    csi_dcache_clean_invalid_range((uint32_t *)spi_st, sizeof(struct spi_struct));
    spi_st->spi_ctrl = HOST_WREQ;
    while ((spi_st->spi_subctrl & HOST_WREADY) == 0);
    while ((spi_st->spi_subctrl & HOST_SPI_TRANS_IDLE) == 0);
    spi_st->key = KEY2;
    spi_st->spi_ctrl &= ~HOST_WREQ;
    spi_st->main_ctrl &= ~HOST_NOT_CACHE;
    g_result = spi_status_done();
    spi_flash_uninit();
    if(g_result)
        return g_result;
    return 0;
}

#ifdef BOOTSPI_SUPPORT_CACHELOCK
void bootspi_cachelock_init(void)
{
    uint32_t RoBase, RoLength, Remain;

    restored = pic_get_enable();
    pic_clr_enable(restored);
    //SL_LOG("restored = %08x", restored);

    RoBase = ((int)&Image$$BOOTSPI_ROM$$Base) & 0xFFFFFFF0;
    Remain = RoLength - RoBase;
    RoLength = ((int)&Image$$BOOTSPI_ROM$$Length + Remain + M0_CACHE_LINE_SIZE - 1) / M0_CACHE_LINE_SIZE;
    //SL_LOG("BASE = %08x", RoBase);
    //SL_LOG("LEGTH = %08x", RoLength);

    m0_cache_init(CACHE_DISABLE, 0, 0, 0);
    m0_cache_init(CACHE_ENABLE, 1, RoBase, RoLength);
}

void bootspi_cachelock_quit(void)
{
    m0_cache_init(CACHE_DISABLE, 0, 0, 0);
    m0_cache_init(CACHE_ENABLE, 0, 0, 0);

    pic_set_enable(restored);
}
#endif

/**
  \brief       Get driver capabilities.
  \param[in]   spiflash handle to operate.
  \return      \ref spiflash_capabilities_t
*/
spiflash_capabilities_t csi_spiflash_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        spiflash_capabilities_t ret;
        memset(&ret, 0, sizeof(spiflash_capabilities_t));
        return ret;
    }

    return driver_capabilities;
}

/**
  \brief       Initialize SPIFLASH Interface. 1. Initializes the resources needed for the SPIFLASH interface 2.registers event callback function
  \param[in]   handle  spiflash handle to operate.
  \param[in]   cb_event  Pointer to \ref spiflash_event_cb_t
  \return      \ref execution_status
*/
spiflash_handle_t csi_spiflash_initialize(int32_t idx, spiflash_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_SPIFLASH_NUM) {
        return NULL;
    }

    ck_spiflash_priv_t *spiflash_priv = &s_spiflash_handle[idx];

    spiflash_info_t info;
    int32_t real_idx = target_get_spiflash(idx, &info);

    if (idx != real_idx) {
        return NULL;
    }

    spiflash_priv->spiflashinfo.start = info.start;
    spiflash_priv->spiflashinfo.end = info.end;
    spiflash_priv->spiflashinfo.sector_count = info.sector_count;

    /* initialize the spiflash context */
    spiflash_priv->cb = cb_event;
    spiflash_priv->status.busy = 0;
    spiflash_priv->status.error = 0U;
    spiflash_priv->spiflashinfo.sector_size = 0x1000;
    spiflash_priv->spiflashinfo.page_size = 0x100;
    spiflash_priv->spiflashinfo.program_unit = 1;
    spiflash_priv->spiflashinfo.erased_value = 0xff;

    spiflash_init_flag = 1;
    uint32_t base = 0u;
    uint32_t irq;

    real_idx = target_spiflash_init(idx, &base, &irq);

    if (real_idx != idx) {
        return NULL;
    }
    spi_st = (spi_struct_t *)base;
    //bootspi.cfg = BOOTSPI_CFG_FASTMODE | BOOTSPI_CFG_RCV_CPOL | BOOTSPI_CFG_PROREAD;
    bootspi.cfg = BOOTSPI_CFG_FASTMODE | BOOTSPI_CFG_RCV_CPOL | BOOTSPI_CFG_PROREAD;
    bootspi.clk = 2;

    return (spiflash_handle_t)spiflash_priv;
}

/**
  \brief       De-initialize SPIFLASH Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle  spiflash handle to operate.
  \return      \ref execution_status
*/
int32_t csi_spiflash_uninitialize(spiflash_handle_t handle)
{
    SPIFLASH_NULL_PARAM_CHK(handle);

    ck_spiflash_priv_t *spiflash_priv = (ck_spiflash_priv_t *)handle;
    spiflash_init_flag = 0;
    spiflash_priv->cb = NULL;

    return 0;
}

/**
  \brief       Read data from Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or \ref execution_status
*/
int32_t csi_spiflash_read(spiflash_handle_t handle, uint32_t addr, void *data, uint32_t cnt)
{
    SPIFLASH_NULL_PARAM_CHK(handle);
    SPIFLASH_NULL_PARAM_CHK(data);
    SPIFLASH_ZERO_PARAM_CHK(cnt);

    ck_spiflash_priv_t *spiflash_priv = handle;

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr || spiflash_priv->spiflashinfo.start > (addr + cnt - 1) || spiflash_priv->spiflashinfo.end < (addr + cnt - 1)) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }
    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;
    uint32_t offset = addr - spiflash_priv->spiflashinfo.start ;

    spi_flash_read_u8(offset, data, cnt, DUALREAD, bootspi.cfg & BOOTSPI_CFG_FASTMODE);

    //spi_flash_read_u32(offset, data, cnt / 4, FASTREAD, bootspi.cfg & BOOTSPI_CFG_FASTMODE);
    spiflash_priv->status.busy = 0U;

    return cnt;
}

/**
  \brief       Program data to Flash.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash..
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or \ref execution_status
*/
int32_t csi_spiflash_program(spiflash_handle_t handle, uint32_t addr, const void *data, uint32_t cnt)
{
    SPIFLASH_NULL_PARAM_CHK(handle);
    SPIFLASH_NULL_PARAM_CHK(data);
    SPIFLASH_ZERO_PARAM_CHK(cnt);

    ck_spiflash_priv_t *spiflash_priv = handle;

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr || spiflash_priv->spiflashinfo.start > (addr + cnt - 1) || spiflash_priv->spiflashinfo.end < (addr + cnt - 1)) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    int32_t i;

    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;

    uint32_t offset = addr - spiflash_priv->spiflashinfo.start;
    uint8_t *src = (uint8_t *)data;
    uint32_t program_length = 0;

    uint8_t tmp[W25Q64FV_PAGE_SIZE];

    uint32_t first_offset = offset % W25Q64FV_PAGE_SIZE;
    uint32_t first_page_space = W25Q64FV_PAGE_SIZE - first_offset;
    if (first_page_space == W25Q64FV_PAGE_SIZE) {
        first_page_space = 0;
    }

    if (first_page_space > cnt) {
        first_page_space = cnt;
    }

    if (first_page_space != 0) {
        spi_flash_read_u32((offset / W25Q64FV_PAGE_SIZE) * W25Q64FV_PAGE_SIZE, (uint32_t *)tmp, W25Q64FV_PAGE_SIZE / 4, FASTREAD, bootspi.cfg & BOOTSPI_CFG_FASTMODE);
        memcpy(&tmp[first_offset], src, first_page_space);
        spi_flash_pagewrite_u32((uint32_t *)tmp, (offset / W25Q64FV_PAGE_SIZE) * W25Q64FV_PAGE_SIZE, W25Q64FV_PAGE_SIZE / 4, bootspi.cfg & BOOTSPI_CFG_FASTMODE);
    }

    program_length += first_page_space;

    offset += first_page_space;
    src += first_page_space;
    cnt -= first_page_space;


    for (i = 0; i < cnt / W25Q64FV_PAGE_SIZE; i++) {
        spi_flash_pagewrite_u32((uint32_t *)src, offset, W25Q64FV_PAGE_SIZE / 4, bootspi.cfg & BOOTSPI_CFG_FASTMODE);
        program_length += W25Q64FV_PAGE_SIZE;
        offset += W25Q64FV_PAGE_SIZE;
        src += W25Q64FV_PAGE_SIZE;

    }
    cnt -= (cnt / W25Q64FV_PAGE_SIZE) *W25Q64FV_PAGE_SIZE;


    if (cnt) {
        spi_flash_read_u32(offset, (uint32_t *)tmp, W25Q64FV_PAGE_SIZE / 4, FASTREAD, bootspi.cfg & BOOTSPI_CFG_FASTMODE);

        memcpy(tmp, src, cnt);
        spi_flash_pagewrite_u32((uint32_t *)tmp, offset , W25Q64FV_PAGE_SIZE / 4, bootspi.cfg & BOOTSPI_CFG_FASTMODE);
        program_length += cnt;
    }
    spiflash_priv->status.busy = 0U;

    return program_length;
}

/**
  \brief       Erase Flash Sector.
  \param[in]   handle  spiflash handle to operate.
  \param[in]   addr  Sector address
  \return      \ref execution_status
*/
int32_t csi_spiflash_erase_sector(spiflash_handle_t handle, uint32_t addr)
{
    SPIFLASH_NULL_PARAM_CHK(handle);

    ck_spiflash_priv_t *spiflash_priv = handle;

    if (spiflash_priv->spiflashinfo.start > addr || spiflash_priv->spiflashinfo.end < addr) {
        return ERR_SPIFLASH(DRV_ERROR_PARAMETER);
    }

    uint32_t offset = addr - spiflash_priv->spiflashinfo.start;

    if (spiflash_priv->status.busy) {
        return ERR_SPIFLASH(DRV_ERROR_BUSY);
    }

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    spiflash_priv->status.busy = 1U;
    spiflash_priv->status.error = 0U;
    spi_flash_sectorerase(offset);

    spiflash_priv->status.busy = 0U;

    if (spiflash_priv->cb) {
        spiflash_priv->cb(0, SPIFLASH_EVENT_READY);
    }

    return 0;

}

/**
  \brief       Erase complete Flash.
  \param[in]   handle  spiflash handle to operate.
  \return      \ref execution_status
*/
int32_t csi_spiflash_erase_chip(spiflash_handle_t handle)
{
    SPIFLASH_NULL_PARAM_CHK(handle);

    if (!spiflash_init_flag) {
        return ERR_SPIFLASH(DRV_ERROR);
    }

    return 0;
}

/**
  \brief       Get Flash information.
  \param[in]   handle  spiflash handle to operate.
  \return      Pointer to Flash information \ref spiflash_info_t
*/
spiflash_info_t *csi_spiflash_get_info(spiflash_handle_t handle)
{
    if (handle == NULL || (!spiflash_init_flag)) {
        return NULL;
    }

    ck_spiflash_priv_t *spiflash_priv = handle;
    spiflash_info_t *spiflash_info = &(spiflash_priv->spiflashinfo);
    return spiflash_info;
}

/**
  \brief       Get SPIFLASH status.
  \param[in]   handle  spiflash handle to operate.
  \return      SPIFLASH status \ref spiflash_status_t
*/
spiflash_status_t csi_spiflash_get_status(spiflash_handle_t handle)
{
    if (handle == NULL || (!spiflash_init_flag)) {
        spiflash_status_t ret;
        memset(&ret, 0, sizeof(spiflash_status_t));
        return ret;
    }

    ck_spiflash_priv_t *spiflash_priv = handle;

    return spiflash_priv->status;
}

