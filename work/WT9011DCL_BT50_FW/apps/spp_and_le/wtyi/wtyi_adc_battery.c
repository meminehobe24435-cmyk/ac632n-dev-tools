#include "app_config.h"
#include "system/includes.h"
#include "asm/adc_api.h"
#include "wtyi_adc_battery.h"
#include "wtyi_log.h"
#include "wtyi_power_manager.h"

static void wtyi_sort_u32(u32 *data, u32 count)
{
    u32 i;

    for (i = 1; i < count; i++) {
        u32 key = data[i];
        int j = (int)i - 1;
        while (j >= 0 && data[j] > key) {
            data[j + 1] = data[j];
            j--;
        }
        data[j + 1] = key;
    }
}

void wtyi_adc_battery_init(void)
{
#if WTYI_ADC_PWR_CH == AD_CH_PB1
#error "WTYI PWR_ADC must use PA1, not PB1 ADKEY"
#endif
    wtyi_power_adc_set_busy(1);
    adc_add_sample_ch(WTYI_ADC_PWR_CH);
    WTYI_LOG_ADC("init ch=PA1 samples=%d divider=%d/%d cal=%d/%d\r\n",
                 WTYI_ADC_SAMPLE_COUNT, WTYI_ADC_DIVIDER_NUM,
                 WTYI_ADC_DIVIDER_DEN, WTYI_ADC_CAL_NUM, WTYI_ADC_CAL_DEN);
    wtyi_power_adc_set_busy(0);
}

int wtyi_adc_battery_read(struct wtyi_adc_battery_sample *out)
{
    u32 raw[WTYI_ADC_SAMPLE_COUNT];
    u32 mv[WTYI_ADC_SAMPLE_COUNT];
    u32 raw_sum = 0;
    u32 mv_sum = 0;
    u32 valid_count;
    u32 i;

    if (!out) {
        return WTYI_ADC_ERR_PARAM;
    }
    if (WTYI_ADC_SAMPLE_COUNT <= (WTYI_ADC_TRIM_COUNT * 2) ||
        WTYI_ADC_DIVIDER_DEN == 0 || WTYI_ADC_CAL_DEN == 0) {
        return WTYI_ADC_ERR_CONFIG;
    }

    wtyi_power_adc_set_busy(1);
    for (i = 0; i < WTYI_ADC_SAMPLE_COUNT; i++) {
        raw[i] = adc_get_value(WTYI_ADC_PWR_CH);
        mv[i] = adc_get_voltage(WTYI_ADC_PWR_CH);
    }

    wtyi_sort_u32(raw, WTYI_ADC_SAMPLE_COUNT);
    wtyi_sort_u32(mv, WTYI_ADC_SAMPLE_COUNT);

    for (i = WTYI_ADC_TRIM_COUNT; i < WTYI_ADC_SAMPLE_COUNT - WTYI_ADC_TRIM_COUNT; i++) {
        raw_sum += raw[i];
        mv_sum += mv[i];
    }

    valid_count = WTYI_ADC_SAMPLE_COUNT - (WTYI_ADC_TRIM_COUNT * 2);
    out->sample_count = WTYI_ADC_SAMPLE_COUNT;
    out->raw_avg = raw_sum / valid_count;
    out->input_mv = mv_sum / valid_count;
    out->vbat_mv = (out->input_mv * WTYI_ADC_DIVIDER_NUM / WTYI_ADC_DIVIDER_DEN) *
                   WTYI_ADC_CAL_NUM / WTYI_ADC_CAL_DEN;
    wtyi_power_adc_set_busy(0);

    return WTYI_ADC_OK;
}

u32 wtyi_adc_battery_apply_meter_cal(u32 adc_input_mv, u32 meter_vbat_mv)
{
    u32 estimated;

    if (!adc_input_mv) {
        return 0;
    }
    estimated = adc_input_mv * WTYI_ADC_DIVIDER_NUM / WTYI_ADC_DIVIDER_DEN;
    if (!estimated) {
        return 0;
    }
    return meter_vbat_mv * 1000 / estimated;
}
