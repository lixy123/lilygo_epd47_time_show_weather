#include "sync_http_time.h"


sync_http_timeManager::sync_http_timeManager() {
  req_url = (String)host + "/time";
  resp = "";
  //resp_new = "";
}

sync_http_timeManager::~sync_http_timeManager() {
  //client.stop();
  //WiFi.disconnect();
}


//获取原始url的json，并精简
//注:原始json约5k,精简成1K,lbe蓝牙传输约需30秒.
//申请变量时有富余,加倍
int sync_http_timeManager::getnow_time()
{

  uint32_t start_time = millis() / 1000;
  int http_code = -1;
  Serial.println(req_url);
  http_client.begin(req_url);
  http_code = http_client.GET();

  Serial.println(http_code);
  resp = "";
  time_str="";
  // resp_new = "";
  if (http_code == HTTP_CODE_OK)
  {
    resp = http_client.getString();
    Serial.println(resp);

    if (resp.indexOf("\"status\":1") > -1)
    {
    DynamicJsonDocument  root(1 * 1024);
    deserializeJson(root, resp);
    time_str= root["data"]["gmt"].as<String>();
    }
  }
  return http_code;
}
