#ifndef SVCALL_H_
#define SVCALL_H_

#if defined(__CC_ARM)
#define SVCALL(number, return_type, signature) return_type __svc(number) signature
#else
#define SVCALL(number, return_type, signature)          \
  _Pragma("GCC diagnostic push")                        \
  _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")   \
  __attribute__((naked))                                \
  __attribute__((unused))                               \
  static return_type signature                          \
  {                                                     \
    __asm(                                              \
        "svc %0\n"                                      \
        "bx r14" : : "I" (number) : "r0"   \
    );                                                  \
  }                                                     \
  _Pragma("GCC diagnostic pop")
#endif

enum svcall_num_enum
{
    SVCALL_FLASH_PROGRAM,
    SVCALL_FLASH_ERASE,
    SVCALL_FLASH_READ,
    SVCALL_FLASH_CHIP_ERASE,
    SVCALL_FLASH_ERASE_SECURITY,
    SVCALL_FLASH_PROGRAM_SECURITY,
    SVCALL_FLASH_READ_SECURITY,
    SVCALL_FLASH_READ_REG,
    SVCALL_NUM_MAX,
};

SVCALL(SVCALL_FLASH_PROGRAM,void,do_spi_flash_program_svcall(uint32_t offset,uint8_t *data,uint16_t length,bool quad));
SVCALL(SVCALL_FLASH_ERASE,void, do_spi_flash_erase_svcall(uint32_t offset,uint8_t opcode));
SVCALL(SVCALL_FLASH_READ,void,do_spi_flash_read_svcall(void *param));
SVCALL(SVCALL_FLASH_CHIP_ERASE,void,do_spi_flash_chip_erase_svcall(void));
SVCALL(SVCALL_FLASH_ERASE_SECURITY,void,do_spi_flash_erase_security_area_svcall(uint8_t idx));
SVCALL(SVCALL_FLASH_PROGRAM_SECURITY,void,do_spi_flash_program_security_area_svcall(uint8_t idx, uint16_t addr, uint8_t * data, uint16_t length));
SVCALL(SVCALL_FLASH_READ_SECURITY,void,do_spi_flash_read_security_area_svcall(uint8_t idx, uint16_t addr, uint8_t * data, uint16_t length));
SVCALL(SVCALL_FLASH_READ_REG,void,do_spi_flash_read_reg_svcall(void *param));
#endif
