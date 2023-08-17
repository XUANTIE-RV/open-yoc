#ifndef CX_AUTH_MANAGER_H_
#define CX_AUTH_MANAGER_H_

#include <stdint.h>

typedef struct {
  struct {
    uint32_t otp_offset; // Offset of otp storage (e.g. efuse) in bytes
    const char* nv_uri;  // Uri of non-volatile storage (e.g. flash)
  } storage;
  struct {
    uint32_t id;
    uint32_t baud_rate;
  } uart;
} cx_auth_config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief        Grant authorization at the factory.
  \param[in]    config The configure of authorization.
  \return       Return only if an error has occurred.
*/
int cx_auth_factory_grant(const cx_auth_config_t* config);

/**
  \brief        Verify authorization at runtime.
  \param[in]    config The configure of authorization.
  \return       0: Success, < 0: Error code.
*/
int cx_auth_verify(const cx_auth_config_t* config);

#ifdef __cplusplus
}
#endif

#endif  // CX_AUTH_MANAGER_H_
