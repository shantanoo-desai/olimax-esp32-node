#include "UMG_CONF.h"
#include <Wire.h> //  For I2C
#include <ETH.h> // Ethernet Library for ESP32
#include <AsyncUDP.h> // UDP Library
#include <UMG_Node.h> // UMG Application Library

/* BNO055 Sensor Library */
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <RTClib.h> // RTC DS3231

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>

Preferences Calibration;

/* Flags */
static bool eth_connected = false; // ethernet connection flag
static bool calibrated = false; // calibration status flag

/* INFLUXDB CONF for UMG NODE */
AsyncUDP udp;
InfluxDB db = InfluxDB();

/* BNO055 CONF */
Adafruit_BNO055 bno = Adafruit_BNO055(55);
imu::Quaternion quat;
imu::Vector<3> euler;
imu::Vector<3> li_ac;
imu::Vector<3> acc;

/* RTC DS3231
    RTC with NTP is currently under testing conditions and not used for Deployment
    Instead Timestamps from InfluxDB are used. The buffer size of the InfluxDB
    UDP setup is increased in order to avoid any packet loss
 */
// DateTime UTC_TIME(2018, 10, 18, 11, 37, 0);

// RTC_DS3231 rtc;
// const byte SQW_PIN = 36;
// volatile uint32_t seconds_since_epoch;
// volatile uint32_t micros_on_tick;

/*NTP Client
    NTP + RTC is currently undertesting.
*/
// WiFiUDP ntpUDP;
//
// NTPClient timeClient(ntpUDP, "192.168.3.11", 0, 60000);

/* Interrupt for Milliseconds */

// void tick() {
//     seconds_since_epoch++;
//     micros_on_tick = micros();
// }
//
// uint64_t millis_since_epoch() {
//     noInterrupts();
//     uint32_t seconds = seconds_since_epoch;
//     uint32_t microseconds = micros_on_tick;
//     interrupts();
//     uint32_t extra_micros = micros() - microseconds;
//     uint16_t extra_millis = extra_micros / 1000;
//     if (extra_millis > 999)
//         extra_millis = 999;
//     return (uint64_t) seconds * 1000 + extra_millis;
// }

/* Event Handler for Ethernet Connection */

void EthEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet-101");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}


void restoreCalibOffsets()
{
    Calibration.begin("offsets", false);
    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    int32_t bnoID = Calibration.getInt("bnoID");

    bno.getSensor(&sensor);
    if (bnoID != sensor.sensor_id)
    {
        Serial.println("\nNo Calibration Data for this sensor exists in NVS");
         while(1);
    }
    else
    {
        Serial.println("\nFound Calibration for this sensor in NVS.");

        // Accelerometer offsets
        calibrationData.accel_offset_x = Calibration.getInt("acc_off_x");
        calibrationData.accel_offset_y = Calibration.getInt("acc_off_y");
        calibrationData.accel_offset_z = Calibration.getInt("acc_off_z");

        // Gyroscrope offsets
        calibrationData.gyro_offset_x = Calibration.getInt("gyr_off_x");
        calibrationData.gyro_offset_y = Calibration.getInt("gyr_off_y");
        calibrationData.gyro_offset_z = Calibration.getInt("gyr_off_z");

        // Magnetometer offsets
        calibrationData.mag_offset_x = Calibration.getInt("mag_off_x");
        calibrationData.mag_offset_y = Calibration.getInt("mag_off_y");
        calibrationData.mag_offset_z = Calibration.getInt("mag_off_z");

        Serial.println("\n\nRestoring Calibration data to the BNO055...");
        bno.setSensorOffsets(calibrationData);

        Serial.println("\n\nCalibration data loaded into BNO055");
        calibrated = true;
    }
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin(I2C_SDA, I2C_SCL, 400000); // start i2c
  Serial.begin(115200);
  WiFi.onEvent(EthEvent);

  if(!bno.begin()) {
    Serial.println("No BNO055 Detected!");
    while(1);
  }

  // if (!rtc.begin()) {
  //   Serial.println("No RTC Detected!");
  //   while(1);
  // }

  // NTP Client
//  timeClient.begin();
  delay(1000);

  //setup BNO
  bno.setMode(Adafruit_BNO055::OPERATION_MODE_NDOF);
  bno.setExtCrystalUse(true);

  delay(1000);
  // RTC Interrupt setting
  // pinMode(SQW_PIN, INPUT_PULLUP);
  // rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  // attachInterrupt(digitalPinToInterrupt(SQW_PIN), tick, FALLING);
  WiFi.onEvent(EthEvent);

  // Ethernet Enable with static IP Configuration
  ETH.begin();
  ETH.config(node_static_ip, default_gateway, subnet_mask, dns_add);
  delay(1000);
  restoreCalibOffsets();
}

void loop() {
  // put your main code here, to run repeatedly:

  if (eth_connected) {
    // get timestamp from NTP Server
//    timeClient.update();
//    Serial.println(timeClient.getEpochTime());
//    DateTime now_time(timeClient.getEpochTime());
//    if (!(now_time.year() >= 2100)) {
//      rtc.adjust(now_time);
//    }
//    seconds_since_epoch = rtc.now().unixtime();
//    Serial.println(seconds_since_epoch);
    quat = bno.getQuat();
    quat.normalize();
    euler = quat.toEuler();
    euler.toDegrees();
    li_ac = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    acc = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

    influxMeasurement row(measurement_name);
    row.addTag("n", node_name);
    row.addTag("plcmt", node_placement);
    if (!isnan(euler[1]) || euler[0] != 0.00 || euler[2] != 0.00) {
        row.addField("yaw", euler.x());
        row.addField("pitch", euler.y());
        row.addField("roll", euler.z());
    }
    row.addField("laX", li_ac.x());
    row.addField("laY", li_ac.y());
    row.addField("laZ", li_ac.z());
    row.addField("accX", acc.x());
    row.addField("accY", acc.y());
    row.addField("accZ", acc.z());
    row.addField("status", 0);
//    row.addTimeStamp_ms(millis_since_epoch());
     Serial.println(row.postString()); // DEBUG Serial Print the Line Protocol
    db.udpWrite(INFLUX_IP, INFLUX_PORT, row);
    delay(100);
  }

}
