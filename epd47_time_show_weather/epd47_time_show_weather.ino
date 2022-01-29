#include <Wire.h>
#include <WiFi.h>
#include "RTClib.h"
#include "weather_multiday.h"
#include "showweather.h"
#include "smartconfigManager.h"

#include "hz3500_36.h"
#include "hz3500_16.h"

/*

  开发板: TTGO-T-WATCH
  PARAM: Enabled

  编译大小: 4.1M (注：字库文件占用空间较大)

  重新编译后：preferences 的数据会保留，不会消失

  待机电流: 1-2ma
  
*/
smartconfigManager* objsmartconfigManager;
Weather_multidayManager * objWeather_multidayManager;
weatherManager * objweatherManager;


bool first_boot;
RTC_DS3231 rtc;

uint32_t stoptime, starttime;

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

int TIME_TO_SLEEP = 60; //下次唤醒间隔时间(秒）
const int short_time_segment = 60;  //休眠唤醒最小时间间隔 (单位：分钟)
String def_reflash_time = "07:00";  //注意：本参数与short_time_segment有相关性,是其整数倍,否则指定时间会不触发

hw_timer_t *timer = NULL;
int dog_timer = 15; //定时狗的分钟数,开机指定时间没有响应，自动重启

void IRAM_ATTR resetModule() {
  ets_printf("resetModule reboot\n");
  delay(100);
  //esp_restart_noos(); 旧api
  esp_restart();
}

void setup() {
  Serial.begin(115200);


  Serial.println("set timer dog:" + String(dog_timer) + "分钟");
  int wdtTimeout = dog_timer * 60 * 1000; //设n分钟 watchdog

  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000 , false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
  timerWrite(timer, 0); //reset timer (feed watchdog)

   //禁用wifi,节能
   WiFi.mode(WIFI_OFF);
   
  Wire.setPins(14, 15);

  starttime = millis() / 1000;

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  first_boot = false;


  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  //目前只能区分两种
  //0 上电/RST/调用esp_restart重启  无法区分
  //4 休眠定时唤醒 (深度或浅度)  
  if (wakeup_reason != ESP_SLEEP_WAKEUP_TIMER )
  {
    //非休眠唤醒标志
    first_boot = true;
    Serial.println("first_boot");
  }

  //如果启动后不调用此函数，有可能电流一直保持在在60ma，起不到节能效果
  //此步骤不适合在唤醒后没有显示需求时优化掉
  epd_init();

  Serial.println("start");
}

String Get_ds3231_time(int flag)
{
  DateTime now = rtc.now();
  char buf[50];

  if (flag == 0)
  {
    sprintf(buf, "%02d,%02d,%02d,%02d,%02d,%02d", now.year(), now.month() , now.day(), now.hour(), now.minute(), now.second());
  }
  if (flag == 1)
  {
    sprintf(buf, "%02d:%02d", now.hour(), now.minute());
  }
  return String(buf);
}

void ShowStr( String mystring, int x0, int y0, int font_size, uint8_t * framebuffer )
{
  y0 = y0 + 60;
  if (font_size >= 100)
    write_string( (GFXfont *)&msyh36, (char *) mystring.c_str(), &x0, &y0, framebuffer);
  else
    write_string( (GFXfont *)&msyh16, (char *) mystring.c_str(), &x0, &y0, framebuffer);
}

int show_weather()
{
  WiFi.mode(WIFI_STA); 
  objsmartconfigManager = new smartconfigManager();
  objsmartconfigManager->first_boot=first_boot;
  int http_code = -1;

  //如果当前为休眠唤醒状态，不会smartconfig，避免了电池无限消耗
  bool wifi_ok = objsmartconfigManager->connectwifi();
  if (wifi_ok == true)
  {
    objWeather_multidayManager = new Weather_multidayManager();

    http_code = objWeather_multidayManager->getnow_weather();
    if (http_code == HTTP_CODE_OK) //200
    {
      objweatherManager = new weatherManager();
      objweatherManager->ShowStr = ShowStr; //函数指针
      http_code = objweatherManager->draw_weather(objWeather_multidayManager->resp);

      if (http_code==0)
        http_code=HTTP_CODE_OK;
    }
    
  }
  WiFi.mode(WIFI_OFF);
  return http_code;
}

void loop() {
  String now_time = Get_ds3231_time(1);
  // Serial.println("def_reflash_time:" + def_reflash_time);
  // Serial.println("now_time:"+now_time);

  int http_code;
  //first_boot = false;

  //上电开机，休眠唤醒时间到了
  //注：初次上电或按下RST后会尝试显示天气，如果不成功会重启再次尝试
  if  (first_boot or def_reflash_time.indexOf(now_time) > -1)
  {
     http_code= show_weather();
     //正常显示天气 http_code==200
     Serial.println("show_weather http_code:" + String(http_code));
  }


  //计算本次需要休眠秒数
  DateTime now = rtc.now();
  Serial.println("计算休眠时间 hour=" + String(now.hour() ) + ",minute=" + String(now.minute() ) + ",second=" + String( now.second()));

  //如果short_time_segment是1，表示每1分钟整唤醒一次,定义的闹钟时间可随意
  //                       5，表示每5分钟整唤醒一次,这时定义的闹钟时间要是5的倍数，否则不会定时响铃
  TIME_TO_SLEEP = (short_time_segment - now.minute() % short_time_segment) * 60;
  TIME_TO_SLEEP = TIME_TO_SLEEP - now.second();
  //休眠时间过短，低于10秒直接视同0
  if (TIME_TO_SLEEP < 10)
    TIME_TO_SLEEP = 60 * short_time_segment;

  //注意：
  //esp32唤醒定时时间四啥五入及精确度问题，发现唤醒时间正好在59秒，
  //导致唤醒后的时间与预定义需要触发任务的时间错过，所以加10秒，确保不错过定义的时间
  TIME_TO_SLEEP = TIME_TO_SLEEP + 10;

  stoptime = millis() / 1000;

  //一般：0秒
  //获取天气: 10-36秒
  Serial.println("wake 用时:" + String(stoptime - starttime) + "秒");


  Serial.println("go sleep,wake after " + String(TIME_TO_SLEEP)  + "秒 AT:" + Get_ds3231_time(0));
  Serial.flush();

  //It will turn off the power of the entire
  // POWER_EN control and also turn off the blue LED light
  epd_poweroff_all();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // ESP进入deepSleep状态
  //最大时间间隔为 4,294,967,295 µs 约合71分钟
  //休眠后，GPIP的高，低状态将失效，无法用GPIO控制开关
  esp_deep_sleep_start();
}
