#ifndef _MBEDTLS_CONFIG_H_
#define _MBEDTLS_CONFIG_H_

#if defined(CONFIG_CLOUDIO_ALICOAP) && defined(CONFIG_CLOUDIO_ALIMQTT)
    #include "configs/config-aliiot.h"
#elif defined(CONFIG_CLOUDIO_ALICOAP)
    #include "configs/config-alicoap.h"
#elif defined(CONFIG_CLOUDIO_ALIMQTT)
    #include "configs/config-alimqtt.h"
#elif defined(CONFIG_USING_TLS)
    #include "configs/config-tls.h"
#else
    #include "configs/config-base.h"
#endif

#endif /* _MBEDTLS_CONFIG_H_ */
