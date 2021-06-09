#include "spi_flash_int.h"
#include "flash_svcall_int.h"
#include "flash_svcall.h"
typedef uint32_t (*svcall_fn_t)(uint32_t,uint32_t,uint32_t,uint32_t);

const svcall_fn_t svcall_table[SVCALL_NUM_MAX] = {
    [SVCALL_FLASH_PROGRAM] = (svcall_fn_t)do_spi_flash_program,
    [SVCALL_FLASH_ERASE] = (svcall_fn_t)do_spi_flash_erase,
    [SVCALL_FLASH_READ] = (svcall_fn_t)do_spi_flash_read,
    [SVCALL_FLASH_CHIP_ERASE] = (svcall_fn_t)do_spi_flash_chip_erase,
    [SVCALL_FLASH_ERASE_SECURITY] = (svcall_fn_t)do_spi_flash_erase_security_area,
    [SVCALL_FLASH_PROGRAM_SECURITY] = (svcall_fn_t)do_spi_flash_program_security_area,
    [SVCALL_FLASH_READ_SECURITY] = (svcall_fn_t)do_spi_flash_read_security_area,
    [SVCALL_FLASH_READ_REG] = (svcall_fn_t)do_spi_flash_read_reg,
};

void SVC_Handler_C(uint32_t *svc_args)
{
    uint8_t svc_num = ((uint8_t *)svc_args[6])[-2];
    if(svc_num<SVCALL_NUM_MAX)
    {
        svc_args[0] = svcall_table[svc_num](svc_args[0],svc_args[1],svc_args[2],svc_args[3]);
    }
}

