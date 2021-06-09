#ifndef LSQSPI_MSP_H_
#define LSQSPI_MSP_H_
#include "lsqspi.h"

void lsqspi_msp_init(struct lsqspi_instance *inst);

uint8_t lsqspi_rd_cap_dly_get(struct lsqspi_instance *inst);

uint8_t lsqspi_baudrate_get(struct lsqspi_instance *inst);

uint32_t lsqspi_dly_get(struct lsqspi_instance *inst);

#endif
