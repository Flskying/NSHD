#include <Arduino.h>
#include <WiFi.h>
#include <U8g2lib.h>  // Assuming you have already included this library
#include "ntptime.h"
const char* ssid = "404! Network error";
const char* password = "ZQH%15034695001.";

const char* ntpServer = "ntp1.aliyun.com";
const long gmtOffset_sec = 8 * 3600;  // GMT+8:00, Beijing time
const int daylightOffset_sec = 0;
int time_ok = 0;
int timenum=1;
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  delay(100);
  // Initialize display
  u8g2.begin();
  u8g2.enableUTF8Print();
  // Connect to Wi-Fi u8g2
  //WIFI正在连接提示
  u8g2.clearBuffer();
  u8g2.setFontMode(1);  // Transparent
  u8g2.setFontDirection(0);
  // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(5, 20);
  u8g2.print("正在连接到");
  u8g2.setCursor(5, 40);
  u8g2.print(ssid);
  u8g2.sendBuffer();

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // u8g2.clearBuffer();
  //WIFI连接显示
  u8g2.setFontMode(1);  // Transparent
  u8g2.setFontDirection(0);
  // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(5, 60);
  u8g2.print("WiFi已连接");
  u8g2.sendBuffer();
  delay(1000);
  // Init time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void drawTime() {
  // Display current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    //获取时间报错
    u8g2.clearBuffer();
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);
    // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(25, 20);
    u8g2.print("未能获取时间");
    u8g2.setCursor(25, 40);
    u8g2.print("第");
    u8g2.print(timenum);
    u8g2.print("次尝试");
    timenum=timenum+1;
    u8g2.sendBuffer();
    delay(1500);
    return;
  }
  time_ok=time_ok+1;
  if(time_ok==1){
    //获取时间提示
    u8g2.clearBuffer();
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);
    // u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(25, 20);
    u8g2.print("更新时间成功");
    u8g2.setCursor(25, 40);
    u8g2.print("请稍后...");
    u8g2.sendBuffer();
    delay(500);
  }
  // //绘制时间
  // if (last_ntp == 0 || currentMillis - last_ntp >= interval_ntp) {
  //   last_ntp = currentMillis;
  //   loopNtp();
  //     u8g2.clearBuffer();
  //     u8g2.setFontMode(1);  // Transparent
  //     u8g2.setFontDirection(0);
  //     u8g2.setFont(u8g2_font_inb16_mn); 
  //     u8g2.setCursor(5, 20);
  //     u8g2.print(dt.localDate);

  //     u8g2.setFont(u8g2_font_inb16_mn); 
  //     u8g2.setCursor(5, 40);
  //     u8g2.print(dt.localTime);
  //     u8g2.sendBuffer();
}
  // char timeString[16]; // Buffer big enough for "HH:MM:SS AM/PM"
  // strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
  // u8g2.clearBuffer();
  // u8g2.setFontMode(1);  // Transparent
  // u8g2.setFontDirection(0);
  // u8g2.setFont(u8g2_font_inb16_mn);  // Example font for time display
  // u8g2.setCursor(5, 20);
  // u8g2.print(timeString);
// }
unsigned long last_ntp = 0;
const long interval_ntp = 1000; //时间的刷新频率（毫秒）
void loop() {
    //获取单片机启动至今的毫秒数
  unsigned long currentMillis = millis();
    //绘制时间
  if (last_ntp == 0 || currentMillis - last_ntp >= interval_ntp) {
    last_ntp = currentMillis;
    loopNtp();
      u8g2.clearBuffer();
      u8g2.setFontMode(1);  // Transparent
      u8g2.setFontDirection(0);
      u8g2.setFont(u8g2_font_wqy12_t_gb2312); 
      u8g2.setCursor(5, 20);
      u8g2.print(dt.localDate);

      u8g2.setFont(u8g2_font_wqy12_t_gb2312); 
      u8g2.setCursor(5, 40);
      u8g2.print(dt.localTime);
      u8g2.sendBuffer();
  }
  // drawTime();
  // delay(1000);  // Update every second
}
