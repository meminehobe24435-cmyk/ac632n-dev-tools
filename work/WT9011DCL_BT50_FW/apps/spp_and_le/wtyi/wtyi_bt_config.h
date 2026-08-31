#ifndef WTYI_BT_CONFIG_H
#define WTYI_BT_CONFIG_H

#include "typedef.h"

struct wtyi_ble_conn_param {
    u16 interval_min;
    u16 interval_max;
    u16 latency;
    u16 timeout;
    u16 mtu;
    u8 phy;
    u16 data_len;
};

const struct wtyi_ble_conn_param *wtyi_ble_get_default_params(void);
void wtyi_ble_print_request_params(void);
void wtyi_ble_print_actual_params(u16 interval, u16 latency, u16 timeout, u16 mtu, u8 phy, u16 data_len);
void wtyi_ble_print_actual_conn_params(u16 interval, u16 latency, u16 timeout);
void wtyi_ble_print_actual_mtu(u16 mtu);

#endif
