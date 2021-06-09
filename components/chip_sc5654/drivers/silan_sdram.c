#include "ap1508.h"
#include "silan_types.h"
#include "silan_sdram.h"
#include "silan_sdram_regs.h"
#include "silan_printf.h"
#include "silan_pmu.h"
#include "silan_syscfg.h"

volatile int *ptr;

static void sdram_init(uint32_t sdram_para,uint32_t sdram_type)
{
    volatile int i;
	int temp;

	// Soft Reset SDRAM Controller
	__REG32(SILAN_SYSCFG_REG8) &= ~(0x3<<5);  
   	__REG32(SILAN_SYSCFG_REG8) |=  (0x3<<5);

	ptr = (int *)(0x22033000);

    // SDRAM Clock Delay Para
	temp = ((sdram_para&0x7fff)|0x2000);
 	__REG32(SILAN_SYSCFG_REG16) = temp;

	// wait for sdram clock to stable 
    i=100;
    while(i--);
    // issue nop to sdram
    SDRAM_CFG_REG1 = 3;

    // Why here init REG0?
	if(sdram_type == SDRAM512K)
    	SDRAM_CFG_REG0 = 0x02fc0000;

    i=100;
    while(i--);
    // precharge all
    SDRAM_CFG_REG1 = 1;
    // sdram refresh timer
    SDRAM_CFG_REG2 = 10;
    i=100;
    while(i--);
    // set refresh timer to operation time
    SDRAM_CFG_REG2 = SDRAM_REFCNT(get_sysclk_val_settled()/1000000);
    // set command write mode
    SDRAM_CFG_REG1 = 2;
    // read data from sdram
    i = ptr[0]; 
    // programe configuration register0
	if(sdram_type == SDRAM512K)
		SDRAM_CFG_REG0 = 0x02fc0000;
	else if(sdram_type == SDRAM1M)
		SDRAM_CFG_REG0 = 0x00fc0000;
	else if(sdram_type == SDRAM4M)
		SDRAM_CFG_REG0 = 0x00fc0008;
	else
		SDRAM_CFG_REG0 = 0x00fc000e;		

    // RAS Add One Clock
	//SDRAM_CFG_REG0 |= 1<<26 ;

    // clear configuration register1
    SDRAM_CFG_REG1 = 0;

}
static int sdram_memery_write_op(uint32_t start_addr, uint32_t data)
{
	volatile uint32_t *mem_ptr ;
	mem_ptr = (uint32_t *)start_addr;
	*mem_ptr = data;
	return 0;
}
static int sdram_memery_read_op(uint32_t start_addr)
{
	uint32_t tmp;
	volatile uint32_t *mem_ptr ;
	mem_ptr = (uint32_t *)start_addr;
	tmp = *mem_ptr;
	return tmp;
}
static int sdram_cap_16M_check()
{
	uint32_t val1,val2;
	sdram_memery_write_op(0x02000200,0xaa55a55a);
	sdram_memery_write_op(0x02000000,0x55aa5aa5);
	val1 = sdram_memery_read_op(0x02000200);
	val2 = sdram_memery_read_op(0x02000000);
	if((0xaa55a55a == val1)&&(0x55aa5aa5 == val2))
		return SDRAM16M;
	else
		return -1;
}

static int sdram_cap_4M_check()
{
	uint32_t val1,val2;
	sdram_memery_write_op(0x02000000+0x400000,0xaa55a55a);
	sdram_memery_write_op(0x02000000,0x55aa5aa5);
	val1 = sdram_memery_read_op(0x02000000+0x400000);
	val2 = sdram_memery_read_op(0x02000000);
	if((0xaa55a55a == val1)&&(0x55aa5aa5 == val2))
		return SDRAM4M;
	else
		return SDRAM1M;
}

static int sdram_type_check()
{
	sdram_init(SDRAM_PARA,SDRAM16M);
	if(sdram_cap_16M_check()>=0)
		return SDRAM16M;
	sdram_init(SDRAM_PARA,SDRAM4M);
	return (sdram_cap_4M_check()) ;
}

int sdram_flag = 0;
void silan_sdram_init()
{
	uint8_t get_sdram_type = 0xff;
	get_sdram_type = sdram_type_check();
	sdram_flag = get_sdram_type;
	SL_DBGLOG("get_sdram_type = %x",get_sdram_type);
	if(silan_get_bus_cclk() == 120000000)
		sdram_init(0x7060, get_sdram_type);
	else
		sdram_init(SDRAM_PARA, get_sdram_type);
	switch(get_sdram_type)
	{
			case SDRAM16M:                                                                                                                                                     
				SL_LOG("SDRAM Init Ok(16*16Mbit Cap)");	
				break;
			case SDRAM4M:
				SL_LOG("SDRAM Init Ok(4*16Mbit Cap)");
				break;
			case SDRAM1M:
				SL_LOG("SDRAM Init Ok(1*16Mbit Cap)");
				break;
				
			default:
				SL_LOG("SDRAM TYPE UNKNOW");
				break;
	}

}
