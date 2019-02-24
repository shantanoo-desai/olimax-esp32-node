/* ESP32-PoE Board Configuration for Ethernet */
#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12
#define I2C_SDA 13
#define I2C_SCL 16


/* IP CONF for UMG NODE */
///////// >>>> MODIFY IP Configuration Here <<<< ////////
const IPAddress node_static_ip(192, 168, 3, 140);
const IPAddress default_gateway(192, 168, 3, 250);
const IPAddress subnet_mask(255, 255, 255, 0);
const IPAddress dns_add(192, 168, 3, 250);

/* InfluxDB CONF for UMG Node */
const IPAddress INFLUX_IP(192, 168, 3, 11);
const uint16_t INFLUX_PORT = 8089;


/* UMG meta data InfluxDB Tags */
const String measurement_name = "imu";
///////////////// >>>>>> MODIFY Values Here <<< ///
const String node_name = "UMGNODE2";
const String  node_placement = "crewChair";
