#ifndef WTYI_POWER_MANAGER_H
#define WTYI_POWER_MANAGER_H

#include "asm/cpu.h"

enum wtyi_power_busy_id {
    WTYI_POWER_BUSY_BT = 0,
    WTYI_POWER_BUSY_SPI,
    WTYI_POWER_BUSY_IIC,
    WTYI_POWER_BUSY_ADC,
    WTYI_POWER_BUSY_OTA,
    WTYI_POWER_BUSY_MAX,
};

void wtyi_power_manager_init(void);
void wtyi_power_set_busy(enum wtyi_power_busy_id id, u8 busy);
u8 wtyi_power_is_busy(enum wtyi_power_busy_id id);
u32 wtyi_power_get_busy_mask(void);
u8 wtyi_power_can_sleep(void);

void wtyi_power_bt_set_busy(u8 busy);
void wtyi_power_spi_set_busy(u8 busy);
void wtyi_power_iic_set_busy(u8 busy);
void wtyi_power_adc_set_busy(u8 busy);
void wtyi_power_ota_begin(void);
void wtyi_power_ota_end(void);

#endif
