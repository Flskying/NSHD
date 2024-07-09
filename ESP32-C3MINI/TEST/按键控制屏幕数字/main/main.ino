/*
  U8g2Logo.ino 
*/
const int vol1 = 3;  
const int vol2 = 4;  
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
int num=0;
void setup(void) {
  pinMode(vol1, INPUT);
  pinMode(vol2, INPUT);
  digitalWrite(9, 0); // default output in I2C mode for the SSD1306 test shield: set the i2c adr to 0
  u8g2.begin();
  u8g2.enableUTF8Print();
}
void drawLogo(void)
{
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);

    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.drawUTF8(5, 20, "hour: ");  
    u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setCursor(50, 20);
    u8g2.print(num);
    u8g2.drawUTF8(5, 40, "minite: ");  
    u8g2.setCursor(50, 40);
    u8g2.print(num);
}

void loop(void) {
  if (!digitalRead(vol1)) {   //判断键值等于0，执行下面的程序

    num=num+1;
    delay(200);//延时消抖
  }
  u8g2.clearBuffer();
  drawLogo();
  u8g2.sendBuffer();
  delay(1000);
}
