#include "systick.h"
#include "arm_math.h"
#include "compile_flag.h"
typedef struct
{
  volatile uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  volatile uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
  volatile uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
  volatile uint32_t CALIB;                  /*!< Offset: 0x00C (R/ )  SysTick Calibration Register */
} SysTick_Type;

#define SysTick_CTRL_COUNTFLAG_Pos         16U                                            /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk         (1UL << SysTick_CTRL_COUNTFLAG_Pos)            /*!< SysTick CTRL: COUNTFLAG Mask */

#define SysTick_CTRL_CLKSOURCE_Pos          2U                                            /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos            1U                                            /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos             0U                                            /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)           /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0U                                            /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/)    /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos             0U                                            /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk            (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/)    /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos            31U                                            /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk            (1UL << SysTick_CALIB_NOREF_Pos)               /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos             30U                                            /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk             (1UL << SysTick_CALIB_SKEW_Pos)                /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos             0U                                            /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk            (0xFFFFFFUL /*<< SysTick_CALIB_TENMS_Pos*/)    /*!< SysTick CALIB: TENMS Mask */

#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address */
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct */

XIP_BANNED void systick_start()
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0xffffff;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

XIP_BANNED uint32_t systick_get_value()
{
    return SysTick->VAL;
}

XIP_BANNED int32_t systick_time_diff(uint32_t a,uint32_t b)
{
    uint32_t diff = (b-a) &0xffffff;
    if(diff&0x800000)
    {
        return diff|0xff000000;
    }else
    {
        return diff;
    }
}

enum wrapping_status
{
    COUNTING_TOP_HALF,
    COUNTING_BOTTOM_HALF,
};

XIP_BANNED static bool wrapping_check(uint32_t start_tick,enum wrapping_status *stat)
{
    uint32_t current = systick_get_value();
    bool wrap = false;
    if(systick_time_diff(current,start_tick)>=0)
    {
        if(*stat == COUNTING_BOTTOM_HALF)
        {
            *stat = COUNTING_TOP_HALF;
            wrap = true;
        }
    }else
    {
        if(*stat == COUNTING_TOP_HALF)
        {
            *stat = COUNTING_BOTTOM_HALF;
        }
    }
    return wrap;
}

XIP_BANNED bool systick_poll_timeout(uint32_t start_tick,uint32_t timeout,bool (*poll)(va_list),...)
{
    uint32_t end = 0xffffff - start_tick + timeout;
    uint8_t i = end>>24;
    uint32_t end_tick = 0xffffff - (end & 0xffffff);
    va_list ap;
    enum wrapping_status wrap_stat = COUNTING_TOP_HALF;
    while(i)
    {
        if(poll)
        {
            va_start(ap,poll);
            if(poll(ap))
            {
                return false;
            }
        }
        if(wrapping_check(start_tick,&wrap_stat))
        {
            i -= 1;
        }
    }
    while(systick_get_value()>end_tick)
    {
        if(poll)
        {
            va_start(ap,poll);
            if(poll(ap))
            {
                return false;
            }
        }
    }
    return true;
}
