#include "app_config.h"
#include "system/includes.h"
#include "asm/power_interface.h"
#include "wtyi_config.h"
#include "wtyi_log.h"
#include "wtyi_power_manager.h"

static volatile u32 wtyi_power_busy_mask;
static volatile u8 wtyi_power_inited;

static u8 wtyi_power_idle_query(void)
{
#if WTYI_LOW_POWER_ENABLE
    return wtyi_power_busy_mask == 0;
#else
    return 0;
#endif
}

REGISTER_LP_TARGET(wtyi_lp_target) = {
    .name = "wtyi",
    .is_idle = wtyi_power_idle_query,
};

void wtyi_power_manager_init(void)
{
    wtyi_power_inited = 1;
#if WTYI_LOW_POWER_ENABLE
    WTYI_LOG_BOOT("low_power=enabled mask=0x%lx\r\n", wtyi_power_busy_mask);
#else
    WTYI_LOG_BOOT("low_power=disabled by WTYI_LOW_POWER_ENABLE\r\n");
#endif
}

void wtyi_power_set_busy(enum wtyi_power_busy_id id, u8 busy)
{
    u32 bit;

    if (id >= WTYI_POWER_BUSY_MAX) {
        return;
    }

    bit = BIT(id);
    if (busy) {
        wtyi_power_busy_mask |= bit;
    } else {
        wtyi_power_busy_mask &= ~bit;
    }
}

u8 wtyi_power_is_busy(enum wtyi_power_busy_id id)
{
    if (id >= WTYI_POWER_BUSY_MAX) {
        return 0;
    }

    return (wtyi_power_busy_mask & BIT(id)) ? 1 : 0;
}

u32 wtyi_power_get_busy_mask(void)
{
    return wtyi_power_busy_mask;
}

u8 wtyi_power_can_sleep(void)
{
    return wtyi_power_idle_query();
}

void wtyi_power_bt_set_busy(u8 busy)
{
    wtyi_power_set_busy(WTYI_POWER_BUSY_BT, busy);
}

void wtyi_power_spi_set_busy(u8 busy)
{
    wtyi_power_set_busy(WTYI_POWER_BUSY_SPI, busy);
}

void wtyi_power_iic_set_busy(u8 busy)
{
    wtyi_power_set_busy(WTYI_POWER_BUSY_IIC, busy);
}

void wtyi_power_adc_set_busy(u8 busy)
{
    wtyi_power_set_busy(WTYI_POWER_BUSY_ADC, busy);
}

void wtyi_power_ota_begin(void)
{
    wtyi_power_set_busy(WTYI_POWER_BUSY_OTA, 1);
    if (wtyi_power_inited) {
        WTYI_LOG_BOOT("ota_busy=1 mask=0x%lx\r\n", wtyi_power_busy_mask);
    }
}

void wtyi_power_ota_end(void)
{
    wtyi_power_set_busy(WTYI_POWER_BUSY_OTA, 0);
    if (wtyi_power_inited) {
        WTYI_LOG_BOOT("ota_busy=0 mask=0x%lx\r\n", wtyi_power_busy_mask);
    }
}
