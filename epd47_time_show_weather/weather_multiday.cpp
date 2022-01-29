#include "weather_multiday.h"

/*
  https://arduinojson.org/v6/how-to/use-external-ram-on-esp32/  如何调用 arduinojson库时使用psram
  //注意：为了解析天气json,
  struct SpiRamAllocator {
  void* allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
  };

  using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;
*/

Weather_multidayManager::Weather_multidayManager() {
  req_url = (String)host + "/weather/query?appkey=";
  req_url += jisuapi_key;
  req_url += "&cityid=";
  req_url += city;
  resp = "";
  //resp_new = "";
}

Weather_multidayManager::~Weather_multidayManager() {
  //client.stop();
  //WiFi.disconnect();
}


//获取原始url的json，并精简
//注:原始json约5k,精简成1K,lbe蓝牙传输约需30秒.
//申请变量时有富余,加倍
int Weather_multidayManager::getnow_weather()
{

  uint32_t start_time = millis() / 1000;
  int http_code = -1;
  Serial.println(req_url);
  http_client.begin(req_url);
  http_code = http_client.GET();

  Serial.println(http_code);
  resp = "";
  // resp_new = "";
  if (http_code == HTTP_CODE_OK)
  {
    resp = http_client.getString();
    //Serial.println(resp);
  }
  return http_code;
}
