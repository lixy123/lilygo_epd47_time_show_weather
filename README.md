<b>一.功能：</b> <br/>
1.lilygo 4.7寸墨水屏每日1次通过互联网获取并显示天气, <br/> 
2.待机状态低能耗约0.24ma.  每天唤醒1次，电流约70ma, 一般16秒后进入休眠待机状态， 18650电池供电理论预期应在3-5个月<br/>
   
<b>二.硬件</b>  <br/>
1.lilygo-epd47 4.7寸墨水屏 + 18650电池 <br/>
<img src= 'https://github.com//lilygo_epd47_time_show_weather/blob/main/1.jpg?raw=true' /> <br/>
<img src= 'https://github.com//lilygo_epd47_time_show_weather/blob/main/2.jpg?raw=true' /> <br/>

<b>三.代码:</b><br/>
烧录到ESP32开发板<br/>
A.软件: arduino 1.8.19<br/>
B.用到库文件:<br/>
https://github.com/espressif/arduino-esp32 版本:1.0.6<br/>
https://github.com/bblanchon/ArduinoJson 版本: 6<br/>
https://github.com/adafruit/RTClib RTClib <br/>
C.开发板选择：TTGO-T-WATCH 参数选默认 (字库文件较大，仅用到其分区定义)<br/>
D.选择端口，点击烧录<br/>
注：<br/>
1.首次运行时ESP32需要配置wifi连接，代码中用到了 ESP32 SmartConfig 配网技术,<br/>
参考:https://www.zhangtaidong.cn/archives/124/ 微信扫描配置wifi网络.否则会每120秒重启,直到正确配置好WIFI.<br/>

