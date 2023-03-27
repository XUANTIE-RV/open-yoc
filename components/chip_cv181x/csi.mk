##
 # Copyright (C) 2017-2019 Alibaba Group Holding Limited
##

INCDIR += -I$(CHIPDIR)/include
INCDIR += -I$(DRIVERDIR)/common/
INCDIR += -I$(DRIVERDIR)/uart/dw/v1.0
INCDIR += -I$(DRIVERDIR)/i2s/wj/v2.0
INCDIR += -I$(DRIVERDIR)/i2s/wj/multi_i2s
INCDIR += -I$(DRIVERDIR)/dma/dw/axi_dma
INCDIR += -I$(DRIVERDIR)/pinmux/wj/sparrow_v1.0
INCDIR += -I$(DRIVERDIR)/timer/dw/v1.0/
INCDIR += -I$(DRIVERDIR)/iic/dw/v1.0/
INCDIR += -I$(DRIVERDIR)/cpr/wj/sparrow
INCDIR += -I$(DRIVERDIR)/gpio/dw/v1.0
INCDIR += -I$(DRIVERDIR)/codec/ES/v2.0
INCDIR += -I$(DRIVERDIR)/wdt/dw/v1.0/
INCDIR += -I$(DRIVERDIR)/sdhc/dw/v1.0/
INCDIR += -I$(DRIVERDIR)/codec/ES/v2.0/
INCDIR += -I$(DRIVERDIR)/vad/wj/v1.0/
INCDIR += -I$(DRIVERDIR)/spdif/wj/v1.0
INCDIR += -I$(DRIVERDIR)/bmu/wj/v1.0
INCDIR += -I$(DRIVERDIR)/mbox/wj/v5.0

DRIVER_SSRC += $(CHIPDIR)/startup.S
DRIVER_CSRC += $(CHIPDIR)/system.c
DRIVER_CSRC += $(CHIPDIR)/cache.c
DRIVER_CSRC += $(CHIPDIR)/sys_clk.c
DRIVER_CSRC += $(CHIPDIR)/devices.c
DRIVER_CSRC += $(CHIPDIR)/novic_irq_tbl.c

DRIVER_CSRC += $(DRIVERDIR)/common/irq.c
DRIVER_CSRC += $(DRIVERDIR)/common/clk.c
DRIVER_CSRC += $(DRIVERDIR)/common/power_manage.c
DRIVER_CSRC += $(DRIVERDIR)/common/target_get.c
DRIVER_CSRC += $(DRIVERDIR)/common/reboot.c
DRIVER_CSRC += $(DRIVERDIR)/common/pre_main.c
DRIVER_CSRC += $(DRIVERDIR)/common/weak.c
DRIVER_CSRC += $(DRIVERDIR)/common/ringbuffer.c

DRIVER_CSRC += $(DRIVERDIR)/core/c906fdv/trap_c.c
DRIVER_SSRC += $(DRIVERDIR)/core/c906fdv/vectors.S
DRIVER_CSRC += $(DRIVERDIR)/irq/plic/v1.0/irq_port.c
DRIVER_CSRC += $(DRIVERDIR)/tick/csitimer/tick.c
DRIVER_CSRC += $(DRIVERDIR)/uart/dw/v1.0/uart.c
DRIVER_CSRC += $(DRIVERDIR)/uart/dw/v1.0/dw_uart_ll.c
DRIVER_CSRC += $(DRIVERDIR)/dma/dw/axi_dma/dma.c
DRIVER_CSRC += $(DRIVERDIR)/pinmux/wj/sparrow_v1.0/pinmux.c
DRIVER_CSRC += $(DRIVERDIR)/timer/dw/v1.0/timer.c
DRIVER_CSRC += $(DRIVERDIR)/vad/wj/v1.0/vad.c
DRIVER_CSRC += $(DRIVERDIR)/i2s/wj/v2.0/i2s.c
DRIVER_CSRC += $(DRIVERDIR)/i2s/wj/multi_i2s/multi_i2s.c
DRIVER_CSRC += $(DRIVERDIR)/iic/dw/v1.0/iic.c
DRIVER_CSRC += $(DRIVERDIR)/iic/dw/v1.0/dw_iic_ll.c
DRIVER_CSRC += $(DRIVERDIR)/gpio/dw/v1.0/gpio.c
DRIVER_CSRC += $(DRIVERDIR)/gpio/dw/v1.0/dw_gpio_ll.c
DRIVER_CSRC += $(DRIVERDIR)/gpio_pin/v1.0/gpio_pin.c
DRIVER_CSRC += $(DRIVERDIR)/wdt/dw/v1.0/wdt.c
DRIVER_CSRC += $(DRIVERDIR)/wdt/dw/v1.0/dw_wdt_ll.c
DRIVER_CSRC += $(DRIVERDIR)/sdhc/dw/v1.0/sdhc.c
DRIVER_CSRC += $(DRIVERDIR)/sdhc/dw/v1.0/sd_host.c
DRIVER_CSRC += $(DRIVERDIR)/codec/ES/v2.0/es7210.c
DRIVER_CSRC += $(DRIVERDIR)/codec/ES/v2.0/es8156.c
DRIVER_CSRC += $(DRIVERDIR)/codec/ES/v2.0/codec.c
DRIVER_CSRC += $(DRIVERDIR)/codec/ES/v2.0/es_port.c
DRIVER_CSRC += $(DRIVERDIR)/tdm/wj/v1.0/tdm.c
DRIVER_CSRC += $(DRIVERDIR)/spdif/wj/v1.0/spdif.c
DRIVER_CSRC += $(DRIVERDIR)/bmu/wj/v1.0/bmu.c
DRIVER_CSRC += $(DRIVERDIR)/mbox/wj/v5.0/mbox.c

