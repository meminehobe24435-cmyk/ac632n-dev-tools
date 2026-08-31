# WT9011DCL-BT50 Hardware Test Checklist

Use this only after a firmware is successfully flashed. Current stage did not flash hardware.

## 0. PC Offline Check

User action:

- Run:

```bat
tools\wtyi\run_all_checks.bat
```

Expected:

```text
Checks passed. Report: project_docs\run_all_checks_report.md
```

Success:

- Environment check passes.
- `build_only` passes.
- Firmware contains `WT9011DCL-BT50`.
- Firmware contains WTYI log strings.

Failure:

- Script exits before `Checks passed`.

Rollback:

- Do not flash. Send the failing console text or `project_docs/run_all_checks_report.md`.

## 1. Bluetooth Name

User action:

- Power-cycle the board after successful flashing.
- Scan with phone system classic Bluetooth list.
- Scan with a BLE scanner app.

Expected:

- Classic Bluetooth name: `WT9011DCL-BT50`
- BLE advertising name: `WT9011DCL-BT50`

Success:

- Both classic and BLE names match exactly.

Failure:

- Old name still appears.
- BLE scanner sees no device after normal boot.

Rollback:

- Flash the baseline firmware recorded in `project_docs/baseline_build_report.md`.

## 2. ADC

Prerequisite:

- Build with `WTYI_TEST_ADC_ENABLE = 1`.
- UART log path must be confirmed.

Expected log:

```text
[WTYI_ADC] init ch=PA1 samples=32 divider=4/1 cal=1000/1000
[WTYI_ADC] raw=<value> input_mv=<value> vbat_mv=<value>
```

User action:

- Measure VBAT with a multimeter and report the voltage.

Success:

- Printed `vbat_mv` differs from multimeter value by no more than 5%.

Failure:

- Raw value fixed at 0 or max.
- Calculated VBAT is far from multimeter value.

Rollback:

- Set `WTYI_TEST_ADC_ENABLE = 0`.

## 3. SPI QMI8685A

Prerequisite:

- Confirm QMI8685A register map or compatible vendor driver.
- Build with `WTYI_TEST_SPI_ENABLE = 1`.

Expected log before register map is added:

```text
[WTYI_SPI] transport ready, qmi8685a register map unconfirmed
[WTYI_SPI] qmi8685a register map unconfirmed, skip id/raw read
```

User action after register map is implemented:

- Rotate the board once when prompted.

Success:

- Device ID/fixed register read is valid.
- Acceleration and gyro raw values change when the board rotates.

Failure:

- SPI timeout.
- All values are `0x00`, `0xff`, or fixed while rotating.

Rollback:

- Set `WTYI_TEST_SPI_ENABLE = 0`.

## 4. IIC QMC5883P

Prerequisite:

- Confirm QMC5883P address/register map.
- Build with `WTYI_TEST_IIC_ENABLE = 1`.

Expected log before register map is added:

```text
[WTYI_IIC] init start scl=USB1DP sda=USB1DM
[WTYI_IIC] transport ready, qmc5883p register map unconfirmed
[WTYI_IIC] scan count=<n> ...
```

User action after register map is implemented:

- Rotate the board once when prompted.

Success:

- IIC scan finds the expected QMC5883P address.
- Magnetic XYZ raw values change with board rotation.

Failure:

- No address found.
- All values are `0x00`, `0xff`, or fixed while rotating.

Rollback:

- Set `WTYI_TEST_IIC_ENABLE = 0`.

## 5. BLE Connection Parameters

Prerequisite:

- BLE connection is stable with default `COMPATIBLE` profile.

Expected logs:

```text
[WTYI_BLE] request interval=24-40 latency=0 timeout=400 mtu=247 phy=1 data_len=251
[WTYI_BLE] actual interval=<value> latency=<value> timeout=<value>
[WTYI_BLE] actual mtu=<value>
```

User action:

- Connect to the BLE device with a phone BLE scanner app.

Success:

- Connection stays stable.
- Actual negotiated values print after connect/update/MTU events.

Failure:

- Connection fails or repeatedly disconnects.
- Actual values never print.

Rollback:

- Keep `WTYI_BLE_PROFILE_DEFAULT` as `WTYI_BLE_PROFILE_COMPATIBLE`.
