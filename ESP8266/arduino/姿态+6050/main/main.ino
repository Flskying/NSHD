#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Servo.h>

Adafruit_MPU6050 mpu;
Servo servoMotor;

// PID参数
double Kp = 1.0; // 比例系数
double Ki = 2.1; // 积分系数
double Kd = 1.3; // 微分系数

double setpoint = 0; // 目标倾斜角度
double prev_error = 0; // 上一个周期的误差
double integral = 0; // 误差积分

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // 等待串口连接
  }

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

  servoMotor.attach(2); // 将舵机连接到引脚 2
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // 计算倾斜角度（考虑 Y 轴方向的倾斜）
  double angle = atan2(-a.acceleration.y, a.acceleration.z) * 180 / PI;

  // 计算误差
  double error = setpoint - angle;

  // 计算误差积分
  integral += error;

  // 计算误差微分
  double derivative = error - prev_error;

  // 计算PID输出
  double output = Kp * error + Ki * integral + Kd * derivative;

  // 限制输出范围在舵机角度范围内
  output = constrain(output, -90, 90);

  // 控制舵机
  int servoPosition = map(output, -90, 90, 0, 180);

  servoMotor.write(servoPosition);

  // 更新上一个周期的误差
  prev_error = error;

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

  Serial.print(output);
  Serial.println("度");

  delay(100); // 延迟一段时间再次读取
}
