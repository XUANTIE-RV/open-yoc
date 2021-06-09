##
 # Copyright (C) 2017-2019 Alibaba Group Holding Limited
##


INCDIR += -I$(CHIPDIR)/include
INCDIR += -I$(DRIVERDIR)/include

ifneq ($(CONFIG_KERNEL_NONE), y)
ifneq ($(CONFIG_SUPPORT_TSPEND), y)
DRIVER_CSRC += $(CHIPDIR)/novic_irq_tbl.c
endif
endif

ifeq ($(CONFIG_CPU_CK804EF), y)
DRIVER_SSRC += $(CHIPDIR)/804ef/vectors.S
DRIVER_CSRC += $(CHIPDIR)/804ef/trap_c.c
DRIVER_CSRC += $(CHIPDIR)/804ef/devices.c
DRIVER_CSRC += $(CHIPDIR)/804ef/isr.c
DRIVER_CSRC += $(OFFCHIPDRIVERDIR)/flash/spiflash_w25q64fv.c
ifeq ($(CONFIG_SYSTEM_SECURE), y)
DRIVER_CSRC += $(DRIVERDIR)/ck_sasc_v4.c
DRIVER_CSRC += $(DRIVERDIR)/ck_efusec.c
INCDIR += -I$(OFFCHIPDRIVERDIR)/tipc
DRIVER_CSRC += $(OFFCHIPDRIVERDIR)/tipc/ck_tipc.c
DRIVER_CSRC += $(DRIVERDIR)/ck_aes.c
DRIVER_CSRC += $(DRIVERDIR)/ck_rsa.c
DRIVER_CSRC += $(DRIVERDIR)/ck_sha_v2.c
DRIVER_CSRC += $(DRIVERDIR)/osr_trng.c
endif
DRIVER_CSRC += $(DRIVERDIR)/ck_rtc.c
DRIVER_CSRC += $(DRIVERDIR)/dw_wdt.c
DRIVER_CSRC += $(DRIVERDIR)/dw_mmc.c
endif

DRIVER_CSRC += $(DRIVERDIR)/ck_pwm.c
ifeq ($(CONFIG_CPU_CK805F), y)
DRIVER_SSRC += $(CHIPDIR)/805f/vectors.S
DRIVER_CSRC += $(CHIPDIR)/805f/trap_c.c
DRIVER_CSRC += $(CHIPDIR)/805f/devices.c
DRIVER_CSRC += $(CHIPDIR)/805f/isr.c
DRIVER_CSRC += $(OFFCHIPDRIVERDIR)/flash/spiflash_w25q64fv.c
endif

DRIVER_SSRC += $(CHIPDIR)/startup.S
DRIVER_CSRC += $(CHIPDIR)/system.c
DRIVER_CSRC += $(CHIPDIR)/sys_freq.c
DRIVER_CSRC += $(CHIPDIR)/lib.c
DRIVER_CSRC += $(CHIPDIR)/pinmux.c
DRIVER_CSRC += $(CHIPDIR)/usi_pin_planning.c
DRIVER_CSRC += $(CHIPDIR)/ck_irq.c
DRIVER_CSRC += $(CHIPDIR)/reboot.c
DRIVER_CSRC += $(CHIPDIR)/yun_pmu.c
DRIVER_CSRC += $(DRIVERDIR)/dw_timer.c
DRIVER_CSRC += $(DRIVERDIR)/dw_gpio.c
DRIVER_CSRC += $(DRIVERDIR)/dw_dmac.c
DRIVER_CSRC += $(DRIVERDIR)/ck_i2s_v2.c
DRIVER_CSRC += $(DRIVERDIR)/ck_i2s_v2_dma.c
DRIVER_CSRC += $(DRIVERDIR)/ck_i2s_v2_int.c
DRIVER_CSRC += $(DRIVERDIR)/ck_i2s_v2_polling.c
DRIVER_CSRC += $(DRIVERDIR)/ck_usi.c
DRIVER_CSRC += $(DRIVERDIR)/ck_usi_iic.c
DRIVER_CSRC += $(DRIVERDIR)/dw_usart.c
DRIVER_CSRC += $(DRIVERDIR)/ck_usi_spi.c
DRIVER_CSRC += $(DRIVERDIR)/ck_usi_wrap.c
DRIVER_CSRC += $(DRIVERDIR)/ck_mailbox_v2.c
DRIVER_CSRC += $(DRIVERDIR)/ck_codec.c
DRIVER_CSRC += $(DRIVERDIR)/ck_codec_dma.c
