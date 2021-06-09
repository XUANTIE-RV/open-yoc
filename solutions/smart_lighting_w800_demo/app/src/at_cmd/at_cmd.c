#include <yoc/atserver.h>
#include <at_cmd.h>
#include <app_config.h>

#if 1//defined(CONFIG_AT) && CONFIG_AT
/**< test Called when recv cmd is AT+<x>=? TEST_CMD*/
/**< query Called when recv cmd is AT+<x>? READ_CMD*/
/**< setup Called when recv cmd is AT+<x>=<...> WRITE_CMD*/
/**< exec Called when recv cmd is AT+<x> EXECUTE_CMD*/

const atserver_cmd_t at_cmd[] = {
    AT,
    AT_HELP,
    AT_CGMR,
    AT_FWVER,
    AT_SYSTIME,
    AT_SAVE,
    AT_FACTORYW,
    AT_FACTORYR,
    AT_REBOOT,
    AT_EVENT,
    AT_ECHO,
#ifdef CONFIG_YOC_LPM
    AT_SLEEP,
#endif
    AT_MODEL,
    AT_KVGET,
    AT_KVSET,
    AT_KVDEL,
    AT_KVGETINT,
    AT_KVSETINT,
    AT_KVDELINT,

    AT_CIPSTART,
    AT_CIPSTOP,
    AT_CIPRECVCFG,
    AT_CIPID,
    AT_CIPSTATUS,
    AT_CIPSEND,
#ifdef CONFIG_YOC_LPM
    AT_CIPSENDPSM,
#endif
    AT_CIPRECV,

    AT_NULL,
};
#endif
