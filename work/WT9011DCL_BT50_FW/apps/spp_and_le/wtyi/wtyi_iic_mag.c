#include "app_config.h"
#include "system/includes.h"
#include "asm/iic_soft.h"
#include "wtyi_iic_mag.h"
#include "wtyi_log.h"
#include "wtyi_power_manager.h"

/* QMC5883P register map/address is not confirmed yet. Do not fill guessed values. */

static int wtyi_iic_write_addr(u8 addr7, u8 read)
{
    return soft_iic_tx_byte(WTYI_IIC_DEV, (addr7 << 1) | (read ? 1 : 0)) ?
           WTYI_IIC_OK : WTYI_IIC_ERR_NACK;
}

int wtyi_iic_mag_init(void)
{
    wtyi_power_iic_set_busy(1);
    WTYI_LOG_IIC("init start scl=USB1DP sda=USB1DM\r\n");
    soft_iic_init(WTYI_IIC_DEV);
    WTYI_LOG_IIC("transport ready, qmc5883p register map unconfirmed\r\n");
    wtyi_power_iic_set_busy(0);
    return WTYI_IIC_OK;
}

int wtyi_iic_mag_scan(u8 *addr_buf, u8 max_count)
{
    u8 addr;
    u8 count = 0;

    if (!addr_buf || !max_count) {
        return WTYI_IIC_ERR_PARAM;
    }

    wtyi_power_iic_set_busy(1);
    for (addr = 0x08; addr < 0x78; addr++) {
        soft_iic_start(WTYI_IIC_DEV);
        if (wtyi_iic_write_addr(addr, 0) == WTYI_IIC_OK) {
            if (count < max_count) {
                addr_buf[count++] = addr;
            }
        }
        soft_iic_stop(WTYI_IIC_DEV);
    }
    wtyi_power_iic_set_busy(0);

    return count;
}

int wtyi_iic_mag_read_reg(u8 dev_addr, u8 reg, u8 *value)
{
    int ret;

    if (!value) {
        return WTYI_IIC_ERR_PARAM;
    }

    wtyi_power_iic_set_busy(1);
    soft_iic_start(WTYI_IIC_DEV);
    ret = wtyi_iic_write_addr(dev_addr, 0);
    if (ret == WTYI_IIC_OK && !soft_iic_tx_byte(WTYI_IIC_DEV, reg)) {
        ret = WTYI_IIC_ERR_NACK;
    }
    if (ret == WTYI_IIC_OK) {
        soft_iic_start(WTYI_IIC_DEV);
        ret = wtyi_iic_write_addr(dev_addr, 1);
    }
    if (ret == WTYI_IIC_OK) {
        *value = soft_iic_rx_byte(WTYI_IIC_DEV, 0);
    }
    soft_iic_stop(WTYI_IIC_DEV);
    wtyi_power_iic_set_busy(0);

    return ret;
}

int wtyi_iic_mag_write_reg(u8 dev_addr, u8 reg, u8 value)
{
    int ret;

    wtyi_power_iic_set_busy(1);
    soft_iic_start(WTYI_IIC_DEV);
    ret = wtyi_iic_write_addr(dev_addr, 0);
    if (ret == WTYI_IIC_OK && !soft_iic_tx_byte(WTYI_IIC_DEV, reg)) {
        ret = WTYI_IIC_ERR_NACK;
    }
    if (ret == WTYI_IIC_OK && !soft_iic_tx_byte(WTYI_IIC_DEV, value)) {
        ret = WTYI_IIC_ERR_NACK;
    }
    soft_iic_stop(WTYI_IIC_DEV);
    wtyi_power_iic_set_busy(0);

    return ret;
}

int wtyi_iic_mag_read_regs(u8 dev_addr, u8 start_reg, u8 *buf, u16 len)
{
    u16 i;
    int ret;

    if (!buf || !len) {
        return WTYI_IIC_ERR_PARAM;
    }

    wtyi_power_iic_set_busy(1);
    soft_iic_start(WTYI_IIC_DEV);
    ret = wtyi_iic_write_addr(dev_addr, 0);
    if (ret == WTYI_IIC_OK && !soft_iic_tx_byte(WTYI_IIC_DEV, start_reg)) {
        ret = WTYI_IIC_ERR_NACK;
    }
    if (ret == WTYI_IIC_OK) {
        soft_iic_start(WTYI_IIC_DEV);
        ret = wtyi_iic_write_addr(dev_addr, 1);
    }
    for (i = 0; ret == WTYI_IIC_OK && i < len; i++) {
        buf[i] = soft_iic_rx_byte(WTYI_IIC_DEV, (i + 1 < len) ? 1 : 0);
    }
    soft_iic_stop(WTYI_IIC_DEV);
    wtyi_power_iic_set_busy(0);

    return ret;
}

int wtyi_iic_mag_read_raw(struct wtyi_mag_raw *raw)
{
    (void)raw;
    return WTYI_IIC_ERR_UNCONFIRMED_REG;
}

void wtyi_iic_mag_test_once(void)
{
    u8 found[8];
    int count = wtyi_iic_mag_scan(found, sizeof(found));
    int i;

    WTYI_LOG_IIC("scan count=%d", count);
    for (i = 0; i < count; i++) {
        WTYI_LOG(" ", "0x%02x", found[i]);
    }
    WTYI_LOG("", "\r\n");
    WTYI_LOG_IIC("qmc5883p register map unconfirmed, skip raw read\r\n");
}
