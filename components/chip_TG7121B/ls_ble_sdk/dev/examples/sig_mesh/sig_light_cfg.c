#include "sig_light_cfg.h"
#include "io_config.h"
#include "sig_mesh_ctl.h"
#include "lsuart.h"
#include "log.h"
#include "builtin_timer.h"

TIM_HandleTypeDef light_tim_hdl;
TIM_OC_InitTypeDef light_tim_cfg;
static struct light_state ls_mesh_light_state[3]={0};
extern void app_client_model_tx_message_handler(uint32_t tx_msg, uint16_t model_cfg_idx );

#define APP_STATE_OFF                (0)
#define APP_STATE_ON                 (1)
 
static void ls_uart_init(void);
 UART_HandleTypeDef UART_SIG_MESH_Config;

#define BUTTON_TIMEOUT 50 // timer units: ms
static struct builtin_timer *button_timer_inst = NULL;
static void ls_button_timer_cb(void *param);

bool button1_valid =false;
bool button2_valid =false;

struct button_info
{
   uint8_t  release_count;
   uint32_t count;
   uint32_t check_count;
};

struct button_info button1,button2;

void ls_button_timer_init(void)
{
    button1_valid =false;
    button2_valid =false;

    button1.release_count=0;
    button1.count=0;
    button1.check_count=0;

    button2.release_count=0;
    button2.count=0;
    button2.check_count=0;

    button_timer_inst = builtin_timer_create(ls_button_timer_cb);
    builtin_timer_start(button_timer_inst, BUTTON_TIMEOUT, NULL); 
}


static void light_control(bool lightness_flag, uint16_t const led_id)
{
    uint32_t channel_id;
    uint8_t *onoff_state;
    uint16_t *led_level;

    if (led_id != LIGHT_LED_1)
    {
	    channel_id = (led_id == LIGHT_LED_2)? TIM_CHANNEL_2:TIM_CHANNEL_3;
	    onoff_state = (led_id == ls_mesh_light_state[0].led_idx)? &(ls_mesh_light_state[0].onoff_state):&(ls_mesh_light_state[1].onoff_state);
	    led_level = (led_id == ls_mesh_light_state[0].led_idx)? &(ls_mesh_light_state[0].level):&(ls_mesh_light_state[1].level);
    }
    else
    {
        channel_id = TIM_CHANNEL_1;
        onoff_state = &(ls_mesh_light_state[2].onoff_state);
        led_level = &(ls_mesh_light_state[2].level);
    }
        
    if((!lightness_flag) && (*onoff_state == 0))
    {
        light_tim_cfg.Pulse = 0;

        HAL_TIM_PWM_ConfigChannel(&light_tim_hdl, &light_tim_cfg, channel_id); 
        HAL_TIM_PWM_Start(&light_tim_hdl, channel_id);
    }
    else
    {
        light_tim_cfg.Pulse = (*led_level);
        
        HAL_TIM_PWM_ConfigChannel(&light_tim_hdl, &light_tim_cfg, channel_id); 
        HAL_TIM_PWM_Start(&light_tim_hdl, channel_id);
    }
}

void ls_mesh_light_set_lightness(uint16_t level, uint16_t const led_id)
{
    uint8_t *onoff_state;
    uint16_t *led_level;

    if (led_id != LIGHT_LED_1)
    {
	  onoff_state = (led_id == ls_mesh_light_state[0].led_idx)? &(ls_mesh_light_state[0].onoff_state):&(ls_mesh_light_state[1].onoff_state);
	  led_level = (led_id == ls_mesh_light_state[0].led_idx)? &(ls_mesh_light_state[0].level):&(ls_mesh_light_state[1].level);
    }
    else
    {
       onoff_state = &(ls_mesh_light_state[2].onoff_state);
	   led_level = &(ls_mesh_light_state[2].level);
    }
    
      if(*led_level != level)
      {
        *onoff_state = 1;
        *led_level = level;
        light_control(true,led_id);
      }
}

void ls_mesh_light_set_onoff(uint8_t on_off, uint16_t const led_id)
{
    uint8_t *onoff_state;
    if (led_id != LIGHT_LED_1)
    {
	    onoff_state = (led_id == ls_mesh_light_state[0].led_idx)? &(ls_mesh_light_state[0].onoff_state):&(ls_mesh_light_state[1].onoff_state);
    }
    else
    {
        onoff_state = &(ls_mesh_light_state[2].onoff_state);
    }
    
    if(*onoff_state != on_off)
    {
        *onoff_state = on_off;
        light_control(false,led_id);
    }
}

uint8_t ls_mesh_light_get_onoff(uint16_t const led_id)
{
    return (led_id == ls_mesh_light_state[0].led_idx)?(ls_mesh_light_state[0].onoff_state):((led_id == ls_mesh_light_state[1].led_idx)?(ls_mesh_light_state[1].onoff_state):(ls_mesh_light_state[2].onoff_state));
}

uint16_t ls_mesh_light_get_lightness(uint16_t const led_id)
{
	return (led_id == ls_mesh_light_state[0].led_idx)?(ls_mesh_light_state[0].level):((led_id == ls_mesh_light_state[1].led_idx)?(ls_mesh_light_state[1].level):(ls_mesh_light_state[2].level));
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* TIMx Peripheral clock enable */
    REG_FIELD_WR(RCC->APB1EN, RCC_GPTIMB1, 1);
    /* Enable all GPIO Channels Clock requested */
    /* Configure PA00, PA01 for PWM output*/
    gptimb1_ch1_io_init(LIGHT_LED_1,true,0);
	gptimb1_ch2_io_init(LIGHT_LED_2,true,0);
    gptimb1_ch3_io_init(LIGHT_LED_3,true,0);
}

void ls_mesh_pwm_init(void)
{
    uint16_t level_t = 0x5FF;
    light_tim_hdl.Instance = TIMx;
  
    light_tim_hdl.Init.Prescaler = 0;
    light_tim_hdl.Init.Period = 0xffff -1;
    light_tim_hdl.Init.ClockDivision = 0;
    light_tim_hdl.Init.CounterMode = TIM_COUNTERMODE_UP;
    light_tim_hdl.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&light_tim_hdl);

    light_tim_cfg.OCMode = TIM_OCMODE_PWM1;
    light_tim_cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
    light_tim_cfg.OCFastMode = TIM_OCFAST_DISABLE;

    ls_mesh_light_state[0].led_idx = LIGHT_LED_2;
    ls_mesh_light_state[0].level = level_t;
    ls_mesh_light_state[0].onoff_state = APP_STATE_OFF;


	ls_mesh_light_state[1].led_idx = LIGHT_LED_3;
    ls_mesh_light_state[1].level = level_t;
    ls_mesh_light_state[1].onoff_state = APP_STATE_OFF;

    ls_mesh_light_state[2].led_idx = LIGHT_LED_1;
    ls_mesh_light_state[2].level = level_t;
    ls_mesh_light_state[2].onoff_state = APP_STATE_OFF;

    ls_mesh_light_set_lightness(0xffff,LIGHT_LED_1);
    ls_mesh_light_set_lightness(level_t,LIGHT_LED_2);
    ls_mesh_light_set_lightness(level_t,LIGHT_LED_3);

}

void light_button_init(void)
{
    io_cfg_input(LIGHT_BUTTON_1);
    io_pull_write(LIGHT_BUTTON_1,IO_PULL_DOWN);
    io_exti_config(LIGHT_BUTTON_1,INT_EDGE_RISING);
    io_exti_enable(LIGHT_BUTTON_1,true);

    io_cfg_input(LIGHT_BUTTON_2);
    io_pull_write(LIGHT_BUTTON_2,IO_PULL_DOWN);
    io_exti_config(LIGHT_BUTTON_2,INT_EDGE_RISING);
    io_exti_enable(LIGHT_BUTTON_2,true);

    ls_uart_init();
}

void io_exti_callback(uint8_t pin) // override io_exti_callback
{
    static uint32_t on_off=0; 
    static uint32_t light_lvl=0; 

    switch (pin)
    {
    case LIGHT_BUTTON_1:
    {
        button1.count++;
       if (button1_valid == false)
       {
         button1.check_count = button1.count;
         on_off++;
         button1_valid =true;
       }
       app_client_model_tx_message_handler(((on_off & 0x00000001)?APP_STATE_ON:APP_STATE_OFF),MESH_CMDL_CFG_IDX_GENC_ONOFF);
       ls_mesh_light_set_onoff(((on_off & 0x00000001)?APP_STATE_ON:APP_STATE_OFF),0xff);
    }   
    break;
    case LIGHT_BUTTON_2:
    { 
        button2.count++;
       if (button2_valid == false)
       {
          light_lvl=0x12345678;
          button2_valid =true;
          button2.check_count = button2.count;
          app_client_model_tx_message_handler(light_lvl,MESH_CMDL_CFG_IDX_GENC_LEVEL);
       }       
    }    
    break;
    default:
        break;
    } 
}

static void ls_uart_init(void)
{
    uart1_io_init(PB00, PB01);
    UART_SIG_MESH_Config.UARTX = UART1;
    UART_SIG_MESH_Config.Init.BaudRate = UART_BAUDRATE_115200;
    UART_SIG_MESH_Config.Init.MSBEN = 0;
    UART_SIG_MESH_Config.Init.Parity = UART_NOPARITY;
    UART_SIG_MESH_Config.Init.StopBits = UART_STOPBITS1;
    UART_SIG_MESH_Config.Init.WordLength = UART_BYTESIZE8;
    HAL_UART_Init(&UART_SIG_MESH_Config);
}

static void ls_button_timer_cb(void *param)
{

   if (button1_valid == true)
   {
      if (button1.check_count != button1.count)
      {
         button1.release_count=0;
         button1.check_count = button1.count; 
      }
      else
      {
           if ((++button1.release_count) >= 10)
           {
              button1_valid = false;
              button1.check_count =0;
              button1.count=0;
              button1.release_count=0;
           }
      }
   }

   if (button2_valid == true)
   {
       if (button2.check_count != button2.count)
       {
           button2.release_count=0;
           button2.check_count = button2.count;
       }
       else
       {
          if ((++button2.release_count) >= 10)
           {
               button2_valid = false;
               button2.check_count =0;
               button2.count=0;
               button2.release_count=0;
           }
       }
   }

    builtin_timer_start(button_timer_inst, BUTTON_TIMEOUT, NULL);
}
