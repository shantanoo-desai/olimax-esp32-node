/*  This Sketch provides Epoch Timestamps and converts them to Human Readable
 *  Format from a Local NTP Server. This Local NTP Server can be connected to
 *  your network.
 *  Author: Shantanoo Desai   <des@biba.uni-bremen.de>
 */

#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12
#define I2C_SDA 13
#define I2C_SCL 16

#include <Wire.h>
#include <ETH.h>
#include <WiFiUdp.h> // UDP Socket needed for NTP
#include <NTPClient.h> // NTPClient Library
#include <RTClib.h> // needed for using DateTime

WiFiUDP ntpUDP; // provide a UDP Socket for NTP

/* STATIC IP CONFIGURATION for the Node */
/* NOTE: not necessary if you have DHCP on your network and know
        the IP address of InfluxDB
*/
const IPAddress node_static_ip(192, 168, 3, 101);
const IPAddress default_gateway(192, 168, 3, 250);
const IPAddress subnet_mask(255, 255, 255, 0);
const IPAddress dns_add(192, 168, 3, 250);

// Configure your NTP Client
// FORMAT:
// NTPClient(UDPSocket, IP Address of NTP Server, TimeZoneOffSet, Update Rate);
// here 0 means UTC, For UTC+1H use 3600 and so on..
NTPClient timeClient(ntpUDP, "192.168.3.11", 0, 60000);


void setup() {

  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  ETH.begin();
  ETH.config(node_static_ip, default_gateway, subnet_mask, dns_add);
  delay(1000);

  timeClient.begin();

}

void loop() {

  timeClient.update(); // update client
  Serial.println(timeClient.getEpochTime());

  // convert Epoch Timestamp to Human Readable format
  DateTime now_time(timeClient.getEpochTime());
  Serial.print(now_time.year(), DEC);
    Serial.print('/');
    Serial.print(now_time.month(), DEC);
    Serial.print('/');
    Serial.print(now_time.day(), DEC);
    Serial.print(" ");
    Serial.print(now_time.hour(), DEC);
    Serial.print(':');
    Serial.print(now_time.minute(), DEC);
    Serial.print(':');
    Serial.print(now_time.second(), DEC);
    Serial.println();

    delay(2000);
}
