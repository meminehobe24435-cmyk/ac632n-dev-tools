#ifndef WTYI_ADC_BATTERY_H
#define WTYI_ADC_BATTERY_H

#include "typedef.h"

#define WTYI_ADC_OK              0
#define WTYI_ADC_ERR_PARAM      -1
#define WTYI_ADC_ERR_CONFIG     -2

struct wtyi_adc_battery_sample {
    u32 raw_avg;
    u32 input_mv;
    u32 vbat_mv;
    u32 sample_count;
};

void wtyi_adc_battery_init(void);
int wtyi_adc_battery_read(struct wtyi_adc_battery_sample *out);
u32 wtyi_adc_battery_apply_meter_cal(u32 adc_input_mv, u32 meter_vbat_mv);

#endif
