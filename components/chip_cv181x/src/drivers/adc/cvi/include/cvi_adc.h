
#include <soc.h>
#include "hal_adc.h"
// #include "cvi_pin.h"


typedef struct {
   uint8_t    readable;
   uint8_t    writeable;
   uint8_t    error;
} cvi_state_t;

typedef enum {
    CVI_ADC_EVENT_CONVERT_COMPLETE = 0,      ///< All data convert completed
    CVI_ADC_EVENT_CONVERT_HALF_DONE,         ///< Convert half done
    CVI_ADC_EVENT_ERROR                      ///< All errors including but not limited to what converted data has not been read before the new conversion result is load to the data register
} cvi_adc_event_t;

typedef struct _cvi_adc_t {
    cvi_dev_t                dev;        ///< Hw-device info
    void (*callback)(struct _cvi_adc_t *adc, cvi_adc_event_t event, void *arg);  ///< User callback ,signaled by driver event
    void                    *arg;        ///< User private param ,passed to user callback
    uint32_t                *data;       ///< Data buf
    uint32_t                 num;        ///< Data size by word
    // cvi_dma_ch_t            *dma;        ///< Dma channel handle
    cvi_error_t             (*start)(struct _cvi_adc_t *adc);  ///< Start function
    cvi_error_t             (*stop)(struct _cvi_adc_t *adc);   ///< Stop function
    cvi_state_t             state;       ///< ADC current state
    void                    *priv;
} cvi_adc_t;

cvi_error_t cvi_adc_init(cvi_adc_t *adc);
void cvi_adc_uninit(cvi_adc_t *adc);
cvi_error_t cvi_adc_set_buffer(cvi_adc_t *adc, uint32_t *data, uint32_t num);
cvi_error_t cvi_adc_start(cvi_adc_t *adc);
cvi_error_t cvi_adc_start_async(cvi_adc_t *adc);
cvi_error_t cvi_adc_stop(cvi_adc_t *adc);
cvi_error_t cvi_adc_stop_async(cvi_adc_t *adc);
cvi_error_t cvi_adc_channel_enable(cvi_adc_t *adc, uint8_t ch_id, bool is_enable);
cvi_error_t cvi_adc_sampling_time(cvi_adc_t *adc, uint16_t clock_num);
uint32_t cvi_adc_freq_div(cvi_adc_t *adc, uint32_t div);
int32_t cvi_adc_read(cvi_adc_t *adc);
cvi_error_t cvi_adc_get_state(cvi_adc_t *adc, cvi_state_t *state);
uint32_t cvi_adc_get_freq(cvi_adc_t *adc);
cvi_error_t cvi_adc_configure_irq(cvi_adc_t *adc, void *callback, void *arg);
