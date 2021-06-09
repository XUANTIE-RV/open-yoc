#include <stddef.h>
#include "lsgpio.h"
#include "reg_lsgpio.h"
#include "io_config.h"
#include "reg_syscfg.h"
#include "sleep.h"
#include "ls_dbg.h"
#include "platform.h"
#include "le501x.h"

gpio_pin_t spi2_clk;
gpio_pin_t spi2_nss;
gpio_pin_t spi2_mosi;
gpio_pin_t spi2_miso;
gpio_pin_t uart1_txd;
gpio_pin_t uart1_rxd;
gpio_pin_t uart1_ck;
gpio_pin_t uart2_txd;
gpio_pin_t uart2_rxd;
gpio_pin_t uart3_txd;
gpio_pin_t uart3_rxd;
gpio_pin_t adtim1_ch1;
gpio_pin_t adtim1_ch1n;
gpio_pin_t adtim1_ch2;
gpio_pin_t adtim1_ch2n;
gpio_pin_t adtim1_ch3;
gpio_pin_t adtim1_ch3n;
gpio_pin_t adtim1_ch4;
gpio_pin_t adtim1_etr;
gpio_pin_t adtim1_bk;
gpio_pin_t gptima1_ch1;
gpio_pin_t gptima1_ch2;
gpio_pin_t gptima1_ch3;
gpio_pin_t gptima1_ch4;
gpio_pin_t gptima1_etr;
gpio_pin_t gptimb1_ch1;
gpio_pin_t gptimb1_ch2;
gpio_pin_t gptimb1_ch3;
gpio_pin_t gptimb1_ch4;
gpio_pin_t gptimb1_etr;
gpio_pin_t gptimc1_ch1;
gpio_pin_t gptimc1_ch1n;
gpio_pin_t gptimc1_ch2;
gpio_pin_t gptimc1_bk;



reg_lsgpio_t* GPIO_GetPort(uint8_t Pin_port)
{
    uint8_t       PortID = Pin_port;
    reg_lsgpio_t* Port = NULL;

    switch (PortID)
    {
    case 0:
        Port = LSGPIOA;
        break;
    case 1:
        Port = LSGPIOB;
        break;
    case 2:
        Port = LSGPIOC;
        break;
    default:
        LS_ASSERT(0);
        break;
    }
    return Port;
}

static void uart_io_cfg(uint8_t txd,uint8_t rxd)
{
    io_set_pin(txd);
    io_cfg_output(txd);
    io_cfg_input(rxd);
}

void af_io_init(gpio_pin_t *pin,enum GPIO_AF af)
{
    reg_lsgpio_t *port = GPIO_GetPort(pin->port);
    switch(pin->num)
    {
    case 0: case 1: case 2: case 3:
        MODIFY_REG(port->AF0, GPIO_IO0_AF_MASK << pin->num * 8, af << pin->num * 8);
    break;
    case 4: case 5: case 6: case 7:
        MODIFY_REG(port->AF1, GPIO_IO4_AF_MASK << (pin->num - 4) * 8, af << (pin->num - 4) * 8);
    break;
    case 8: case 9: case 10: case 11:
        MODIFY_REG(port->AF2, GPIO_IO8_AF_MASK << (pin->num - 8) * 8, af << (pin->num - 8) * 8);
    break;
    case 12: case 13: case 14: case 15:
        MODIFY_REG(port->AF3, GPIO_IO12_AF_MASK << (pin->num - 12) * 8, af << (pin->num - 12) * 8);
    break;
    }
    MODIFY_REG(port->MODE, GPIO_MODE0_MASK << (pin->num << 1u), SET_GPIO_MODE_AF << (pin->num << 1u));
}

static void ana_io_init(gpio_pin_t *pin,enum GPIO_ANA_FUNC ana)
{
   reg_lsgpio_t *port = GPIO_GetPort(pin->port);
   MODIFY_REG(port->AE, GPIO_AE0_MASK << (pin->num << 1u), ana << (pin->num << 1u));
   MODIFY_REG(port->MODE, GPIO_MODE0_MASK << (pin->num << 1u), SET_GPIO_MODE_ANALOG << (pin->num << 1u));
  
}

void set_gpio_mode(gpio_pin_t *pin)
{
    reg_lsgpio_t *gpiox = GPIO_GetPort(pin->port);
    MODIFY_REG(gpiox->MODE, GPIO_MODE0_MASK << (pin->num << 1u), SET_GPIO_MODE_GPIO << (pin->num << 1u));
}

static void uart_7816_io_cfg(uint8_t txd,uint8_t ck)
{
    io_set_pin(txd);
    io_set_pin(ck);
    io_cfg_opendrain(txd);
    io_cfg_output(txd);
    io_cfg_output(ck);
}

void qspi_flash_io_init(void){}

void qspi_flash_io_deinit(void){}

static void spi_clk_io_cfg(uint8_t clk)
{
    io_set_pin(clk);
    io_cfg_output(clk);
}

static void spi_nss_io_cfg(uint8_t nss)
{
    io_set_pin(nss);
    io_cfg_output(nss);
}

static void spi_mosi_io_cfg(uint8_t mosi)
{
    io_set_pin(mosi);
    io_cfg_output(mosi);
}

static void spi_miso_io_cfg(uint8_t miso)
{
    io_cfg_input(miso);
}

void spi2_clk_io_init(uint8_t clk)
{
    *(uint8_t *)&spi2_clk = clk;
    spi_clk_io_cfg( clk);
    af_io_init((gpio_pin_t *)&clk,AF_SPI2_SCK);
}

void spi2_nss_io_init(uint8_t nss)
{
    *(uint8_t *)&spi2_nss = nss;
    spi_nss_io_cfg( nss);
    af_io_init((gpio_pin_t *)&nss,AF_SPI2_NSS);    
}

void spi2_mosi_io_init(uint8_t mosi)
{
    *(uint8_t *)&spi2_mosi = mosi;
    spi_mosi_io_cfg( mosi);
    af_io_init((gpio_pin_t *)&mosi,AF_SPI2_MOSI);
}

void spi2_miso_io_init(uint8_t miso)
{

    *(uint8_t *)&spi2_miso = miso;
    spi_miso_io_cfg( miso);
    af_io_init((gpio_pin_t *)&miso,AF_SPI2_MISO); 
}

void spi2_clk_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&spi2_clk);
}

void spi2_nss_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&spi2_nss);
}

void spi2_mosi_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&spi2_mosi);
}

void spi2_miso_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&spi2_miso);
}

void uart1_io_init(uint8_t txd,uint8_t rxd)
{
    *(uint8_t *)&uart1_txd = txd;
    *(uint8_t *)&uart1_rxd = rxd;
    uart_io_cfg(txd,rxd);
    af_io_init((gpio_pin_t *)&txd,AF_UART1_TXD);
    af_io_init((gpio_pin_t *)&rxd,AF_UART1_RXD);
}

void uart1_io_deinit()
{
    set_gpio_mode((gpio_pin_t *)&uart1_txd);
    set_gpio_mode((gpio_pin_t *)&uart1_rxd);
}

void uart2_io_init(uint8_t txd,uint8_t rxd)
{
    *(uint8_t *)&uart2_txd = txd;
    *(uint8_t *)&uart2_rxd = rxd;
    uart_io_cfg(txd,rxd);
    af_io_init((gpio_pin_t *)&txd,AF_UART2_TXD);
    af_io_init((gpio_pin_t *)&rxd,AF_UART2_RXD);
}

void uart2_io_deinit()
{
    set_gpio_mode((gpio_pin_t *)&uart2_txd);
    set_gpio_mode((gpio_pin_t *)&uart2_rxd);
}

void uart3_io_init(uint8_t txd,uint8_t rxd)
{
    *(uint8_t *)&uart3_txd = txd;
    *(uint8_t *)&uart3_rxd = rxd;
    uart_io_cfg(txd,rxd);
    af_io_init((gpio_pin_t *)&txd,AF_UART3_TXD);
    af_io_init((gpio_pin_t *)&rxd,AF_UART3_RXD);
}

void uart3_io_deinit()
{
    set_gpio_mode((gpio_pin_t *)&uart3_txd);
    set_gpio_mode((gpio_pin_t *)&uart3_rxd);
}

void uart1_7816_io_deinit()
{
    set_gpio_mode((gpio_pin_t *)&uart1_txd);
    set_gpio_mode((gpio_pin_t *)&uart1_ck);
}

void io_cfg_opendrain(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->OT |= 1<< x->num;
}

void io_cfg_pushpull(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->OT |= 0<< x->num;
}

void uart1_7816_io_init(uint8_t txd,uint8_t ck)
{
    *(uint8_t *)&uart1_txd = txd;
    *(uint8_t *)&uart1_ck = ck;
    uart_7816_io_cfg(txd,ck);
    af_io_init((gpio_pin_t *)&txd,AF_UART1_TXD);
    af_io_init((gpio_pin_t *)&ck,AF_UART1_CK);
}


static void timer_ch_io_output_cfg(uint8_t pin,uint8_t default_val)
{
    io_write_pin(pin, default_val);
    io_cfg_output(pin);
}

static void timer_ch_io_cfg(uint8_t pin,bool output,uint8_t default_val)
{
    if(output)
    {
        timer_ch_io_output_cfg(pin,default_val);
    }else
    {
        io_cfg_input(pin);
    }
}

void adtim1_ch1_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&adtim1_ch1 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH1);
}

void adtim1_ch1_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch1);
}

void adtim1_ch2_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&adtim1_ch2 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH2);
}

void adtim1_ch2_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch2);
}

void adtim1_ch3_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&adtim1_ch3 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH3);
}

void adtim1_ch3_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch3);
}

void adtim1_ch4_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&adtim1_ch4 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH4);
}

void adtim1_ch4_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch4);
}

void adtim1_ch1n_io_init(uint8_t pin)
{
    *(uint8_t *)&adtim1_ch1n = pin;
    timer_ch_io_output_cfg(pin,!io_get_output_val(*(uint8_t *)&adtim1_ch1));
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH1N);
}

void adtim1_ch1n_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch1n);
}

void adtim1_ch2n_io_init(uint8_t pin)
{
    *(uint8_t *)&adtim1_ch2n = pin;
    timer_ch_io_output_cfg(pin,!io_get_output_val(*(uint8_t *)&adtim1_ch2));
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH2N);
}

void adtim1_ch2n_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch2n);
}

void adtim1_ch3n_io_init(uint8_t pin)
{
    *(uint8_t *)&adtim1_ch3n = pin;
    timer_ch_io_output_cfg(pin,!io_get_output_val(*(uint8_t *)&adtim1_ch3));
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_CH3N);
}

void adtim1_ch3n_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_ch3n);
}

void adtim1_etr_io_init(uint8_t pin)
{
    *(uint8_t *)&adtim1_etr = pin;
    io_cfg_input(pin);
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_ETR);
}

void adtim1_etr_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_etr);
}

void adtim1_bk_io_init(uint8_t pin)
{
    *(uint8_t *)&adtim1_bk = pin;
    io_cfg_input(pin);
    af_io_init((gpio_pin_t *)&pin,AF_ADTIM1_BK);
}

void adtim1_bk_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&adtim1_bk);
}

void gptima1_ch1_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptima1_ch1 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMA1_CH1);
}

void gptima1_ch1_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptima1_ch1);
}

void gptima1_ch2_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptima1_ch2 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMA1_CH2);
}

void gptima1_ch2_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptima1_ch2);
}

void gptima1_ch3_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptima1_ch3 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMA1_CH3);
}

void gptima1_ch3_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptima1_ch3);
}

void gptima1_ch4_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptima1_ch4 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMA1_CH4);
}

void gptima1_ch4_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptima1_ch4);
}

void gptima1_etr_io_init(uint8_t pin)
{
    *(uint8_t *)&gptima1_etr = pin;
    io_cfg_input(pin);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMA1_ETR);
}

void gptima1_etr_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptima1_etr);
}

void gptimb1_ch1_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptimb1_ch1 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMB1_CH1);
}

void gptimb1_ch1_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimb1_ch1);
}

void gptimb1_ch2_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptimb1_ch2 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMB1_CH2);
}

void gptimb1_ch2_io_deinit(void)

{
    set_gpio_mode((gpio_pin_t *)&gptimb1_ch2);
}

void gptimb1_ch3_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptimb1_ch3 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMB1_CH3);
}

void gptimb1_ch3_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimb1_ch3);
}

void gptimb1_ch4_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptimb1_ch4 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMB1_CH4);
}

void gptimb1_ch4_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimb1_ch4);
}

void gptimb1_etr_io_init(uint8_t pin)
{
    *(uint8_t *)&gptimb1_etr = pin;
    io_cfg_input(pin);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMB1_ETR);
}

void gptimb1_etr_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimb1_etr);
}

void gptimc1_ch1_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptimc1_ch1 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMC1_CH1);
}

void gptimc1_ch1_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimc1_ch1);
}

void gptimc1_ch1n_io_init(uint8_t pin)
{
    *(uint8_t *)&gptimc1_ch1n = pin;
    timer_ch_io_output_cfg(pin,!io_get_output_val(*(uint8_t *)&gptimc1_ch1));
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMC1_CH1N);
}

void gptimc1_ch1n_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimc1_ch1n);
}

void gptimc1_ch2_io_init(uint8_t pin,bool output,uint8_t default_val)
{
    *(uint8_t *)&gptimc1_ch2 = pin;
    timer_ch_io_cfg(pin,output,default_val);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMC1_CH2);
}

void gptimc1_ch2_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimc1_ch2);
}

void gptimc1_bk_io_init(uint8_t pin)
{
    *(uint8_t *)&gptimc1_bk = pin;
    io_cfg_input(pin);
    af_io_init((gpio_pin_t *)&pin,AF_GPTIMC1_BK);
}

void gptimc1_bk_io_deinit(void)
{
    set_gpio_mode((gpio_pin_t *)&gptimc1_bk);
}

void ana_func1_io_init(uint8_t ain)
{
    ana_io_init((gpio_pin_t *)&ain,ANA_FUNC1);
}

void ain_io_deinit(uint8_t ain)
{
    set_gpio_mode((gpio_pin_t *)&ain);
    io_cfg_disable(ain);
}

void adc12b_in0_io_init(void)
{
    ana_func1_io_init(PB12);
}

void adc12b_in0_io_deinit(void)
{    
    ain_io_deinit(PB12);
}

void adc12b_in1_io_init(void)
{
    ana_func1_io_init(PB13);
}

void adc12b_in1_io_deinit(void)
{
    ain_io_deinit(PB13);
}

void adc12b_in2_io_init(void)
{
    ana_func1_io_init(PC00);
}

void adc12b_in2_io_deinit(void)
{
    ain_io_deinit(PC00);
}

void adc12b_in3_io_init(void)
{
    ana_func1_io_init(PC01);
}

void adc12b_in3_io_deinit(void)
{
    ain_io_deinit(PC01);
}

void adc12b_in4_io_init(void)
{
    ana_func1_io_init(PA00);
}

void adc12b_in4_io_deinit(void)
{
    ain_io_deinit(PA00);
}

void adc12b_in5_io_init(void)
{
    ana_func1_io_init(PA01);
}

void adc12b_in5_io_deinit(void)
{
    ain_io_deinit(PA01);
}


void adc12b_in6_io_init(void)
{
    ana_func1_io_init(PA02);
}

void adc12b_in6_io_deinit(void)
{
    ain_io_deinit(PA02);
}

void adc12b_in7_io_init(void)
{
    ana_func1_io_init(PA03);
}

void adc12b_in7_io_deinit(void)
{
    ain_io_deinit(PA03);
}

void adc12b_in8_io_init(void)
{
    ana_func1_io_init(PA04);
}

void adc12b_in8_io_deinit(void)
{
    ain_io_deinit(PA04);
}

void EXTI_Handler(void);
void io_init(void)
{
    RCC->AHBEN |= RCC_GPIOA_MASK | RCC_GPIOB_MASK | RCC_GPIOC_MASK;
    LSGPIOA->MODE = 0;
    LSGPIOA->IE = 0;
    LSGPIOA->OE = 0;
    LSGPIOA->PUPD = 0;
    LSGPIOB->MODE = 0x2800;
    LSGPIOB->IE = 0;
    LSGPIOB->OE = 0;
    LSGPIOB->PUPD = 0x2800;
    arm_cm_set_int_isr(EXTI_IRQn,EXTI_Handler);
    __NVIC_EnableIRQ(EXTI_IRQn);
}

void io_cfg_output(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->OE |= 1<< x->num;
}

void io_cfg_input(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->OE &= ~(1<< x->num);
    gpiox->IE |= 1<< x->num;
}

void io_cfg_disable(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->IE &= ~(1<< x->num);
}

void io_set_pin(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->DOUT |= 1<< x->num;
}

void io_clr_pin(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->DOUT &= ~(1<< x->num);
}

void io_write_pin(uint8_t pin, uint8_t val)
{
    if(val)
    {
        io_set_pin(pin);
    }else
    {
        io_clr_pin(pin);
    }
}

uint8_t io_get_output_val(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    uint8_t val = (gpiox->DOUT >> x->num) & 0x1;
    return val;
}

uint8_t io_read_pin(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    uint8_t val = (gpiox->DIN >> x->num) & 0x1;
    return val;

}

void io_toggle_pin(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    gpiox->DOUT ^= 1<< x->num;
}

void io_pull_write(uint8_t pin,io_pull_type_t pull)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    MODIFY_REG(gpiox->PUPD, 0x3 << 2*x->num, pull << 2*x->num);
}

io_pull_type_t io_pull_read(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    reg_lsgpio_t *gpiox = GPIO_GetPort(x->port);
    io_pull_type_t pull = (io_pull_type_t)((gpiox->PUPD >> 2 * x->num ) & 0x3);
    return pull;
}

void io_exti_config(uint8_t pin,exti_edge_t edge)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    if(x->num <= 7)
    {
        MODIFY_REG(EXTI->EICFG0, 0xf<< 4 * x->num,x->port << 4 * x->num);
    }else
    {
        MODIFY_REG(EXTI->EICFG1, 0xf<< 4 * (x->num - 8),x->port << 4 * (x->num - 8));
    }
    if(edge == INT_EDGE_FALLING)
    {
        EXTI->ERTS &= ~(1 << x->num);
        EXTI->EFTS |= 1 << x->num;
        switch(pin)
        {
        case PA00:
            CLEAR_BIT(SYSCFG->PMU_WKUP,PA00_IO_WKUP<<WKUP_EDGE_POS);
        break;
        case PA07:
            CLEAR_BIT(SYSCFG->PMU_WKUP,PA07_IO_WKUP<<WKUP_EDGE_POS);
        break;
        case PB11:
            CLEAR_BIT(SYSCFG->PMU_WKUP,PB11_IO_WKUP<<WKUP_EDGE_POS);
        break;
        case PB15:
            CLEAR_BIT(SYSCFG->PMU_WKUP,PB15_IO_WKUP<<WKUP_EDGE_POS);
        break;
        }
    }else
    {
        EXTI->EFTS &= ~(1 << x->num);
        EXTI->ERTS |= 1 << x->num;
        switch(pin)
        {
        case PA00:
            SET_BIT(SYSCFG->PMU_WKUP,PA00_IO_WKUP<<WKUP_EDGE_POS);
        break;
        case PA07:
            SET_BIT(SYSCFG->PMU_WKUP,PA07_IO_WKUP<<WKUP_EDGE_POS);
        break;
        case PB11:
            SET_BIT(SYSCFG->PMU_WKUP,PB11_IO_WKUP<<WKUP_EDGE_POS);
        break;
        case PB15:
            SET_BIT(SYSCFG->PMU_WKUP,PB15_IO_WKUP<<WKUP_EDGE_POS);
        break;
        }
    }
}

void io_ext_intrp_disable(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    EXTI->EIDR =  1 << x->num;
    switch(pin)
    {
    case PA00:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PA00_IO_WKUP<<WKUP_EN_POS);
    break;
    case PA07:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PA07_IO_WKUP<<WKUP_EN_POS);
    break;
    case PB11:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PB11_IO_WKUP<<WKUP_EN_POS);
    break;
    case PB15:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PB15_IO_WKUP<<WKUP_EN_POS);
    break;
    }
}

void io_ext_intrp_enable(uint8_t pin)
{
    gpio_pin_t *x = (gpio_pin_t *)&pin;
    switch(pin)
    {
    case PA00:
        SET_BIT(SYSCFG->PMU_WKUP,PA00_IO_WKUP<<WKUP_EN_POS);
    break;
    case PA07:
        SET_BIT(SYSCFG->PMU_WKUP,PA07_IO_WKUP<<WKUP_EN_POS);
    break;
    case PB11:
        SET_BIT(SYSCFG->PMU_WKUP,PB11_IO_WKUP<<WKUP_EN_POS);
    break;
    case PB15:
        SET_BIT(SYSCFG->PMU_WKUP,PB15_IO_WKUP<<WKUP_EN_POS);
    break;
    }
    EXTI->EIER =  1 << x->num;
}

void io_exti_enable(uint8_t pin,bool enable)
{
    if(enable==false)
    {
        io_ext_intrp_disable(pin);
    }else
    {
        io_ext_intrp_enable(pin);
    }
}

void io_wkup_en_clr_set(uint8_t pin)
{
    switch(pin)
    {
    case PA00:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PA00_IO_WKUP<<WKUP_EN_POS);
        SET_BIT(SYSCFG->PMU_WKUP,PA00_IO_WKUP<<WKUP_EN_POS);
    break;
    case PA07:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PA07_IO_WKUP<<WKUP_EN_POS);
        SET_BIT(SYSCFG->PMU_WKUP,PA07_IO_WKUP<<WKUP_EN_POS);
    break;
    case PB11:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PB11_IO_WKUP<<WKUP_EN_POS);
        SET_BIT(SYSCFG->PMU_WKUP,PB11_IO_WKUP<<WKUP_EN_POS);
    break;
    case PB15:
        CLEAR_BIT(SYSCFG->PMU_WKUP,PB15_IO_WKUP<<WKUP_EN_POS);
        SET_BIT(SYSCFG->PMU_WKUP,PB15_IO_WKUP<<WKUP_EN_POS);
    break;
    }
}

__attribute__((weak)) void io_exti_callback(uint8_t pin){}

static void exti_io_handler(uint8_t port,uint8_t num)
{
    uint8_t pin = port<<4 | num;
    EXTI->EICR = 1<<num;
    io_wkup_en_clr_set(pin);
    io_exti_callback(pin);
}

void EXTI_Handler(void)
{
    uint16_t int_stat = EXTI->EEIFM;
    uint8_t i;
    uint8_t port;
    for(i=0;i<8;++i)
    {
        if(1<<i & int_stat)
        {
            port = EXTI->EICFG0 >> (4*i);
            exti_io_handler(port,i);
        }
    }
    for(i=8;i<16;++i)
    {
        if(1<<i & int_stat)
        {
            port = EXTI->EICFG1 >> (4*(i-8));
            exti_io_handler(port,i);
        }
    }
}

