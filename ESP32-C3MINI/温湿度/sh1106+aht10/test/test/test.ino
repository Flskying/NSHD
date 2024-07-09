/*
  U8g2Logo.ino 
*/
//aht10
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_AHTX0.h"

//#define SDA_PIN 8 // 定义SDA引脚
//#define SCL_PIN 9 // 定义SCL引脚
//const int SDA_PIN=D4;
//const int SCL_PIN=D3;
Adafruit_AHTX0 aht = Adafruit_AHTX0();

//屏幕
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup(void) {
  pinMode(9, OUTPUT);
  digitalWrite(9, 0); // default output in I2C mode for the SSD1306 test shield: set the i2c adr to 0
  u8g2.begin();
  u8g2.enableUTF8Print();
  //aht10
  Serial.begin(115200);  
}
String returnmsg="";
void drawLogo(void)
{
      sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp); // 获取湿度和温度事件
  if (isnan(humidity.relative_humidity) || isnan(temp.temperature)) {
  } else {
    returnmsg=String(humidity.relative_humidity)+"@"+String(temp.temperature);
    //Serial.print(returnmsg);
    Serial.print("湿度: ");
    Serial.print(humidity.relative_humidity);
    Serial.print("%\t 温度: ");
    Serial.print(temp.temperature);
    Serial.println("°C");
    
    
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);

    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.drawUTF8(5, 20, "TEM: ");  
    u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setCursor(50, 20);
    u8g2.print(temp.temperature);
    u8g2.drawUTF8(5, 40, "HUM: "); 
    u8g2.setCursor(50, 40);
    u8g2.print(humidity.relative_humidity);
    
  }
  delay(500); // 延迟500毫秒

  
 
//    u8g2.setFontDirection(1);
//    u8g2.setFont(u8g2_font_inb19_mn);
//    u8g2.drawStr(14,8,"8");
}

void loop(void) {
  
  u8g2.clearBuffer();
  drawLogo();
  u8g2.sendBuffer();
  delay(1000);
}
