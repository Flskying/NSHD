#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//连接wifi用的库
#include <ESP8266WiFi.h>

//屏幕分辨率
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**
   你的wifi 账号和密码
   注意：切勿连接 5G的频率、不要使用双频融合（路由器）
   注意：账号密码必须完全正确，包括字母大小写、空格、中划线、下划线
*/
const char* ssid     = "LGCWZS";
const char* password = "87129168";

#include "wifi.h"
#include "ntptime.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  Serial.println("");
  //连接WI-FI
  initWiFi();
  //请求网络时间
  initNtp();

  Serial.println("OLED FeatherWing test2");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  Serial.println("OLED begun2");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
   display.display();
   delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();
}

unsigned long last_ntp = 0;
const long interval_ntp = 1000; //时间的刷新频率（毫秒）

void loop()
{
  //获取单片机启动至今的毫秒数
  unsigned long currentMillis = millis();
  //update ntp 时间
  if (last_ntp == 0 || currentMillis - last_ntp >= interval_ntp) {
    last_ntp = currentMillis;
    loopNtp();

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(5,10);
    display.print(dt.localDate);   

    display.setCursor(18,40);
    display.print(dt.localTime);
    display.display(); // actually display all of the above
  }
}
