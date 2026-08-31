#include "app_config.h"
#include "system/includes.h"
#include "asm/spi.h"
#include "asm/gpio.h"
#include "wtyi_spi_imu.h"
#include "wtyi_log.h"
#include "wtyi_power_manager.h"

/* QMI8685A register map is not confirmed yet. Do not fill guessed addresses. */
#define WTYI_QMI8685A_CHIP_ID_REG_UNCONFIRMED  0xff

static void wtyi_spi_cs_init(void)
{
    gpio_set_direction(WTYI_SPI_CS_PORT, 0);
    gpio_write(WTYI_SPI_CS_PORT, 1);
    gpio_set_direction(WTYI_SPI_INT_PORT, 1);
}

static void wtyi_spi_cs_low(void)
{
    gpio_write(WTYI_SPI_CS_PORT, 0);
}

static void wtyi_spi_cs_high(void)
{
    gpio_write(WTYI_SPI_CS_PORT, 1);
}

static int wtyi_spi_transfer_byte(u8 tx, u8 *rx)
{
    int err = 0;
    u8 data = spi_send_recv_byte(SPI1, tx, &err);

    if (rx) {
        *rx = data;
    }
    return err ? WTYI_SPI_ERR_BUS : WTYI_SPI_OK;
}

int wtyi_spi_imu_init(void)
{
    int ret;

    wtyi_power_spi_set_busy(1);
    WTYI_LOG_SPI("init start cs=PB0 sck=PA7 mosi=PA8 miso=PA6 int=PA3\r\n");
    wtyi_spi_cs_init();
    ret = spi_open(SPI1);
    if (ret) {
        WTYI_LOG_ERROR("spi open failed\r\n");
        wtyi_power_spi_set_busy(0);
        return WTYI_SPI_ERR_BUS;
    }
    WTYI_LOG_SPI("transport ready, qmi8685a register map unconfirmed\r\n");
    wtyi_power_spi_set_busy(0);
    return WTYI_SPI_OK;
}

int wtyi_spi_imu_read_reg(u8 reg, u8 *value)
{
    int ret;

    if (!value) {
        return WTYI_SPI_ERR_PARAM;
    }
    if (reg == WTYI_QMI8685A_CHIP_ID_REG_UNCONFIRMED) {
        return WTYI_SPI_ERR_UNCONFIRMED_REG;
    }

    wtyi_power_spi_set_busy(1);
    wtyi_spi_cs_low();
    ret = wtyi_spi_transfer_byte(reg, NULL);
    if (!ret) {
        ret = wtyi_spi_transfer_byte(0xff, value);
    }
    wtyi_spi_cs_high();
    wtyi_power_spi_set_busy(0);

    return ret;
}

int wtyi_spi_imu_write_reg(u8 reg, u8 value)
{
    int ret;

    if (reg == WTYI_QMI8685A_CHIP_ID_REG_UNCONFIRMED) {
        return WTYI_SPI_ERR_UNCONFIRMED_REG;
    }

    wtyi_power_spi_set_busy(1);
    wtyi_spi_cs_low();
    ret = wtyi_spi_transfer_byte(reg, NULL);
    if (!ret) {
        ret = wtyi_spi_transfer_byte(value, NULL);
    }
    wtyi_spi_cs_high();
    wtyi_power_spi_set_busy(0);

    return ret;
}

int wtyi_spi_imu_read_regs(u8 start_reg, u8 *buf, u16 len)
{
    u16 i;
    int ret;

    if (!buf || !len) {
        return WTYI_SPI_ERR_PARAM;
    }
    if (start_reg == WTYI_QMI8685A_CHIP_ID_REG_UNCONFIRMED) {
        return WTYI_SPI_ERR_UNCONFIRMED_REG;
    }

    wtyi_power_spi_set_busy(1);
    wtyi_spi_cs_low();
    ret = wtyi_spi_transfer_byte(start_reg, NULL);
    for (i = 0; !ret && i < len; i++) {
        ret = wtyi_spi_transfer_byte(0xff, &buf[i]);
    }
    wtyi_spi_cs_high();
    wtyi_power_spi_set_busy(0);

    return ret;
}

int wtyi_spi_imu_read_id(u8 *id)
{
    (void)id;
    return WTYI_SPI_ERR_UNCONFIRMED_REG;
}

int wtyi_spi_imu_read_raw(struct wtyi_imu_raw *raw)
{
    (void)raw;
    return WTYI_SPI_ERR_UNCONFIRMED_REG;
}

void wtyi_spi_imu_test_once(void)
{
    u8 id = 0;
    int ret = wtyi_spi_imu_read_id(&id);

    if (ret == WTYI_SPI_ERR_UNCONFIRMED_REG) {
        WTYI_LOG_SPI("qmi8685a register map unconfirmed, skip id/raw read\r\n");
        return;
    }
    WTYI_LOG_SPI("device id=0x%02x ret=%d\r\n", id, ret);
}
