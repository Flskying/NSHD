/*
esp32-c3MINI点亮sh1106屏幕
*/

#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
void setup(void)
{  
  u8x8.begin();
  u8x8.setPowerSave(0);
  
}
void loop(void)
{
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,1,"Hello World!");
  u8x8.setInverseFont(1);
  u8x8.drawString(0,0,"012345678901234567890123456789");
  u8x8.setInverseFont(0);
  //u8x8.drawString(0,8,"Line 8");
  //u8x8.drawString(0,9,"Line 9");
  u8x8.refreshDisplay();    // only required for SSD1606/7  
  delay(2000);
}
