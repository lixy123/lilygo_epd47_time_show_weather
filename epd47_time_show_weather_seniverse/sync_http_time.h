#include "HTTPClient.h"
//https://arduinojson.org/v6/doc/deserialization/?utm_source=github&utm_medium=readme
#include <ArduinoJson.h>


class sync_http_timeManager
{
  private:
    const char *http_host = "api.uukit.com";
    const char *host = "https://api.uukit.com"; 
    String req_url = "";
    HTTPClient http_client;
    WiFiClient client;   
   
  public:
   // String resp_new;  //精简后的json,便于蓝
    String resp;      //原始的json 
    String time_str;

    sync_http_timeManager();
    ~sync_http_timeManager();

    int getnow_time();
};
