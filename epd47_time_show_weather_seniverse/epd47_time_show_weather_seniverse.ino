#include <WiFi.h>
#include "RTClib.h"
#include "sync_http_weather.h"
#include "showweather.h"
#include "smartconfigManager.h"
#include "sync_http_time.h"

#include "hz3500_36.h"
#include "hz3500_16.h"

char daysOfTheWeek[7][12] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

/*

  开发板: TTGO-T-WATCH
  PARAM: Enabled

  编译大小: 4.1M (注：字库文件占用空间较大)

  重新编译后：preferences 的数据会保留，不会消失

  待机电流: 1-2ma

  连接wifi用到了 smartconfig技术，手机安装

*/
smartconfigManager* objsmartconfigManager;
Weather_multidayManager * objWeather_multidayManager;
weatherManager * objweatherManager;
sync_http_timeManager * objsync_http_timeManager;

bool state_sync_time = false;
bool state_sync_weather = false;

bool first_boot;
RTC_Millis rtc;

uint32_t stoptime, starttime;

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
uint32_t TIME_TO_SLEEP = 60; //下次唤醒间隔时间(秒）

hw_timer_t *timer = NULL;
int dog_timer = 15; //定时狗的分钟数,开机指定时间没有响应，自动重启

void IRAM_ATTR resetModule() {
  ets_printf("resetModule reboot\n");
  delay(100);
  //esp_restart_noos(); 旧api
  esp_restart();
}


String Get_softrtc_time(int flag)
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
  else if (flag == 2)
  {
    sprintf(buf, "%02d,%02d,%02d,%02d,%02d", now.year(), now.month() , now.day(), now.hour(), now.minute());

  }
  else if (flag == 3)
  {
    sprintf(buf, "%02d月%02d日%s",  now.month() , now.day(), daysOfTheWeek[now.dayOfTheWeek()]);
  }
  else if (flag == 4)
  {
    sprintf(buf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  }
  else if (flag == 5)
  {
    sprintf(buf, "%02d-%02d-%02d %02d:%02d", now.year(), now.month() , now.day(), now.hour(), now.minute());

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



//计算总休眠秒数 到TIME_TO_SLEEP
void cal_waker_seconds()
{
  //如果前次有成功同步时间
  if (state_sync_time)
  {
    DateTime now = rtc.now();
    String now_time = Get_softrtc_time(1);
    Serial.println("now_time:" + now_time);

    //计算本次需要休眠秒数

    Serial.println("计算休眠时间 hour=" + String(now.hour() ) + ",minute=" + String(now.minute() ) + ",second=" + String( now.second()));

    //先计算到 00:00的秒数
    TIME_TO_SLEEP =   ((24 - now.hour()) * 60 -  now.minute() ) * 60;
    TIME_TO_SLEEP = TIME_TO_SLEEP - now.second();

    if (TIME_TO_SLEEP < 30)
      TIME_TO_SLEEP = 24 * 60 * 60 + 50;

    //再加上到 07:00的秒数
    TIME_TO_SLEEP = TIME_TO_SLEEP + 7 * 3600;

    //24小时唤醒平均会少12几分钟，所以用20分钟当误差
    //TIME_TO_SLEEP = TIME_TO_SLEEP + 1200 ;
    Serial.println("go sleep,wake after " + String(TIME_TO_SLEEP)  + "秒 AT:" + Get_softrtc_time(0));

  }
  //时间未同步，时间无效，2小时后再试
  else
  {
    TIME_TO_SLEEP = 3600 * 2 + 10;
    Serial.println("时间未同步， go sleep,wake after " + String(TIME_TO_SLEEP)  + "秒 ");
  }
}



//每天1次,早7点整
void goto_sleep()
{
  Serial.println("goto sleep");

  //计算休眠秒数
  cal_waker_seconds();

  stoptime = millis() / 1000;

  //平均35-80秒不等，nbiot同步时间需要时间较长
  Serial.println("wake 用时:" + String(stoptime - starttime) + "秒");

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


void sync_softrtc(String time_str)
{
  Serial.println("time_str len=" + String(time_str.length()));
  Serial.println(time_str);
  //2022-06-11 03:24:56
  if (time_str.length() == 19)
  {
    DateTime now = DateTime(time_str.substring(0, 4).toInt(), time_str.substring(5, 7).toInt(), time_str.substring(8, 10).toInt(),
                            time_str.substring(11, 13).toInt(), time_str.substring(14, 16).toInt(), time_str.substring(17, 19).toInt());

    // calculate a date which is 7 days and 30 seconds into the future
    //增加8小时
    DateTime future (now.unixtime() + 28800L);

    rtc.adjust(future);
    Serial.println("时间已同步:" + Get_softrtc_time(0));
  }
}

//同步时间，天气
void sync_time_weather()
{
  WiFi.mode(WIFI_STA);
  objsmartconfigManager = new smartconfigManager();
  //本参数为true时，能进入配置wifi状态, 但会造成电池耗电!
  objsmartconfigManager->first_boot = first_boot;
  int http_code = -1;

  //如果当前为休眠唤醒状态，不会smartconfig，避免了电池无限消耗
  bool wifi_ok = objsmartconfigManager->connectwifi();
  if (wifi_ok == true)
  {
    int http_code;

    //1.同步时间
    Serial.println(">>> 同步时间");
    objsync_http_timeManager = new sync_http_timeManager();
    http_code = objsync_http_timeManager->getnow_time();

    if (http_code == HTTP_CODE_OK && objsync_http_timeManager->time_str.length() == 19) //HTTP_CODE_OK:200
    {
      state_sync_time = true;
      sync_softrtc(objsync_http_timeManager->time_str);
    }

    //同步天气
    Serial.println(">>> 同步天气");
    objWeather_multidayManager = new Weather_multidayManager();

    http_code = objWeather_multidayManager->getnow_weather();
    Serial.println(objWeather_multidayManager->resp.length());
    if (http_code == HTTP_CODE_OK && objWeather_multidayManager->resp.length() > 500 ) //HTTP_CODE_OK:200
    {
      Serial.println("显示天气");
      objweatherManager = new weatherManager();
      objweatherManager->ShowStr = ShowStr; //函数指针

      cal_waker_seconds();
      stoptime = millis() / 1000;
      String memo1 = "waker after " + String(TIME_TO_SLEEP) + "秒";
      String memo2 = Get_softrtc_time(2) +  " - " + String(stoptime - starttime) + "秒";
      objweatherManager->draw_weather(objWeather_multidayManager->resp, memo1, memo2 );
      state_sync_weather = true;
    }
  }
  WiFi.mode(WIFI_OFF);
}


void setup() {
  starttime = millis() / 1000;
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

  DateTime now = DateTime(2000, 1, 1,  0, 0, 0);
  rtc.adjust(now);

  first_boot = false;

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  //目前只能区分两种
  //0 上电/RST/调用esp_restart重启  无法区分
  //4 休眠定时唤醒 (深度或浅度)

  //注意：epd47墨水屏电池供电时，永远返回0, 不会返回ESP_SLEEP_WAKEUP_TIMER
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

  //同步时间，天气
  sync_time_weather();

  //进入休眠
  goto_sleep();
}


void loop() {

}
