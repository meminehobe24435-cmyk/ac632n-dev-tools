#ifndef WTYI_IIC_MAG_H
#define WTYI_IIC_MAG_H

#include "typedef.h"

#define WTYI_IIC_OK                    0
#define WTYI_IIC_ERR_PARAM            -1
#define WTYI_IIC_ERR_NACK             -2
#define WTYI_IIC_ERR_UNCONFIRMED_REG  -3

struct wtyi_mag_raw {
    s16 x;
    s16 y;
    s16 z;
};

int wtyi_iic_mag_init(void);
int wtyi_iic_mag_scan(u8 *addr_buf, u8 max_count);
int wtyi_iic_mag_read_reg(u8 dev_addr, u8 reg, u8 *value);
int wtyi_iic_mag_write_reg(u8 dev_addr, u8 reg, u8 value);
int wtyi_iic_mag_read_regs(u8 dev_addr, u8 start_reg, u8 *buf, u16 len);
int wtyi_iic_mag_read_raw(struct wtyi_mag_raw *raw);
void wtyi_iic_mag_test_once(void);

#endif
