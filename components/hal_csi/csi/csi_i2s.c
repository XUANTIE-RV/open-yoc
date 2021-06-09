#include "drv/i2s.h"
typedef struct {
    i2s_config_type_t cfg;
    uint32_t rate;
    uint32_t tx_period;         ///< i2s send bytes tigger cb
    uint32_t rx_period;         ///< i2s receive bytes tigger cb
    uint8_t *tx_buf;                ///< i2s send buf
    uint32_t tx_buf_length;         ///< i2s send buf length
    uint8_t *rx_buf;                ///< i2s recv buf
    uint32_t rx_buf_length;         ///< i2s recv buf length
} csi_i2s_config_t;

int32_t csi_hal_i2s_config(i2s_handle_t handle, csi_i2s_config_t *config)
{
    return csi_i2s_config(handle, (i2s_config_t *)config);
}