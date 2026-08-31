#ifndef WTYI_CONFIG_H
#define WTYI_CONFIG_H

/* Central WT9011DCL-BT50 board-test configuration. */

#define WTYI_BOARD_NAME                 "WT9011DCL_BT50"

#ifndef WTYI_TEST_SPI_ENABLE
#define WTYI_TEST_SPI_ENABLE            0
#endif

#ifndef WTYI_TEST_IIC_ENABLE
#define WTYI_TEST_IIC_ENABLE            0
#endif

#ifndef WTYI_TEST_ADC_ENABLE
#define WTYI_TEST_ADC_ENABLE            0
#endif

#ifndef WTYI_BT_LOG_ENABLE
#define WTYI_BT_LOG_ENABLE              1
#endif

#ifndef WTYI_UART_LOG_ENABLE
#define WTYI_UART_LOG_ENABLE            0
#endif

#ifndef WTYI_TEST_POLL_ENABLE
#define WTYI_TEST_POLL_ENABLE           0
#endif

#ifndef WTYI_LOW_POWER_ENABLE
#define WTYI_LOW_POWER_ENABLE           1
#endif

#define WTYI_SPI_CS_PORT                IO_PORTB_00
#define WTYI_SPI_SCK_PORT               IO_PORTA_07
#define WTYI_SPI_MOSI_PORT              IO_PORTA_08
#define WTYI_SPI_MISO_PORT              IO_PORTA_06
#define WTYI_SPI_INT_PORT               IO_PORTA_03

#define WTYI_IIC_DEV                    0
#define WTYI_IIC_SCL_PORT               IO_PORT_DP1
#define WTYI_IIC_SDA_PORT               IO_PORT_DM1
#define WTYI_IIC_DELAY_CNT              50

#define WTYI_ADC_PWR_CH                 AD_CH_PA1
#define WTYI_ADC_SAMPLE_COUNT           32
#define WTYI_ADC_TRIM_COUNT             4
#define WTYI_ADC_DIVIDER_NUM            4
#define WTYI_ADC_DIVIDER_DEN            1
#define WTYI_ADC_CAL_NUM                1000
#define WTYI_ADC_CAL_DEN                1000

#define WTYI_BLE_PROFILE_COMPATIBLE     0
#define WTYI_BLE_PROFILE_BALANCED       1
#define WTYI_BLE_PROFILE_LOW_LATENCY    2
#define WTYI_BLE_PROFILE_DEFAULT        WTYI_BLE_PROFILE_COMPATIBLE

#define WTYI_BLE_COMPATIBLE_INTERVAL_MIN    24
#define WTYI_BLE_COMPATIBLE_INTERVAL_MAX    40
#define WTYI_BLE_COMPATIBLE_LATENCY         0
#define WTYI_BLE_COMPATIBLE_TIMEOUT         400

#define WTYI_BLE_BALANCED_INTERVAL_MIN      12
#define WTYI_BLE_BALANCED_INTERVAL_MAX      24
#define WTYI_BLE_BALANCED_LATENCY           0
#define WTYI_BLE_BALANCED_TIMEOUT           300

#define WTYI_BLE_LOW_LATENCY_INTERVAL_MIN   6
#define WTYI_BLE_LOW_LATENCY_INTERVAL_MAX   12
#define WTYI_BLE_LOW_LATENCY_LATENCY        0
#define WTYI_BLE_LOW_LATENCY_TIMEOUT        200

#endif
