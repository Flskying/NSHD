#include "SevSeg.h"
/*
  数码管与屏幕同时计时
*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

SevSeg sevseg;  // 创建数码管对象
byte set = COMMON_ANODE;  // 如果是共阳极数码管则为COMMON_ANODE
byte number = 4;  // 数码管位数
byte COM[] = {0, 21, 20, 7};  // 数码管的COM引脚
byte pins[] = {1, 10, 5, 4, 3, 2, 6, 9};  // 数码管的段选引脚

unsigned long startTime = 0;  // 计时开始时间
unsigned long elapsedTime = 0;  // 经过的时间
int returnmsg=1;
void setup() {
  sevseg.begin(set, number, COM, pins);  // 初始化数码管
  bool disableDecPoint = true;  // 禁用小数点
  startTime = millis();  // 初始化开始时间为当前时间
    pinMode(9, OUTPUT);
  digitalWrite(9, 0); // default output in I2C mode for the SSD1306 test shield: set the i2c adr to 0
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void loop() {
  elapsedTime = millis() - startTime;  // 计算经过的时间
  
  // 将毫秒转换为秒
  int seconds = elapsedTime / 1000;

  // 显示在数码管上
  sevseg.setNumber(seconds, 0);  // 设置秒数
  sevseg.refreshDisplay();  // 刷新显示
  u8g2.clearBuffer();
  drawLogo();
  u8g2.sendBuffer();
  // delay(1000);
}
void drawLogo(void)
{
    elapsedTime = millis() - startTime;  // 计算经过的时间
  
  // 将毫秒转换为秒
  int seconds = elapsedTime / 1000;
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.drawUTF8(5, 20, "TEM: ");  
    u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setCursor(50, 20);
    u8g2.print(seconds);
    u8g2.drawUTF8(5, 40, "HUM: ");  
    // returnmsg=returnmsg+1;
//    u8g2.setFontDirection(1);
//    u8g2.setFont(u8g2_font_inb19_mn);
//    u8g2.drawStr(14,8,"8");
}
