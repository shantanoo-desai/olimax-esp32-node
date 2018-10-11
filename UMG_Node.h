/*
  UMG_Node.h - Library for providing InfluxDB Line Protocol Measurements and
               sending the information via HTTP or UDP
  Created by Shantanoo Desai <des@biba.uni-bremen.de>, October 09, 2018

  Code Adapted from H.W.Wong's ESP_influxdb Repository (MIT License)
*/

#ifndef UMG_NODE_H
#define UMG_NODE_H

#include "Arduino.h"

#if defined(ESP32)
  #include<ETH.h>
  #include<HTTPClient.h>
  #include<AsyncUDP.h>
#endif

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include<ESP8266HTTPClient.h>
#endif

/* Response from central InfluxDB instance */
enum HTTP_RESPONSE { INFLUX_SUCCESS, INFLUX_ERROR, INFLUX_CONNECT_FAILED };

String URLEncode(String msg);

/*
  influxMeasurement: Class to create InfluxDB Line Protocol for measurements
 */

class influxMeasurement
{
  public:
    influxMeasurement(String meas); /* Constructor */

    String measurement; // the complete line protocol measurement

    /* Public Methods */
    void empty(); /* Empty Measurement */
    void addField(String key, float value); /* Add Field Name, Value */
    void addTag(String key, String value); /*  Add Tag Name, Value */
    void addTimeStamp(uint32_t timestamp); /*  Add Timestamp in seconds to measurement */
    void addTimeStamp_ms(uint64_t ts_ms); /* Add timestamp in milliseconds precision to measurement */
    String postString(); /* final measurement to be sent out */

    ~influxMeasurement(); /* Destructor */

  private:

    String _data;
    String _tag;
    String _uint64tToString(uint64_t milliseconds_epoch_time);
};

class InfluxDB {
  public:
    InfluxDB(); /* Default Constructor */
    InfluxDB(const char* host, uint16_t port, const char* dbname); /* Parameterized Constructor */

    /* HTTP Based Information Exchange */
    HTTP_RESPONSE httpWrite(influxMeasurement measurements, String precision); // HTTP POST Wrapper for InfluxDB
    HTTP_RESPONSE httpResponse(); // Response from HTTP API

    /* UDP based Information Exchange*/
    size_t udpWrite(IPAddress host_add, uint16_t udp_port, influxMeasurement measurement);

    ~InfluxDB(); /* Destructor */

  private:
    String _port; // Host InfluxDB port
    String _host; // Host InfluxDB host
    String _dbname; // Host InfluxDB Database name
    HTTP_RESPONSE _write(String str_data, String ts_precision); // HTTP POST Wrapper for InfluxDB
    HTTP_RESPONSE _response = INFLUX_ERROR; // response from HTTP API of InfluxDB

};

#endif
