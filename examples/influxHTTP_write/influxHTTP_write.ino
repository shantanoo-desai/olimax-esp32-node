/*
 * This Sketch provides an example to send information via HTTP Post to InfluxDB
 * on your local network.
 * Currently a simple numeric value is sent as Line Protocol to InfluxDB without
 * timing precision.
 *
 * Author: Shantanoo Desai <des@biba.uni-bremen.de>
 */

#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#include <ETH.h>
#include "UMG_Node.h"

/* STATIC IP CONFIGURATION for the Node */
/* NOTE: not necessary if you have DHCP on your network and know
         the IP address of InfluxDB
*/
const IPAddress node_static_ip(192, 168, 3, 101);
const IPAddress default_gateway(192, 168, 3, 250);
const IPAddress subnet_mask(255, 255, 255, 0);
const IPAddress dns_address(192, 168, 3, 250);

/* InfluxDB CONFIGURATION */

const char* INFLUXDB_IP = "192.168.3.11"; // add your InfluxDB's IP address here
const uint16_t INFLUX_PORT = 8086;
const char* INFLUX_DB_NAME = "example"; // add the name of the Database

// create databse for the above configuration
InfluxDB db(INFLUXDB_IP, INFLUX_PORT, INFLUX_DB_NAME);

// random number to be used as field
long randomNumber;

// ethernet connectivity bool flag
static bool eth_connected = false;

// Event Handler for Ethernet Port
void EthEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
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

void setup()
{
  Serial.begin(115200);
  WiFi.onEvent(EthEvent);

  ETH.begin();
  // Enable STATIC IP Address for the Node
  // NOTE: not necessary if DHCP used.
  ETH.config(node_static_ip, default_gateway, subnet_mask, dns_address);
  delay(1000);
}

void loop()
{
  if (eth_connected) {
    randomNumber = random(10, 20);
    influxMeasurement row("random"); // Name of the Measurement
    row.addTag("name", "node1"); // add tag for your measurement
    row.addTag("loc", "lab");
    row.addField("value", randomNumber); // add field name and value

    // Serial.println(row.postString());// DEBUG: See how your Line Protocol Looks like..

    // Send it via HTTP
    // if no RTC available then precision = "".
    // InfluxDB will add timestamps upon receiving measurment
    // If precision is not "s|ms|u|ns" default timestamps will be in "ns"
    Serial.println((db.httpWrite(row, "")) == INFLUX_SUCCESS ? "Success Write" : "Failed Write");
  }
  delay(5000);
}
