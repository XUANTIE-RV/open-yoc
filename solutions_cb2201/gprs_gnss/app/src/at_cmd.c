#include <yoc/atserver.h>
#include <at_cmd.h>

#if defined(CONFIG_AT) && CONFIG_AT
/**< test Called when recv cmd is  AT+<x>=? TEST_CMD*/
/**< query Called when recv cmd is AT+<x>? READ_CMD*/
/**< setup Called when recv cmd is AT+<x>=<...> WRITE_CMD*/
/**< exec Called when recv cmd is  AT+<x> EXECUTE_CMD*/

extern void at_cmd_app_send(char *cmd, int type, char *data);
extern void at_cmd_app_get(char *cmd, int type, char *data);
extern void at_cmd_app_set(char *cmd, int type, char *data);
extern void at_cmd_app_sleep(char *cmd, int type, char *data);
const atserver_cmd_t at_cmd[] = {
    AT,
    AT_HELP,
    AT_CGMR,
    AT_FWVER,
    AT_REBOOT,

#if 1
    AT_KVGET,
    AT_KVSET,
    AT_KVDEL,
    AT_KVGETINT,
    AT_KVSETINT,
    AT_KVDELINT,
#endif

    // AT_CIPSTART,
    // AT_CIPSTOP,
    // AT_CIPRECVCFG,
    // AT_CIPID,
    // AT_CIPSTATUS,
    // AT_CIPSEND,
    // AT_CIPRECV,
    {"AT+SEND", at_cmd_app_send},
    {"AT+GET", at_cmd_app_get},
    {"AT+SET", at_cmd_app_set},
    {"AT+SLEEP", at_cmd_app_sleep},


    AT_NULL,
};
#endif
