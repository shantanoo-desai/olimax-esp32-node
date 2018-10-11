/*
 * This Sketch provides a very basic Ping test to an InfluxDB connected
 * to your local network.
 * Every InfluxDB's HTTP API can be checked via `/ping` interface.
 * One needs to only execute HTTP GET on [IP_Address_InfluxDB]:8086/ping
 *
 * NOTE: This Sketch is similar to Olimex ESP32 Gateway -> WiFi -> ETH_LAN8260.ino
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

void testClient(const char * host, uint16_t port)
{
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  /* use the `/ping` API for testing connectivity */
  client.printf("GET /ping HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
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
    testClient(INFLUXDB_IP, INFLUX_PORT);
  }
  delay(10000);
}
