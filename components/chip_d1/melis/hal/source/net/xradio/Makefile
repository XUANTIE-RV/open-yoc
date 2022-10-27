# The minimum set of files needed for lwIP.
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/adapter
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/adapter/net_ctrl
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/adapter/net_ctrl/sys_ctrl
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/adapter/cmd
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/include
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/os
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/os/include
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/os/include/net
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/drv/source/net/xradio/os/include/sys

subdir-ccflags-y +=-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/
subdir-ccflags-y +=-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/include/lwip/
subdir-ccflags-y +=-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/
subdir-ccflags-y +=-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/core/ipv4/
subdir-ccflags-y +=-I$(srctree)/ekernel/subsys/net/rt-thread/lwip/src/arch/include/
subdir-ccflags-y +=-I$(srctree)/ekernel/drivers/test/network/tcpip

CFLAGS_COMMON += -DCONFIG_BACKEND_XRADIO
CFLAGS_COMMON += -DCONFIG_DRIVER_XRADIO
CFLAGS_COMMON += -DCONFIG_CRYPTO_INTERNAL
CFLAGS_COMMON += -DCONFIG_NO_RANDOM_POOL
CFLAGS_WPAS += -DCONFIG_NO_CONFIG_BLOBS
CFLAGS_WPAS += -DCONFIG_NO_ROAMING
CFLAGS_HOSTAPD += -DCONFIG_NO_RADIUS
CFLAGS_HOSTAPD += -DCONFIG_NO_ACCOUNTING
CFLAGS_HOSTAPD += -DCONFIG_NO_VLAN
CFLAGS_HOSTAPD += -DCONFIG_NO_CTRL_IFACE

include $(srctree)/ekernel/drivers/drv/source/net/xradio/config.mk

XR_CFLAGS +=-DCONFIG_OS_NUTTX -DCONFIG_OS_NUTTX_CM_PASS
XR_CFLAGS +=$(CONFIG_SYMBOLS) $(CFLAGS_COMMON) $(CFLAGS_WPAS) $(CFLAGS_HOSTAPD)

ifeq ($(CONFIG_XR819),y)
XR_CFLAGS += -DCONFIG_XR819
else
XR_CFLAGS += -DCONFIG_XR829
endif

subdir-ccflags-y += $(XR_CFLAGS)

obj-$(CONFIG_XRADIO) += ethernetif.o \
	os/sys/mbuf/mbuf_0.o \
	os/sys/atomic.o \
	os/kernel/os/os_mutex.o \
	os/kernel/os/os_queue.o \
	os/kernel/os/os_semaphore.o \
	os/kernel/os/os_thread.o \
	os/kernel/os/os_timer.o \
	os/kernel/os/os_error.o \
	driver/port/atomic.o \
	driver/port/gpio.o \
	driver/port/sdio.o \
	driver/port/xr_drv_mem.o \
	driver/port/ffs.o \
	driver/port/hexdump.o \
	driver/port/workqueue.o

obj-$(CONFIG_XRADIO_CTRL) += adapter/net_ctrl/net_ctrl.o \
	adapter/net_ctrl/net_init.o \
	adapter/net_ctrl/net_sys.o \
	adapter/net_ctrl/sysinfo.o \
	adapter/net_ctrl/sys_ctrl/container.o \
	adapter/net_ctrl/sys_ctrl/event_queue.o \
	adapter/net_ctrl/sys_ctrl/looper.o \
	adapter/net_ctrl/sys_ctrl/observer.o \
	adapter/net_ctrl/sys_ctrl/publisher.o \
	adapter/net_ctrl/sys_ctrl/sys_ctrl.o \
	adapter/wifi_adapter.o \
	adapter/wifimanager.o \
	adapter/wifi_main.o \
	adapter/cmd/cmd_ifconfig.o \
	adapter/cmd/cmd_util.o \
	adapter/cmd/cmd_wlan.o \
	adapter/cmd/cmd_lmac.o \
	adapter/cmd/cmd_wlancmd.o \
	adapter/cmd/frm_construct.o \
	adapter/cmd/command.o
