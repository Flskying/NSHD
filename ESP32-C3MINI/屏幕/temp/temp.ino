/*
  U8g2Logo.ino 
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

void setup(void) {
  pinMode(9, OUTPUT);
  digitalWrite(9, 0); // default output in I2C mode for the SSD1306 test shield: set the i2c adr to 0
  u8g2.begin();
  u8g2.enableUTF8Print();
}
void drawLogo(void)
{
    u8g2.setFontMode(1);  // Transparent
    u8g2.setFontDirection(0);

    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.drawUTF8(5, 20, "TEM: ");  
    u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setCursor(50, 20);
    u8g2.print("test");
    u8g2.drawUTF8(5, 40, "HUM: ");  
    u8g2.setCursor(50, 40);
    u8g2.print("test");
}

void loop(void) {
  u8g2.clearBuffer();
  drawLogo();
  u8g2.sendBuffer();
  delay(1000);
}
