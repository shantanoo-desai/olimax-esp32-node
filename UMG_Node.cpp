#include "UMG_Node.h"
#include "Arduino.h"


/**
 *   influxMeasurement Class Methods
 */

/* Constructor */
influxMeasurement::influxMeasurement(String meas)
{
  measurement = meas;
}

void influxMeasurement::empty() {
  _data = "";
  _tag = "";
}

void influxMeasurement::addField(String key, float value)
{
  _data = (_data == "") ? (" ") : (_data += ",");
  _data += key + "=" + String(value);
}

void influxMeasurement::addTag(String key, String value)
{
  _tag += "," + key + "=" + value;
}

void influxMeasurement::addTimeStamp(uint32_t ts)
{
  if (ts > 1) { // check if timestamp is a valid one
    _data += " " + String(ts);
  }
}

void influxMeasurement::addTimeStamp_ms(uint64_t ts_ms)
{
  // convert uint64t to String
  if (ts_ms > 1) {
      String _result = _uint64tToString(ts_ms);
      _data += " " + _result;
  }
}

String influxMeasurement::postString()
{
  return measurement + _tag + _data;
}

String URLEncode(String msg)
{
  const char *hex = "0123456789abcdef";
        String encodedMsg = "";

        uint16_t i;
        for (i = 0; i < msg.length(); i++) {
                if (('a' <= msg.charAt(i) && msg.charAt(i) <= 'z') ||
                    ('A' <= msg.charAt(i) && msg.charAt(i) <= 'Z') ||
                    ('0' <= msg.charAt(i) && msg.charAt(i) <= '9')) {
                        encodedMsg += msg.charAt(i);
                } else {
                        encodedMsg += '%';
                        encodedMsg += hex[msg.charAt(i) >> 4];
                        encodedMsg += hex[msg.charAt(i) & 15];
                }
        }
  return encodedMsg;
}

String influxMeasurement::_uint64tToString(uint64_t value)
{
  String result = "";
  uint8_t base = 10;

  do {
    char c = value % base;
    value /= base;

    if (c < 10)
      c +='0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (value);
  return result;
}

influxMeasurement::~influxMeasurement()
{

}

/**
 * InfluxDB Class Methods
 */

InfluxDB::InfluxDB()
{

}

InfluxDB::InfluxDB(const char* host, uint16_t port, const char* dbname)
{
 _port = String(port);
 _host = String(host);
 _dbname = String(dbname);
}

HTTP_RESPONSE InfluxDB::httpWrite(influxMeasurement mes_data, String timestamp_precision)
{
  return _write(mes_data.postString(), timestamp_precision);
}

HTTP_RESPONSE InfluxDB::httpResponse()
{
  return _response;
}

size_t InfluxDB::udpWrite(IPAddress host_add, uint16_t udp_port, influxMeasurement measurement)
{
  AsyncUDP udp;
  // shoot and forget UDP approach
  udp.connect(host_add, udp_port);
  udp.print(measurement.postString().c_str());
  udp.close();
}

HTTP_RESPONSE InfluxDB::_write(String mes_dat, String ts_precision)
{
  HTTPClient http;

  if (ts_precision == "") {
    // send without precision
    http.begin("http://" + _host + ":" + _port + "/write?db=" + _dbname);
  }
  else if (ts_precision == "s" || ts_precision == "ms" || ts_precision == "u" || ts_precision == "ns") {
    // send with defined precision
    http.begin("http://" + _host + ":" + _port + "/write?db=" + _dbname + "&precision=" + ts_precision);
  }
  else {
    // send with default precision of nanoseconds
    ts_precision = "ns";
    http.begin("http://" + _host + ":" + _port + "/write?db=" + _dbname + "&precision=" + ts_precision);
  }
  http.addHeader("Content-Type", "text/plain");
  int httpResponseCode = http.POST(mes_dat);
  if (httpResponseCode == 204) {
    _response = INFLUX_SUCCESS;
  } else {
    _response = INFLUX_ERROR;
  }

  http.end();
  return _response;
}

InfluxDB::~InfluxDB()
{

}
