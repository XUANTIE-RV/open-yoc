#ifndef COMPONENTS_CHIP_BL606P_CHIP_BL606_H_
#define COMPONENTS_CHIP_BL606P_CHIP_BL606_H_

#ifdef __blog_h_
#include <blog.h>
#endif

#ifndef   log_error
#define   log_error           printf
#endif
#ifndef   log_info
#define   log_info            printf
#endif
#ifndef  blog_error
#define  blog_error           printf
#endif
#ifndef  blog_warn
#define  blog_warn            printf
#endif
#ifndef  blog_info
#define  blog_info(fmt, ...) do{}while(0)
#endif
#ifndef blog_print
#define blog_print            printf
#endif

#ifndef pvPortMalloc
#define pvPortMalloc          malloc
#endif
#ifndef vPortFree
#define vPortFree             free
#endif
#ifndef vTaskDelay
#define vTaskDelay           msleep
#endif

#include <bl606p.h>
#include <bl606p_glb.h>
#include <bl606p_common.h>

#include <bl606p_dma.h>


#include <bl606p_gpio.h>
#include <bl606p_uart.h>
#include <bl606p_adc.h>




#endif /* COMPONENTS_CHIP_BL606_CHIP_BL606_H_ */
