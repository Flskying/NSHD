#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_AHTX0.h"

#define SDA_PIN D4 // 定义SDA引脚
#define SCL_PIN D3 // 定义SCL引脚
Adafruit_AHTX0 aht = Adafruit_AHTX0();
void setup() {
  Serial.begin(9600);
  Serial.println("1");


  Wire.begin(SDA_PIN, SCL_PIN); // 初始化I2C总线
  Serial.println("2");
  aht.begin();
  // if (!aht.begin()) {
  //   Serial.println("3");
  //   while (1);
  // }
  Serial.println("4");
}
String returnmsg="";
void loop() {
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
  }
  delay(500); // 延迟500毫秒
}
