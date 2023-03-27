#ifndef _CVI_VIP_LDC_H_
#define _CVI_VIP_LDC_H_

#include <ldc_uapi.h>

#define CVILDC_BEGIN_JOB _IOWR('D', 0x00, struct gdc_handle_data)
#define CVILDC_END_JOB _IOW('D', 0x01, struct gdc_handle_data)
#define CVILDC_ADD_ROT_TASK _IOW('D', 0x02, struct gdc_task_attr)
#define CVILDC_ADD_LDC_TASK _IOW('D', 0x03, struct gdc_task_attr)
#define CVILDC_ROT _IOW('D', 0x04, unsigned long long)
#define CVILDC_LDC _IOW('D', 0x05, unsigned long long)

#endif // _CVI_VIP_LDC_H_
