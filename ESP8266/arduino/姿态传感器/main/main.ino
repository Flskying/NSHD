#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // 等待串口连接
  }
  
//  TwoWire::begin(4, 5); // 初始化I2C总线，参数为SDA引脚和SCL引脚
  
  // 尝试初始化传感器
  if (!mpu.begin()) {
    Serial.println("未找到 MPU6050 传感器，请检查连接！");
    while (1);
  }
  
  // 设置传感器范围，你可以根据需要选择
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  
  // 等待一段时间使传感器稳定
  delay(100);
}

void loop() {
  // 读取加速度和陀螺仪数据
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // 将数据打印到串口监视器
  Serial.print("加速度: ");
  Serial.print(a.acceleration.x);
  Serial.print(", ");
  Serial.print(a.acceleration.y);
  Serial.print(", ");
  Serial.print(a.acceleration.z);
  Serial.print(" m/s^2\t");

  Serial.print("陀螺仪: ");
  Serial.print(g.gyro.x);
  Serial.print(", ");
  Serial.print(g.gyro.y);
  Serial.print(", ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
  
  delay(100); // 延迟一段时间再次读取
}
