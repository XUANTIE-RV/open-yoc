#ifndef RTC_H_
#define RTC_H_
#include <stdbool.h>
#include "reg_rtc.h"
#include "HAL_def.h"
#include "sdk_config.h"
#include "le501x.h"
#include "field_manipulate.h"

typedef struct __RTC_HandleTypeDef
{
    reg_rtc_t                *Instance;      /*!<  registers base address  >*/
    uint8_t                  rtc_cksel;
    __IO uint32_t            ErrorCode;      /*!<  Error code  >*/
} RTC_HandleTypeDef;

typedef struct 
{	
    uint32_t  date:5,
              mon:4,
              year:7;
}_calendar_cal;	

typedef struct 
{
    uint32_t  
              sec:6,
              min:6,
              hour:5,
              week:3;
}_calendar_time;

enum 
{
    CKSEL_NO_CLOCK,
    CKSEL_LSE,
    CKSEL_LSI,
    CKSEL_PLL,
    CKSEL_MAX,
};

void RTC_Init(void);
void RTC_Deinit(void);
void RTC_CalendarSet(_calendar_cal *calendar_cal,_calendar_time *calendar_time);
HAL_StatusTypeDef RTC_CalendarGet(_calendar_cal *calendar_cal,_calendar_time *calendar_time);

#endif 


