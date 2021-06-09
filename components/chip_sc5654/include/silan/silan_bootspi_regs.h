#ifndef __BOOTSPI_REG_H__
#define __BOOTSPI_REG_H__
#include "silan_types.h"
typedef volatile struct spi_struct
{
        unsigned int  main_ctrl;           // offset 0x000
        unsigned int  flash_power_down;     // 0x004
        unsigned int  flash_power_up;     // 0x008
        unsigned int  reserv1[5];
        unsigned int  cache_config;        // offset 0x020
        unsigned int  cache_sub_config;        // offset 0x024
        unsigned int  cache_crc_addr;            // offset 0x028
        unsigned int  cryp_code;            // offset 0x02c
        unsigned int  flash_addr_mask;            // offset 0x30
        unsigned int  reserv2[3];
        unsigned int  spi_config;          // offset 0x040
        unsigned int  spi_sub_config;          // offset 0x044
        unsigned int  spi_cmd;             // offset 0x048
        unsigned int  spi_addr;             // offset 0x04c
        unsigned int  spi_ctrl;            // offset 0x050
        unsigned int  spi_subctrl;         // offset 0x054
        unsigned int  spi_wdata;           // offset 0x058
        volatile unsigned int  spi_rdata;           // offset 0x05c
        unsigned int  reserv3[8]; //offset 0x060
        unsigned int  spicache_intr; //offset 0x080
        unsigned int  reserv4[3]; //offset 0x084
        unsigned int  softreset;           // offset 0x090
        unsigned int  key;                 // offset 0x094
        unsigned int  reserv0[27];        // offset 0x040 ~ 0x0fc reserved
        unsigned int  id_code;             // offset 0x100
        unsigned int  reserv[31];             // offset 0x104
        unsigned int  cache_state;         // offset 0x180

}spi_struct_t;

// main_ctrl
#define FLASH_PWD_RESUME_EN              (1 <<25)
#define SPICACHE_SYSRST_EN              (1 <<24)
#define HOST_NOT_CACHE                  (1 << 16)

//cache_sub_config
#define CACHE_CRC_LIMIT_ERR_NUM(x)            ((x&0x3f) << 26)
#define CACHE_CRC_ERR_OUT_EN            (1 << 25)
#define CACHE_CRC_ENABLE                (1 << 24)
#define CACHE_CRC_TRAP_FIRST_ADDR_EN                (1 << 23)
#define CACHE_CRM_DATA(x)                    ((x&0xff) << 8)
#define CACHE_CMD(x)                    ((x&0xff))

// cache_config
#define CACHE_SET_CFG                   ((unsigned int)1 << 31)
#define CAHCE_DTR_MODE                  ((unsigned int)1 << 28)
#define CACHE_DUMMY_CLK(x)                 ((x&0xff) << 16)
#define CACHE_CRM_NOCMD                     ((unsigned int)1 << 13)
#define CACHE_CRM_EXIST                     ((unsigned int)1 << 12)


#define CACHE_CMD_WIREMODE(x)                     ((unsigned int) (x&0x03))
#define CACHE_ADDR_WIREMODE(x)                    ((unsigned int) (x&0x03)<< 4)
#define CACHE_DATA_WIREMODE(x)                    ((unsigned int) (x&0x03)<< 24)
#define CACHE_ADDR_XBYTE(x)                     ((x&0xff) <<6)

// spi_config
//#define SPI_SET_CFG                   ((unsigned int)1 << 31)
#define SPI_DATA_WIREMODE(x)                     ((unsigned int) (x&0x03)<< 28)
#define HOST_CRM_DATA(x)                 ((unsigned int)(x&0xff) << 16)
#define HOST_CRM_EXIST                 ((unsigned int)(1) <<24)
#define HOST_CRM_NOCMD                 ((unsigned int)(1) <<25)
#define SPI_DUMMY_CLK(x)                 ((unsigned int)(x&0xff) << 8)
#define Host_Cmd  1
#define Host_Data 2
#define Host_Addr 3
#define SPI_CMD_WIREMODE(x)                     ((unsigned int) (x&0x03))
#define SPI_ADDR_WIREMODE(x)                    ((unsigned int) (x&0x03)<< 4)
#define SPI_ADDR_XBYTE(x)                     ((unsigned int) (x&0x03)<< 6)

// spi_sub_config
#define SPI_SET_CFG                   ((unsigned int)1 << 31)
#define NO_PROREADING                   ((unsigned int)1 << 24)
#define SPI_RCV_CPOL                   ((unsigned int)1 << 20)
#define SPI_CLK_DIV(x)                   ((x&0x7) << 10)
#define SPI_CS_SEL(x)                   ((x&0x3) << 16)
#define SPI_IDLE_NUM(x)                   (((unsigned int)x&0x3ff))



// spicmd
#define SPI_OP_CMD(x)                   ((unsigned int)(x & 0xff))
//SPI ADDR
#define SPI_OP_ADDR(x)                  ((unsigned int)(x & 0xffffffff) )

// spi_ctrl
#define FAST_READ_WRITE									((unsigned int)1 << 31)
#define HOST_RREQ                       (1 << 8)
#define HOST_WREQ                       (1 << 0)

// spi_subctrl
#define HOST_DATA_NUM(x)                ((unsigned int)(x & 0x3) << 24)
#define HOST_SPI_TRANS_IDLE             ((unsigned int)1 << 20)
#define HOST_SPI_DONE_STATUS            ((unsigned int)1 << 16)
#define HOST_RREADY                     (1 << 9)
#define HOST_RVALID                     (1 << 8)
#define HOST_WREADY                     (1 << 1)
#define HOST_WVALID                     (1 << 0)

// key
#define KEY0                            0x81058888
#define KEY1                            0xf2370609
#define KEY2                            0x35051f3c

#define Cache_Cmd  1
#define Cache_Data 2
#define Cache_Addr 3
#endif



