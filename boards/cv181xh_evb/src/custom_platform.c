//#include "platform.h"
#include <drv/pin.h>
#include <pinctrl-mars.h>
#include "cvi_type.h"
#define GPIO_SPKEN_GRP 0
#define GPIO_SPKEN_NUM 15

#define GPIO_PIN_MASK(_gpio_num) (1 << _gpio_num)


void _GPIOSetValue(u8 gpio_grp, u8 gpio_num, u8 level)
{
	csi_error_t ret;
	csi_gpio_t gpio = {0};

	ret = csi_gpio_init(&gpio, gpio_grp);
	if(ret != CSI_OK) {
		printf("csi_gpio_init failed\r\n");
		return;
	}
	// gpio write
	ret = csi_gpio_dir(&gpio , GPIO_PIN_MASK(gpio_num), GPIO_DIRECTION_OUTPUT);

	if(ret != CSI_OK) {
		printf("csi_gpio_dir failed\r\n");
		return;
	}
	csi_gpio_write(&gpio , GPIO_PIN_MASK(gpio_num), level);
	//printf("test pin end and success.\r\n");
}

static void _TouchPanlePinmux()
{
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
    PINMUX_CONFIG(VIVO_D2, XGPIOB_19);
    PINMUX_CONFIG(VIVO_D3, XGPIOB_18);
    PINMUX_CONFIG(VIVO_D10, IIC1_SDA);
    PINMUX_CONFIG(VIVO_D9, IIC1_SCL);
#else
    PINMUX_CONFIG(CAM_PD0, XGPIOA_1);
    PINMUX_CONFIG(CAM_PD1, XGPIOA_4);
    PINMUX_CONFIG(IIC3_SCL, IIC3_SCL);
    PINMUX_CONFIG(IIC3_SDA, IIC3_SDA);
#endif
}

static void _SensorPinmux()
{
}

static void _MipiRxPinmux(void)
{
}

static void _MipiTxPinmux(void)
{
//mipi tx pinmux
#if CONFIG_PANEL_ILI9488
	PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
	PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
	PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
	PINMUX_CONFIG(IIC0_SCL, XGPIOA_28);
#elif (CONFIG_PANEL_HX8394)
	PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
	PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
	PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
	PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
	PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
#if (CONFIG_BOARD_CV181XH == 1)
	PINMUX_CONFIG(PAD_MIPI_TXM3, XGPIOC_20);
	PINMUX_CONFIG(PAD_MIPI_TXP3, XGPIOC_21);
#elif (CONFIG_BOARD_CV181XC == 1)
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
	PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19);
#endif
	PINMUX_CONFIG(SPK_EN, XGPIOA_15);
#elif (CONFIG_PANEL_ST7701S)
	PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
	PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
	PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
	PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
	PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
	PINMUX_CONFIG(CAM_RST0, XGPIOA_2);
	PINMUX_CONFIG(PAD_MIPI_TXM4, PWM_12);
#else
	PINMUX_CONFIG(IIC2_SCL, PWR_GPIO_12);
	PINMUX_CONFIG(PWR_GPIO0, PWM_8);
#endif
#endif
}

static void _AudI2S1Pinmux(void)
{
    PINMUX_CONFIG(UART2_TX, IIS2_BCLK);
    PINMUX_CONFIG(UART2_RX, IIS2_DI);
    //speaker mute
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
    PINMUX_CONFIG(SPK_EN, XGPIOA_15);
#else
    PINMUX_CONFIG(SPK_EN, XGPIOA_15);
    PINMUX_CONFIG(AUX0, XGPIOA_30);
#endif
}

static void _BtPinmux()
{
#if (CONFIG_BOARD_BT_RTL8723DS == 1)
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
    PINMUX_CONFIG(SD1_D1, UART3_RX);
    PINMUX_CONFIG(SD1_D2, UART3_TX);
	PINMUX_CONFIG(VIVO_D0, XGPIOB_21);
#else
    PINMUX_CONFIG(ADC3, UART3_TX);
    PINMUX_CONFIG(ADC2, UART3_RX);
#endif
#endif
}

static void _WifiPinmux()
{
    //PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
    //PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19);
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
	PINMUX_CONFIG(AUX0, XGPIOA_30); //WIFI_CHIP_EN
#endif
}

void PLATFORM_IoInit(void)
{
//pinmux 切换接口
    _WifiPinmux();
    _MipiRxPinmux();
    _MipiTxPinmux();
    _SensorPinmux();
    _TouchPanlePinmux();
    _AudI2S1Pinmux();
    _BtPinmux();
}

void PLATFORM_PowerOff(void)
{
//下电休眠前调用接口
}

int PLATFORM_PanelInit(void)
{
    return CVI_SUCCESS;
}

void PLATFORM_PanelBacklightCtl(int level)
{

}

void PLATFORM_SpkMute(int value)
{
//0静音 ，1非静音
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
    if(value){
        _GPIOSetValue(GPIO_SPKEN_GRP, GPIO_SPKEN_NUM, 0);
    }else{
        _GPIOSetValue(GPIO_SPKEN_GRP, GPIO_SPKEN_NUM, 1);
    }
#else
    if(value){
        _GPIOSetValue(GPIO_SPKEN_GRP, GPIO_SPKEN_NUM, 1);
        _GPIOSetValue(GPIO_SPKEN_GRP, 30, 1);
    }else{
        _GPIOSetValue(GPIO_SPKEN_GRP, GPIO_SPKEN_NUM, 0);
        _GPIOSetValue(GPIO_SPKEN_GRP, 30, 0);
    }
#endif
}

int PLATFORM_IrCutCtl(int duty)
{
    return 0;
}
