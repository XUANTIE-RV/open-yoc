INCDIR += -I$(CSI_TESTS_DIR)/include/driver
CSRC += $(CSI_TESTS_DIR)/driver/*.c

ifeq ($(CONFIG_UART), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/uart/*.c)
endif
ifeq ($(CONFIG_GPIO), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/gpio/*.c)
endif
ifeq ($(CONFIG_PWM), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/pwm/*.c)
endif
ifeq ($(CONFIG_DMA), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/dma/*.c)
endif
ifeq ($(CONFIG_RTC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/rtc/*.c)
endif
ifeq ($(CONFIG_WDT), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/wdt/*.c)
endif
ifeq ($(CONFIG_ADC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/adc/*.c)
endif
ifeq ($(CONFIG_IIC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/iic/*.c)
endif
ifeq ($(CONFIG_SPI), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/spi/*.c)
endif
ifeq ($(CONFIG_SPIFLASH), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/spiflash/*.c)
endif
ifeq ($(CONFIG_SPINAND), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/spinand/*.c)
endif
ifeq ($(CONFIG_I2S), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/i2s/*.c)
endif
ifeq ($(CONFIG_CODEC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/codec/*.c)
endif
ifeq ($(CONFIG_TIMER), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/timer/*.c)
endif
ifeq ($(CONFIG_MBOX), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/mbox/*.c)
endif
ifeq ($(CONFIG_EFUSE), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/efuse/*.c)
endif
ifeq ($(CONFIG_AES), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/aes/*.c)
endif
ifeq ($(CONFIG_SHA), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/sha/*.c)
endif
ifeq ($(CONFIG_RSA), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/rsa/*.c)
endif
ifeq ($(CONFIG_TRNG), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/trng/*.c)
endif
ifeq ($(CONFIG_PIN), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/pin/*.c)
endif
ifeq ($(CONFIG_PWRMNG), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/pm/*.c)
endif
ifeq ($(CONFIG_TICK), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/tick/*.c)
endif
ifeq ($(CONFIG_TIPC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/tipc/*.c)
SSRC += $(wildcard $(CSI_TESTS_DIR)/driver/tipc/*.S)
endif
ifeq ($(CONFIG_INTNEST), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/intnest/*.c)
endif
ifeq ($(CONFIG_BAUD_CALC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/baud_calc/*.c)
endif
ifeq ($(CONFIG_SASC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/sasc/*.c)
SSRC += $(wildcard $(CSI_TESTS_DIR)/driver/sasc/*.S)
endif
ifeq ($(CONFIG_MMC), y)
CSRC += $(wildcard $(CSI_TESTS_DIR)/driver/mmc/*.c)
endif