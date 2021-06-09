#ifndef SYS_STAT_H_
#define SYS_STAT_H_
#include <stdbool.h>
void uart1_status_set(bool);

void uart2_status_set(bool);

void uart3_status_set(bool);

void spi1_status_set(bool);

void spi2_status_set(bool);

void i2c1_status_set(bool);

void i2c2_status_set(bool);

void rtc_status_set(bool);

void adtimer1_status_set(bool);

void bstimer1_status_set(bool);

void gptimera1_status_set(bool);

void gptimerb1_status_set(bool);

void gptimerc1_status_set(bool);

void pdm_status_set(bool);

void dma_status_set(bool);

void crypt_status_set(bool);

void adc12b_status_set(bool);

void app_status_set(bool);

bool peri_status_busy(void);

bool app_event_status_busy(void);

#endif
