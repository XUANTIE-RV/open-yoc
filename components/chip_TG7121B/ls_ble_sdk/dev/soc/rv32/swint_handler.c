#include <stdint.h>
#include <stdbool.h>
#include "swint_call_asm.h"
#include "compile_flag.h"
#include "spi_flash_int.h"
#include "platform.h"

void SWINT_Handler_C(uint32_t *args)
{
    uint32_t (*func)(uint32_t,uint32_t,uint32_t,uint32_t) = (void *)args[5];
    args[9] = func(args[9],args[8],args[7],args[6]);
}

XIP_BANNED void do_spi_flash_program_svcall(uint32_t offset,uint8_t *data,uint16_t length,bool quad)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_program,flash_swint_set);
}

XIP_BANNED void do_spi_flash_erase_svcall(uint32_t offset,uint8_t opcode)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_erase,flash_swint_set);
}

XIP_BANNED void do_spi_flash_read_svcall(void *param)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_read,flash_swint_set);
}

XIP_BANNED void do_spi_flash_chip_erase_svcall(void)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_chip_erase,flash_swint_set);

}

XIP_BANNED void do_spi_flash_erase_security_area_svcall(uint8_t idx)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_erase_security_area,flash_swint_set);

}

XIP_BANNED void do_spi_flash_program_security_area_svcall(uint8_t idx, uint16_t addr, uint8_t * data, uint16_t length)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_program_security_area,flash_swint_set);

}

XIP_BANNED void do_spi_flash_read_security_area_svcall(uint8_t idx, uint16_t addr, uint8_t * data, uint16_t length)
{
    SWINT_FUNC_CALL_INLINE_ASM(do_spi_flash_read_security_area,flash_swint_set);
}
