# WT9011DCL-BT50 Hardware Map

Date: 2026-07-23

## Confirmed Sources

- `project_docs/requirements.md`
- `project_docs/WIT2026053WT9011DCL-BT50 V1.0.pdf`
- `project_docs/ad56307a8a8f2ac3d6a72ed55c81338b.jpg`
- `project_docs/cc9a105667ae8dd3140cbafbe416f14d.jpg`
- `project_docs/lQDPJx8cSxXgF-fNAlfNA6uwWDw4oo4xP_UKLRBYW8ToAA_939_599.jpg` BOM screenshot
- `project_docs/219fad1dc57d1667ebe287e3b89be760.jpg` PCB layout screenshot
- `project_docs/e15502f6ddc6b8fae8a793aadbc19b36.jpg` PCB layout screenshot

## Board Identity

| Item | Confirmed value | Evidence |
| --- | --- | --- |
| Product | WT9011DCL-BT50 | requirements and PCB silkscreen |
| PCB silkscreen | `WT2026053WT9011dcl_bt50 V1.0` | back-side photo |
| MCU | AC6321A4 | schematic U1, top-side chip marking, BOM U1 Comment `ac6321a4` |
| SDK platform | bd19 | requirements and SDK README |
| Application | `apps/spp_and_le` | requirements |
| Download connector | Type-C on USB0 D+/D- | schematic labels `USBD+`, `USBD-` to U1 USB0DP/USB0DM; BOM U7 Comment `TYPE-C-31-M-12` |
| Board key | TSA053G20-250T | BOM switch Comment `TSA053G20-250T` |

## MCU Pin Map From Schematic

| Function | Schematic signal | AC6321A4 pin / SDK IO | Notes |
| --- | --- | --- | --- |
| Type-C USB D- | `USBD-` | USB0DM / `IO_PORT_DM` | Reserve for download. Do not use for IIC. |
| Type-C USB D+ | `USBD+` | USB0DP / `IO_PORT_DP` | Reserve for download. Do not use for IIC. |
| IIC SCL | `MCU_I2C1_SCL`, `QMC_SCL` | USB1DP / `IO_PORT_DP1` | QMC5883 bus, 10K pull-up to 3.3V. |
| IIC SDA | `MCU_I2C1_SDA`, `QMC_SDA` | USB1DM / `IO_PORT_DM1` | QMC5883 bus, 10K pull-up to 3.3V. |
| SPI CS | `MCU_PB0_CS`, `ICM42607_CS` | PB0 / `IO_PORTB_00` | Board uses this as sensor chip-select. |
| SPI SCK | `MCU_SPI1_SCK`, `ICM42607_SCK` | PA7 / `IO_PORTA_07` | Conflicts with SDK demo IIC option C. |
| SPI MOSI | `MCU_SPI1_MOSI`, `ICM42607_MOSI` | PA8 / `IO_PORTA_08` | Conflicts with SDK demo IIC option C. |
| SPI MISO | `MCU_SPI1_MISO`, `ICM42607_MISO` | PA6 / `IO_PORTA_06` | Sensor MISO. |
| SPI INT | `MCU_PA3_INT`, `ICM42607_INT` | PA3 / `IO_PORTA_03` | Interrupt input for motion sensor. |
| Battery ADC | `PWR_ADC` | PA1 / `AD_CH_PA1` | VBAT divider: 3M high side, 1M low side, ADC input is about VBAT / 4. |
| Key | `MCU_P14_KYE` / switch net | switch part TSA053G20-250T | Needs code review before assigning, because PB0 is also SPI CS in product requirements. |
| UART log | not confirmed on PCB | SDK demo uses PA0 / `IO_PORTA_00` | Do not assume PA0 has an accessible test point. |

## Board Devices

| Device | Interface | Confirmed wiring | Validation plan |
| --- | --- | --- | --- |
| QMI8685A | SPI | CS PB0, SCK PA7, MOSI PA8, MISO PA6, INT PA3 | BOM U2 Comment is `QMI8685A`. Footprint/LibRef show `bmi160/BMI160`, treated as reused CAD library only. Need reliable QMI8685A-compatible register map before coding. SDK has no local QMI driver. |
| QMC5883P | IIC | SCL USB1DP, SDA USB1DM, 10K pull-ups to 3.3V | BOM U6 Comment is `QMC5883P`. LibRef shows `IST8308`, treated as reused CAD library only. Need QMC5883P register map. SDK has no local QMC5883 driver. |
| Battery divider | ADC | PA1, `PWR_ADC`, 3M/1M divider | Sample PA1, compute VBAT = ADC input * 4, compare with multimeter. |

## Photo Versus Schematic Check

- Match: PCB name/version, AC6321A4 location, Type-C, key, crystal, U2 sensor position, U6 magnetometer position, and general passive layout match the schematic.
- Match: BOM confirms U1 `ac6321a4`, U2 `QMI8685A`, U6 `QMC5883P`, U7 `TYPE-C-31-M-12`, and switch `TSA053G20-250T`.
- Partial: U2 package marking is not fully readable in the photo. BOM Comment is authoritative for now: use `QMI8685A` direction, not QMI8658A, unless later physical marking or vendor documentation proves otherwise.
- Partial: U2 Footprint/LibRef shows `bmi160/BMI160`; this is a CAD-library reuse warning only and must not drive register selection.
- Partial: U6 LibRef shows `IST8308`; this is a CAD-library reuse warning only and must not drive register selection.
- Partial: PA0 UART test point is not confirmed from the photos.

## Hardware Facts To Preserve

- USB0 D+/D- must remain reserved for Type-C download.
- USB1DP/USB1DM are the product IIC pins and must not be treated as USB download pins.
- PA7/PA8 belong to product SPI and must not be reused as IIC.
- PB1 ADKEY is not the product battery ADC; product battery ADC is PA1.
- USB-TTL is only for UART logs after TX/GND are confirmed.
- Do not use BMI160 register code for U2.
- Do not use IST8308 register code for U6.
- Do not write QMI8658A-specific code unless QMI8685A compatibility is verified from a reliable source.
