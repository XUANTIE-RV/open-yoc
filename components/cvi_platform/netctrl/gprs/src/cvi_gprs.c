#include <stdio.h>
//#include <aos/kernel.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>

#include "usbh_core.h"
#include "usbh_rndis.h"
#include "usbh_serial.h"

#include "drv_usbh_class.h"

uint8_t g_gprs_got_ip = 0;

void APP_GprsInit(void)
{
	USBH_REGISTER_RNDIS_CLASS();
	USBH_REGISTER_USB_SERIAL_CLASS();
	usbh_initialize();

	drv_ec200a_serial_register(0);

	drv_ec200a_rndis_register();
	
	netmgr_hdl_t app_netmgr_hdl;

	app_netmgr_hdl = netmgr_dev_gprs_init();

	if (app_netmgr_hdl) {
		g_gprs_got_ip = 1;
		utask_t *task = utask_new("netmgr", 10 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
		netmgr_service_init(task);
		netmgr_start(app_netmgr_hdl);
	}
}
