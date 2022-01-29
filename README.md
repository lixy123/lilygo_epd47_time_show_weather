<b>一.功能：</b> <br/>
1.lilygo 4.7寸墨水屏显示天气,每日刷新1-2次(程序内调整)   <br/> 
2.待机状态低能耗，没有更高精度测量仪，初估约0.5-2ma. 如用18650电池供电,预期1-2月 <br/>

   
<b>二.硬件</b>  <br/>
1. lilygo-epd47 4.7寸墨水屏 <br/>
2. 18650电池 <br/>
3. ds3231 时钟模块 <br/>
连线: <br/>
ESP32 ==>DS3231 <br/>
3.3V 3.3v <br/>
GND GND <br/>
14 SDA <br/>
15 SCL <br/>
 <br/>
注意： <br/>
lilygo-epd47 墨水屏顶部虽然有两个VCC,但休眠后此处vcc会断开给ds3231的供电，导致DS3231时钟会重置,无法用时钟模块 <br/>
需要用墨水屏给触摸屏供电接口位置的3.3V,GND <br/>
需要用到如下配件： <br/>
1.0.5mm 6p 同向 FPC <br/>
   https://item.taobao.com/item.htm?id=575345145498 <br/>
2. FPC转接板0.5转2.54 6P <br/>
   https://item.taobao.com/item.htm?id=574894948498 <br/>
通过以上配件，引出墨水屏给触摸屏供电的VCC,GND接到DS3231时钟模块 <br/>


<b>三.代码:</b><br/>
烧录到ESP32开发板<br/>
A.软件: arduino 1.8.13<br/>
B.用到库文件:<br/>
https://github.com/espressif/arduino-esp32 版本:1.0.6<br/>
https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library 最新版本, 用不到其代码，仅为了用到它预定义开发板<br/>
https://github.com/Xinyuan-LilyGO/LilyGo-EPD47 最新版本<br/>
https://github.com/bblanchon/ArduinoJson 版本: 6<br/>
ds3231时钟模块驱动 RTClib 最新版本<br/>
C.开发板选择：TTGO-T-WATCH 参数按默认<br/>
D.选择端口，点击烧录<br/>
注：<br/>
1.代码调整,config.h 文件需要修改 jisuapi_key变量 key 为极速天气key <br/>
注册获取KEY：https://www.jisuapi.com/api/weather/<br/>
2.首次运行时ESP32需要配置wifi连接，代码中用到了 ESP32 SmartConfig 配网技术,<br/>
参考:https://www.zhangtaidong.cn/archives/124/ 微信扫描配置wifi网络.否则会每120秒重启,直到正确配置好WIFI.<br/>

