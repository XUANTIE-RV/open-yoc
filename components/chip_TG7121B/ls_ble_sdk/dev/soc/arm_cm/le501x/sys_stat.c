#include <string.h>
#include "sys_stat.h"

struct peri_stat_env
{
    bool rtc;
    bool uart1;
    bool uart2;
    bool uart3;
    bool spi1;
    bool spi2;
    bool i2c1;
    bool i2c2;
    bool adtimer1;
    bool bstimer1;
    bool gptimera1;
    bool gptimerb1;
    bool gptimerc1;
    bool pdm;
    bool dma;
    bool crypt;
    bool adc12b;
}peri_stat;

bool app_event_status;

void app_status_set(bool stat)
{
    app_event_status = stat;
}

void uart1_status_set(bool stat)
{
    peri_stat.uart1 = stat;
}

void uart2_status_set(bool stat)
{
    peri_stat.uart2 = stat;
}

void uart3_status_set(bool stat)
{
    peri_stat.uart3 = stat;
}

void spi1_status_set(bool stat)
{
    peri_stat.spi1 = stat;
}

void spi2_status_set(bool stat)
{
    peri_stat.spi2 = stat;
}

void i2c1_status_set(bool stat)
{
    peri_stat.i2c1 = stat;
}

void i2c2_status_set(bool stat)
{
    peri_stat.i2c2 = stat;
}

void rtc_status_set(bool stat)
{
    peri_stat.rtc = stat;
}

void adtimer1_status_set(bool stat)
{
    peri_stat.adtimer1 = stat;
}

void bstimer1_status_set(bool stat)
{
    peri_stat.bstimer1 = stat;
}

void gptimera1_status_set(bool stat)
{
    peri_stat.gptimera1 = stat;
}

void gptimerb1_status_set(bool stat)
{
    peri_stat.gptimerb1 = stat;
}

void gptimerc1_status_set(bool stat)
{
    peri_stat.gptimerc1 = stat;
}

void pdm_status_set(bool stat)
{
    peri_stat.pdm = stat;
}

void dma_status_set(bool stat)
{
    peri_stat.dma = stat;
}

void crypt_status_set(bool stat)
{
     peri_stat.crypt = stat;
}

void adc12b_status_set(bool stat)
{
    peri_stat.adc12b = stat;
}


bool peri_status_busy()
{
    const struct peri_stat_env idle_stat = {0};
    if(memcmp(&peri_stat,&idle_stat,sizeof(struct peri_stat_env))==0)
    {
        return false;
    }else
    {
        return true;
    }
}

bool app_event_status_busy()
{
    return app_event_status;
}
