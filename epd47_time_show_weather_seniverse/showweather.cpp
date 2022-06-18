#include "showweather.h"


weatherManager::weatherManager()
{
  //readbuff = (byte*)ps_malloc(10 * 1024);

  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer) {
    Serial.println("alloc memory failed !!!");
  }
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}




weatherManager::~weatherManager()
{

}


void weatherManager::ShowLine(int x0, int y0, int x1, int y1)
{
  epd_draw_line(x0, y0, x1, y1, 0, framebuffer);
}


void weatherManager::ShowRec(int x0, int y0, int x1, int y1)
{
  epd_draw_rect(x0, y0, abs(x1 - x0), abs(y1 - y0), 0, framebuffer);
}

String  weatherManager::calcWeekOfDay_string(String date_str)
{
  int week = calcWeekOfDay(date_str.substring(0, 4).toInt(), date_str.substring(5, 7).toInt(), date_str.substring(8, 10).toInt());
  return daysOfTheWeek[week];
}



uint8_t * weatherManager::get_icon(String no)
{

  uint8_t * ret_p = (uint8_t *)w0_data;
  if (no == "晴")   // your hand is on the sensor
    ret_p = (uint8_t *)w0_data;
  else if (no == "多云")
    ret_p = (uint8_t *) w1_data;
  else if (no == "阴")
    ret_p = (uint8_t *) w2_data;
  else if (no == "阵雨")
    ret_p = (uint8_t *) w3_data;
  else if (no == "雷阵雨")
    ret_p = (uint8_t *) w4_data;
  else if (no == "雷阵雨伴有冰雹")
    ret_p = (uint8_t *) w5_data;
  else if (no == "雨夹雪")
    ret_p = (uint8_t *) w6_data;
  else if (no == "小雨")
    ret_p = (uint8_t *) w7_data;
  else if (no == "中雨")
    ret_p = (uint8_t *) w8_data;
  else if (no == "大雨")
    ret_p = (uint8_t *) w9_data;
  else if (no == "暴雨")
    ret_p = (uint8_t *) w10_data;
  else if (no == "大暴雨")
    ret_p = (uint8_t *) w11_data;
  else if (no == "特大暴雨")
    ret_p = (uint8_t *) w12_data;
  else if (no == "阵雪")
    ret_p = (uint8_t *) w13_data;
  else if (no == "小雪")
    ret_p = (uint8_t *) w14_data;
  else if (no == "中雪")
    ret_p = (uint8_t *) w15_data;
  else if (no == "大雪")
    ret_p = (uint8_t *) w16_data;
  else if (no == "暴雪")
    ret_p = (uint8_t *) w17_data;
  else if (no == "雾")
    ret_p = (uint8_t *) w18_data;
  else if (no == "冻雨")
    ret_p = (uint8_t *) w19_data;
  else if (no == "沙尘暴")
    ret_p = (uint8_t *) w20_data;
  else if (no == "小雨-中雨")
    ret_p = (uint8_t *) w21_data;
  else if (no == "中雨-大雨")
    ret_p = (uint8_t *) w22_data;
  else if (no == "大雨-暴雨")
    ret_p = (uint8_t *) w23_data;
  else if (no == "暴雨-大暴雨")
    ret_p = (uint8_t *) w24_data;
  else if (no == "大暴雨-特大暴雨")
    ret_p = (uint8_t *) w25_data;
  else if (no == "小雪-中雪")
    ret_p = (uint8_t *) w26_data;
  else if (no == "中雪-大雪")
    ret_p = (uint8_t *) w27_data;
  else if (no == "大雪-暴雪")
    ret_p = (uint8_t *) w28_data;
  else if (no == "浮尘")
    ret_p = (uint8_t *) w29_data;
  else if (no == "扬沙")
    ret_p = (uint8_t *) w30_data;
  else if (no == "强沙尘暴")
    ret_p = (uint8_t *) w31_data;
  else if (no == "浓雾")
    ret_p = (uint8_t *) w32_data;
  else if (no == "台风")
    ret_p = (uint8_t *) w39_data;
  else if (no == "强浓雾")
    ret_p = (uint8_t *) w49_data;
  else if (no == "霾")
    ret_p = (uint8_t *) w53_data;
  else if (no == "中度霾")
    ret_p = (uint8_t *) w54_data;
  else if (no == "重度霾")
    ret_p = (uint8_t *) w55_data;
  else if (no == "严重霾")
    ret_p = (uint8_t *) w56_data;
  else if (no == "大雾")
    ret_p = (uint8_t *) w57_data;
  else if (no == "特强浓雾")
    ret_p = (uint8_t *) w58_data;
  else if (no == "雨")
    ret_p = (uint8_t *) w301_data;
  else if (no == "雪")
    ret_p = (uint8_t *) w302_data;
  else
    ret_p = (uint8_t *) w99_data;

  return ret_p;
}


/* Leap Year Judging 闰年检测 */
bool weatherManager::isLeapYear(int Years)
{
  bool isLeapYears;
  if (Years % 4 == 0)
  {
    if (Years % 100 == 0)
    {
      if (Years % 400 == 0)
      {
        //是400年一次的世纪闰年(如1600,2000年)
        return true;
      }
      else
      {
        //不是闰年(能被100整除)
        return false;
      }
    }
    else
    {
      //是4年一次的闰年(能被4整除且不能被100整除)
      return true;
    }
  }
  else
  {
    //不是闰年(不能被4整除)
    return false;
  }
  return false;
}

/* 计算星期几 */
int weatherManager::calcWeekOfDay(int Years, int Months, int Days)
{
  int toDaysDays = 0; //1900年1月1日是星期一,所以前一天是星期日

  //增加从1900年到今年的天数
  for (int i = 1900; i < Years; i++)
  {
    if (isLeapYear(i) == true)
    {
      //如果是闰年,366/7=52,366%7=2,所以哪一年会过52个星期多两天,所以来年的1月1日会比前一年晚两个工作日(休息日)
      //e.g. 1904年1月1日是星期五,1905年1月1日是星期天,多了两天
      toDaysDays += 2;
    }
    else
    {
      //如果是平年,365%7=1,所以多一天
      //e.g. 1901年1月1日是星期一,1902年1月1日是星期二,多了一天
      toDaysDays += 1;
    }
    toDaysDays %= 7;
  }

  //增加从今年1月到这个月的天数
  for (int i = 0; i < Months; i++)
  {
    if (i == 1 || i == 3 || i == 5 || i == 7 || i == 8 || i == 10 || i == 12)
    {
      //如果是大月,31%7=3,所以多3天
      toDaysDays += 3;
    }
    else if (i == 4 || i == 6 || i == 9 || i == 11)
    {
      //如果是小月,30%7=2,所以多2天
      toDaysDays += 2;
    }
    else if (isLeapYear(Years) == true  && i == 2)
    {
      //如果是二月,而且是闰年,29%7=1,所以多1天
      toDaysDays += 1;
    }
    else if (isLeapYear(Years) == false && i == 2)
    {
      //如果是二月,而且不是闰年,28%7=0,所以多0天,什么都不做
      ;
    }
    toDaysDays %= 7;
  }

  //增加从这个月1日到今天的天数,并且%7
  toDaysDays += Days;
  toDaysDays %= 7;

  return toDaysDays;
}


void weatherManager::draw_weather(String weather_result, String memo1,String memo2)
{

  if (weather_result.length() < 512 || weather_result.indexOf("{\"results\":") == -1)
  {
    Serial.println("无效的天气数据");
    return;
  }
  //约4-5kB
  /*
    StaticJsonBuffer<10 * 1024> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(weather_result);
  */

  Serial.println("deserializeJson");
  //字符长度约1020B上下，为保险，3KB，
  //必须用 DynamicJsonDocument
  DynamicJsonDocument  root(3 * 1024);
  deserializeJson(root, weather_result);

  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);


  ShowRec(10, 10, EPD_WIDTH - 20, EPD_HEIGHT - 8);

  ShowLine(10, EPD_HEIGHT / 5, EPD_WIDTH - 20, EPD_HEIGHT / 5);
  ShowLine(10, EPD_HEIGHT / 5 * 3, EPD_WIDTH - 20, EPD_HEIGHT / 5 * 3);
  ShowLine(10, EPD_HEIGHT - 70, EPD_WIDTH - 20,  EPD_HEIGHT - 70);

  ShowLine(EPD_WIDTH / 2, EPD_HEIGHT / 5, EPD_WIDTH / 2, EPD_HEIGHT - 70);
  ShowLine(EPD_WIDTH / 4, EPD_HEIGHT / 5 * 3, EPD_WIDTH / 4, EPD_HEIGHT - 70);
  ShowLine(EPD_WIDTH / 4 * 3, EPD_HEIGHT / 5 * 3, EPD_WIDTH / 4 * 3, EPD_HEIGHT - 70);


  uint8_t * icon_data;

  Rect_t area = {
    .x = EPD_WIDTH / 16,
    .y = EPD_HEIGHT / 5 + 120,
    .width = 70,
    .height = 60,
  };
  icon_data = get_icon(root["results"][0]["daily"][0]["text_day"].as<String>());
  epd_copy_to_framebuffer(area, icon_data, framebuffer);


  ShowStr(root["results"][0]["daily"][0]["date"].as<String>(), 20, 20, 150, framebuffer);
  ShowStr(calcWeekOfDay_string(root["results"][0]["daily"][0]["date"].as<String>()), 520, 20, 150, framebuffer);


  //   ShowPicture("pictures_weather/"+result["img"]+".png",EPD_WIDTH/16,EPD_HEIGHT/5+120);
  ShowStr(root["results"][0]["location"]["name"].as<String>() + " " + root["results"][0]["daily"][0]["text_day"].as<String>(), 20, EPD_HEIGHT / 5 + 20, 130, framebuffer);

  //ShowStr(root["result"]["temp"].as<String>() + "℃", EPD_WIDTH / 4 + 140, EPD_HEIGHT / 5 + 20, 160, framebuffer);

  ShowStr("最低" + root["results"][0]["daily"][0]["low"].as<String>() + "℃" + " " + "最高" + root["results"][0]["daily"][0]["high"].as<String>() + "℃", EPD_WIDTH / 4 - 40, EPD_HEIGHT / 5 * 2 - 20, 48, framebuffer);
  ShowStr("湿度:" + root["results"][0]["daily"][0]["humidity"].as<String>() + "%", EPD_WIDTH / 4 - 40, EPD_HEIGHT / 5 * 2 + 20, 48, framebuffer);

  ShowStr(root["results"][0]["daily"][0]["wind_direction"].as<String>() + "风", EPD_WIDTH / 2 + 20, EPD_HEIGHT / 5 + 20, 130, framebuffer);
  ShowStr(root["results"][0]["daily"][0]["wind_scale"].as<String>() + "级", EPD_WIDTH / 2 + 20,  EPD_HEIGHT / 5 + 110, 130, framebuffer);

  //空气质量
  //ShowStr("PM2.5:" + root["result"]["aqi"]["pm2_5"].as<String>(),   EPD_WIDTH / 2 + 200, EPD_HEIGHT / 5 * 2 - 20, 48, framebuffer);
  //ShowStr("空气质量:" + root["result"]["aqi"]["quality"].as<String>(), EPD_WIDTH / 2 + 200, EPD_HEIGHT / 5 * 2 + 20, 48, framebuffer);


  //连续2天天气...

  ShowStr(root["results"][0]["daily"][1]["date"].as<String>(), EPD_WIDTH / 32, EPD_HEIGHT / 5 * 3 - 15 , 50, framebuffer)   ;
  ShowStr(root["results"][0]["daily"][1]["wind_direction"].as<String>() + "风", EPD_WIDTH / 32 + 90, EPD_HEIGHT / 5 * 3 + 30, 64, framebuffer);
  ShowStr(root["results"][0]["daily"][1]["low"].as<String>() + "~" + root["results"][0]["daily"][1]["high"].as<String>() + "℃", EPD_WIDTH / 32 + 90, EPD_HEIGHT / 5 * 3 + 70, 64, framebuffer);

  area = {
    .x = EPD_WIDTH / 32,
    .y = EPD_HEIGHT / 5 * 3 + EPD_HEIGHT / 10,
    .width = 70,
    .height = 60,
  };
  icon_data = get_icon(root["results"][0]["daily"][1]["text_day"].as<String>());
  epd_copy_to_framebuffer(area, icon_data, framebuffer);

  ShowStr(root["results"][0]["daily"][2]["date"].as<String>(), EPD_WIDTH / 4 + EPD_WIDTH / 32, EPD_HEIGHT / 5 * 3 - 15 , 50, framebuffer)   ;
  ShowStr(root["results"][0]["daily"][2]["wind_direction"].as<String>() + "风", EPD_WIDTH / 4 + EPD_WIDTH / 32 + 90, EPD_HEIGHT / 5 * 3 + 30, 64, framebuffer);
  ShowStr(root["results"][0]["daily"][2]["low"].as<String>() + "~" + root["results"][0]["daily"][1]["high"].as<String>() + "℃", EPD_WIDTH / 4 + EPD_WIDTH / 32 + 90, EPD_HEIGHT / 5 * 3 + 70, 64, framebuffer);

  area = {
    .x = EPD_WIDTH / 4 + EPD_WIDTH / 32,
    .y = EPD_HEIGHT / 5 * 3 + EPD_HEIGHT / 10,
    .width = 70,
    .height = 60,
  };
  icon_data = get_icon(root["results"][0]["daily"][2]["text_day"].as<String>());
  epd_copy_to_framebuffer(area, icon_data, framebuffer);

  if (memo1.length() > 0)
    ShowStr(memo1 , EPD_WIDTH / 4 * 2 + EPD_WIDTH / 32 + 90 -80, EPD_HEIGHT / 5 * 3 + 30, 64, framebuffer);

  if (memo2.length() > 0)
    ShowStr(memo2 , EPD_WIDTH / 4 * 2 + EPD_WIDTH / 32 + 90 -80, EPD_HEIGHT / 5 * 3 + 70, 64, framebuffer);


  ShowStr("Last update:" + root["results"][0]["last_update"].as<String>(), 190, EPD_HEIGHT - 85, 60, framebuffer);

  epd_poweron();
  //uint32_t t1 = millis();
  //全局刷
  epd_clear();

  //前面不要用writeln,有一定机率阻塞,无法休眠
  epd_draw_grayscale_image(epd_full_screen(), framebuffer);

  //delay(500);
  epd_poweroff();

}
