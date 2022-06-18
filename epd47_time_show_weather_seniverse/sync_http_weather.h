#include "HTTPClient.h"

//https://arduinojson.org/v6/doc/deserialization/?utm_source=github&utm_medium=readme
//#include <ArduinoJson.h>
#include "config.h"


class Weather_multidayManager
{
  private:
    const char *http_host = "api.seniverse.com";
    const char *host = "https://api.seniverse.com"; 
    const char *param = "/v3/weather/daily.json?key=urfcfaxdk4zxr3z0&location=beijing&language=zh-Hans&unit=c&start=0&days=3";                  
    String req_url = "";
    HTTPClient http_client;
    WiFiClient client;

   
  public:
   // String resp_new;  //精简后的json,便于蓝
    String resp;      //原始的json 

   // DynamicJsonDocument  root(15 * 1024);
     
    Weather_multidayManager();
    ~Weather_multidayManager();

    int getnow_weather();
};
