#ifndef __SPIFLASH_TEST__
#define __SPIFLASH_TEST__

#include <stdint.h>
#include <drv/spiflash.h>
#include <soc.h>
#include <string.h>
#include <stdlib.h>
#include <autotest.h>
#include <test_log.h>
#include <test_config.h>
#include <test_common.h>



extern int test_spiflash_interface(void *args);
extern int spiflash_get_info(void *args);
extern int spiflash_erase_read(void *args);
extern int spiflash_rease_write_erase_read(void *args);
extern int spiflash_erase_overflow(void *args);
extern int spiflash_erase_chip(void *args);
extern int spiflash_erase_write_read(void *args);
extern int spiflash_write_overflow(void *args);
extern int spiflash_read_overflow(void *args);
extern int spiflash_stability(void *args);
extern int spiflash_get_status(void *args);


#endif