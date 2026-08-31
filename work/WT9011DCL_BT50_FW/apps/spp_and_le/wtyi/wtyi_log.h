#ifndef WTYI_LOG_H
#define WTYI_LOG_H

#include "wtyi_config.h"

#if WTYI_BT_LOG_ENABLE || WTYI_UART_LOG_ENABLE
extern int printf(const char *format, ...);
#define WTYI_LOG(tag, fmt, ...)         printf(tag " " fmt, ##__VA_ARGS__)
#else
#define WTYI_LOG(tag, fmt, ...)
#endif

#define WTYI_LOG_BOOT(fmt, ...)         WTYI_LOG("[WTYI_BOOT]", fmt, ##__VA_ARGS__)
#define WTYI_LOG_BT(fmt, ...)           WTYI_LOG("[WTYI_BT]", fmt, ##__VA_ARGS__)
#define WTYI_LOG_BLE(fmt, ...)          WTYI_LOG("[WTYI_BLE]", fmt, ##__VA_ARGS__)
#define WTYI_LOG_SPI(fmt, ...)          WTYI_LOG("[WTYI_SPI]", fmt, ##__VA_ARGS__)
#define WTYI_LOG_IIC(fmt, ...)          WTYI_LOG("[WTYI_IIC]", fmt, ##__VA_ARGS__)
#define WTYI_LOG_ADC(fmt, ...)          WTYI_LOG("[WTYI_ADC]", fmt, ##__VA_ARGS__)
#define WTYI_LOG_ERROR(fmt, ...)        WTYI_LOG("[WTYI_ERROR]", fmt, ##__VA_ARGS__)

#endif
