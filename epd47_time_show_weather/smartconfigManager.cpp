#include "smartconfigManager.h"

smartconfigManager::smartconfigManager()
{
  //初始化配置类
  preferences.begin("wifi-config");
  readparams();
}

smartconfigManager::~smartconfigManager()
{
  preferences.end();
}

void smartconfigManager::writeparams()
{
  Serial.println("Writing params to EEPROM... >" + ssid + " >" + password);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
}


void smartconfigManager::readparams()
{
  ssid = preferences.getString("ssid");
  password = preferences.getString("password");
  Serial.println("Read params >" + ssid + " >" + password);
}

//配网函数 限时try_num秒
//为防止无限等待，超时返回false
bool smartconfigManager::smartConfig(int try_num)
{
  int trytime = 0;
  Serial.println("\r\nWait for Smartconfig...");//串口打印
  WiFi.beginSmartConfig();//等待手机端发出的名称与密码
  //死循环，等待获取到wifi名称和密码
  while (1)
  {
    //等待,每秒计时一次
    Serial.print(".");
    delay(1000);
    trytime = trytime + 1;
    //超过计时，不再尝试
    if (trytime > try_num)
    {
      return false;
    }
    if (WiFi.smartConfigDone())//获取到之后退出等待
    {
      Serial.println("SmartConfig Success");
      //打印获取到的wifi名称和密码
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      //写入配置
      ssid = WiFi.SSID();
      password = WiFi.psk();
      writeparams();

      delay(2000);
      break;
    }
  }
  return true;
}


//如果上电后开始连接wifi，连接不上会进入smartConfig技术配网， 配网连接不上，重启esp32
//如果上电后有连接wifi成功过至少一次， 连接不上不会进入smartConfig技术配网，也不会重启esp32 (但是，连续超10次连续不成功连接会重启)
bool smartconfigManager::connectwifi()
{
  if (WiFi.status() == WL_CONNECTED)
    return true;
  WiFi.disconnect();
  delay(200);

  WiFi.mode(WIFI_STA);
  Serial.println("Connecting to WIFI");

  bool connect_ok = false;
  if (ssid.length() > 0)
  {
    int lasttime = millis() / 1000;
    WiFi.begin(ssid.c_str(), password.c_str());

    while ((!(WiFi.status() == WL_CONNECTED)))
    {
      delay(1000);
      Serial.print(".");

      //15秒连接不上，转入手机进行配网模式120秒
      if ( abs(millis() / 1000 - lasttime ) > 15)
      {
        if (first_boot)
        {
          Serial.println("WiFi连接失败，请用手机进行配网");
          connect_ok = smartConfig(120); //smartConfig技术配网,限时120秒
        }
        break;
      }
    }

    if (connect_ok == false)
    {
      if (WiFi.status() == WL_CONNECTED)
        connect_ok = true;
    }
  }
  else
  {
    if (first_boot)
    {
      Serial.println("WiFi未配置，请用手机进行配网");
      connect_ok = smartConfig(120); //smartConfig技术配网,限时120秒
    }
  }

  if (connect_ok)
  {
    delay(5000); //很重要，待生效！
    Serial.println("Connected");
    Serial.println("My Local IP is : ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    if (first_boot)
    {
      Serial.println("smartConfig配网失败，重启！");
      Serial.flush();      
      esp_restart();
    }
    else
      Serial.println("wifi 连接失败...");
  }

  return connect_ok;
}
