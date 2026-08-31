#include "app_config.h"
#include "system/includes.h"
#include "wtyi_bt_config.h"
#include "wtyi_log.h"

#define WTYI_BLE_INTERVAL_UNIT_MS_X100  125
#define WTYI_BLE_TIMEOUT_UNIT_MS        10

static const struct wtyi_ble_conn_param wtyi_ble_profiles[] = {
    [WTYI_BLE_PROFILE_COMPATIBLE] = {
        .interval_min = WTYI_BLE_COMPATIBLE_INTERVAL_MIN,
        .interval_max = WTYI_BLE_COMPATIBLE_INTERVAL_MAX,
        .latency = WTYI_BLE_COMPATIBLE_LATENCY,
        .timeout = WTYI_BLE_COMPATIBLE_TIMEOUT,
        .mtu = 247,
        .phy = 1,
        .data_len = 251,
    },
    [WTYI_BLE_PROFILE_BALANCED] = {
        .interval_min = WTYI_BLE_BALANCED_INTERVAL_MIN,
        .interval_max = WTYI_BLE_BALANCED_INTERVAL_MAX,
        .latency = WTYI_BLE_BALANCED_LATENCY,
        .timeout = WTYI_BLE_BALANCED_TIMEOUT,
        .mtu = 247,
        .phy = 1,
        .data_len = 251,
    },
    [WTYI_BLE_PROFILE_LOW_LATENCY] = {
        .interval_min = WTYI_BLE_LOW_LATENCY_INTERVAL_MIN,
        .interval_max = WTYI_BLE_LOW_LATENCY_INTERVAL_MAX,
        .latency = WTYI_BLE_LOW_LATENCY_LATENCY,
        .timeout = WTYI_BLE_LOW_LATENCY_TIMEOUT,
        .mtu = 247,
        .phy = 1,
        .data_len = 251,
    },
};

#if WTYI_BLE_PROFILE_DEFAULT < WTYI_BLE_PROFILE_COMPATIBLE || WTYI_BLE_PROFILE_DEFAULT > WTYI_BLE_PROFILE_LOW_LATENCY
#error "Invalid WTYI_BLE_PROFILE_DEFAULT"
#endif

static int wtyi_ble_param_valid(const struct wtyi_ble_conn_param *p)
{
    u32 max_interval_ms_x100;
    u32 timeout_ms_x100;

    if (!p || p->interval_min < 6 || p->interval_max > 3200 ||
        p->interval_min > p->interval_max || p->latency > 499 ||
        p->timeout < 10 || p->timeout > 3200) {
        return 0;
    }

    max_interval_ms_x100 = (u32)p->interval_max * WTYI_BLE_INTERVAL_UNIT_MS_X100;
    timeout_ms_x100 = (u32)p->timeout * WTYI_BLE_TIMEOUT_UNIT_MS * 100;
    return timeout_ms_x100 > ((1 + (u32)p->latency) * max_interval_ms_x100 * 2);
}

const struct wtyi_ble_conn_param *wtyi_ble_get_default_params(void)
{
    const struct wtyi_ble_conn_param *p = &wtyi_ble_profiles[WTYI_BLE_PROFILE_DEFAULT];

    if (!wtyi_ble_param_valid(p)) {
        return &wtyi_ble_profiles[WTYI_BLE_PROFILE_COMPATIBLE];
    }
    return p;
}

void wtyi_ble_print_request_params(void)
{
    const struct wtyi_ble_conn_param *p = wtyi_ble_get_default_params();

    WTYI_LOG_BLE("request interval=%u-%u latency=%u timeout=%u mtu=%u phy=%u data_len=%u\r\n",
                 p->interval_min, p->interval_max, p->latency, p->timeout,
                 p->mtu, p->phy, p->data_len);
}

void wtyi_ble_print_actual_params(u16 interval, u16 latency, u16 timeout, u16 mtu, u8 phy, u16 data_len)
{
    WTYI_LOG_BLE("actual interval=%u latency=%u timeout=%u mtu=%u phy=%u data_len=%u\r\n",
                 interval, latency, timeout, mtu, phy, data_len);
}

void wtyi_ble_print_actual_conn_params(u16 interval, u16 latency, u16 timeout)
{
    WTYI_LOG_BLE("actual interval=%u latency=%u timeout=%u\r\n",
                 interval, latency, timeout);
}

void wtyi_ble_print_actual_mtu(u16 mtu)
{
    WTYI_LOG_BLE("actual mtu=%u\r\n", mtu);
}
