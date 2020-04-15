#ifndef _MBEDTLS_CONFIG_H_
#define _MBEDTLS_CONFIG_H_

#include <yoc_config.h>
#if defined(CONFIG_USING_TLS)
#include "configs/config-tls.h"
#elif defined(CONFIG_CLOUDIO_ALICOAP) && defined(CONFIG_CLOUDIO_ALIMQTT)
    #include "configs/config-aliiot.h"
#else
    #if defined(CONFIG_CLOUDIO_ALICOAP)
        #include "configs/config-alicoap.h"
    #elif defined(CONFIG_CLOUDIO_ALIMQTT)
        #include "configs/config-alimqtt.h"
    #else
        // #include "config-base.h"
    #endif
#endif

#endif /* _MBEDTLS_CONFIG_H_ */
