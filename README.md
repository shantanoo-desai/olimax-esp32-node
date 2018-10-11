# UMG_Node

UMG_Node provides `String` based InfluxDB measurements in Line Protocol Format
sent via HTTP/UDP to a central InfluxDB instance.

### Hardware

- Olimex ESP32-PoE

### Software

`arduino-esp32` library for ESP32 programming.

Install it via `Board-Manager` from Arduino IDE as mentioned [here](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)

### Interfacing

| Interface | Hardware Pins | GPIO Pins |
|--|--|--|
| __I2C__ | `EXT2_7_SCL` `EXT2_10_SDA` `UEXT_5_SCL`, `UEXT_6_SDA`| `GPIO_13 -> I2C_SDA` `GPIO_16 -> SCL` |


### Note

In order to use the Olimex PoE module add the following to the beginning of every Arduino Sketch

    #define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
    #define ETH_PHY_POWER 12

    #include <ETH.h>
    // code

Use `ETH_LAN8260` under `WiFi` for `Olimex-ESP32 Gateway` with the above mentioned values for initial tests.


### Examples

| Sketch | Description |
|--|--|
| `influxPing` | Simple Sketch to check if the node is able to reach InfluxDB |
| `influxHTTP_write` | Sending Line Protocol Information via `HTTP` |
| `influxUDP_write` | Sending Line Protocol Information via `UDP` |
| `rtcTimestamp` | Add Epoch Timestamps to Line Protocol from `RTC DS3231` |
| `bnoCalibStorage` | Sketch to store the `BNO055` to ESP32's Non-Volatile Storage (`NVS`) |


## Maintainers

- Shantanoo Desai (des@biba.uni-bremen.de)
