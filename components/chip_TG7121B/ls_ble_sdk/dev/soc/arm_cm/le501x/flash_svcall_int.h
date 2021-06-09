#ifndef FLASH_SVCALL_INT_H_
#define FLASH_SVCALL_INT_H_
#include <stdbool.h>
#include "le501x.h"
#include "svcall.h"

#define GLOBAL_INT_MASK_STATUS() __get_PRIMASK()

#endif
