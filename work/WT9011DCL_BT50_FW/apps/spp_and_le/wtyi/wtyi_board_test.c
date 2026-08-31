#include "app_config.h"
#include "system/includes.h"
#include "wtyi_board_test.h"
#include "wtyi_log.h"
#include "wtyi_spi_imu.h"
#include "wtyi_iic_mag.h"
#include "wtyi_adc_battery.h"
#include "wtyi_bt_config.h"
#include "wtyi_power_manager.h"

void wtyi_board_test_init(void)
{
    WTYI_LOG_BOOT("board=%s build=%s %s\r\n", WTYI_BOARD_NAME, __DATE__, __TIME__);
    WTYI_LOG_BT("classic_name=%s\r\n", WTYI_BT_CLASSIC_NAME);
    WTYI_LOG_BLE("adv_name=%s\r\n", WTYI_BT_BLE_NAME);
    wtyi_ble_print_request_params();
    wtyi_power_manager_init();

#if WTYI_TEST_SPI_ENABLE
    wtyi_spi_imu_init();
#else
    WTYI_LOG_SPI("disabled\r\n");
#endif

#if WTYI_TEST_IIC_ENABLE
    wtyi_iic_mag_init();
#else
    WTYI_LOG_IIC("disabled\r\n");
#endif

#if WTYI_TEST_ADC_ENABLE
    wtyi_adc_battery_init();
#else
    WTYI_LOG_ADC("disabled\r\n");
#endif
}

void wtyi_board_test_run_once(void)
{
#if WTYI_TEST_SPI_ENABLE
    wtyi_spi_imu_test_once();
#endif

#if WTYI_TEST_IIC_ENABLE
    wtyi_iic_mag_test_once();
#endif

#if WTYI_TEST_ADC_ENABLE
    struct wtyi_adc_battery_sample sample;
    if (wtyi_adc_battery_read(&sample) == 0) {
        WTYI_LOG_ADC("raw=%lu input_mv=%lu vbat_mv=%lu\r\n",
                     sample.raw_avg, sample.input_mv, sample.vbat_mv);
    }
#endif
}

void wtyi_board_test_poll(void)
{
#if WTYI_TEST_POLL_ENABLE
    wtyi_board_test_run_once();
#endif
}
