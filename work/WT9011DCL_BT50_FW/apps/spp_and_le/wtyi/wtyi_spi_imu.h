#ifndef WTYI_SPI_IMU_H
#define WTYI_SPI_IMU_H

#include "typedef.h"

#define WTYI_SPI_OK                    0
#define WTYI_SPI_ERR_PARAM            -1
#define WTYI_SPI_ERR_BUS              -2
#define WTYI_SPI_ERR_UNCONFIRMED_REG  -3

struct wtyi_imu_raw {
    s16 acc_x;
    s16 acc_y;
    s16 acc_z;
    s16 gyro_x;
    s16 gyro_y;
    s16 gyro_z;
};

int wtyi_spi_imu_init(void);
int wtyi_spi_imu_read_id(u8 *id);
int wtyi_spi_imu_read_reg(u8 reg, u8 *value);
int wtyi_spi_imu_write_reg(u8 reg, u8 value);
int wtyi_spi_imu_read_regs(u8 start_reg, u8 *buf, u16 len);
int wtyi_spi_imu_read_raw(struct wtyi_imu_raw *raw);
void wtyi_spi_imu_test_once(void);

#endif
