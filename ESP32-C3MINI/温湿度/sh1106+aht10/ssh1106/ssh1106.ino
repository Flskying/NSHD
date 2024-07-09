#include <Wire.h>

#define AHT10_ADDR 0x38 // AHT10 默认的 I2C 地址

void setup() {
  Serial.begin(115200); // 初始化串口通信
  Wire.begin(); // 初始化 I2C 总线
}

void loop() {
  // 发送读取温湿度命令
  Wire.beginTransmission(AHT10_ADDR);
  Wire.write(0xE1); // 发送读取数据的命令
  Wire.endTransmission();
  
  // 等待传感器完成测量
  delay(75); // AHT10 最小等待时间为 75 毫秒
  
  // 从传感器读取数据
  Wire.requestFrom(AHT10_ADDR, 6); // 读取 6 个字节的数据
  if (Wire.available() == 6) {
    uint8_t rawData[6];
    for (int i = 0; i < 6; i++) {
      rawData[i] = Wire.read();
    }
    
    // 解析数据
    uint32_t rawHumidity = ((uint32_t)rawData[1] << 12) | ((uint32_t)rawData[2] << 4) | ((uint32_t)rawData[3] >> 4);
    uint32_t rawTemperature = ((uint32_t)(rawData[3] & 0x0F) << 16) | ((uint32_t)rawData[4] << 8) | rawData[5];
    
    // 计算实际的温度和湿度值
    float humidity = rawHumidity * 100.0 / 0x100000;
    float temperature = (rawTemperature * 200.0 / 0x100000) - 50.0;
    
    // 输出到串口
    Serial.print("湿度: ");
    Serial.print(humidity);
    Serial.print("%\t 温度: ");
    Serial.print(temperature);
    Serial.println("°C");
  }
  
  // 等待一段时间再次读取
  delay(2000); // 间隔 2 秒
}
