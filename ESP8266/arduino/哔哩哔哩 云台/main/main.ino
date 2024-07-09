#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <Stepper.h>
#include <Ticker.h>

Adafruit_MPU6050 mpu;

float timeChange = 200; // 滤波法采样时间间隔毫秒
float dt = timeChange * 0.001; // 注意：dt的取值为滤波器采样时间
float K1 = 0.05; // 对加速度计取值的权重
float angle1; // 一阶滤波角度输出 （角度为舵机的角度位置）
float angleax;

const float accLBS = 16834; // 加速度计的灵敏度
const float groLBS = 131;   // 加速度计的灵敏度
float ax, ay, az, gx, gy, gz; // 定义传感器测量出的实际加速度和角速度

// PID
float value;
float a; // PID计算
Stepper myStepper(4096, 8, 10, 9, 11); // 初始化Stepper，参数根据你的电机步进角度设置

struct look {
    float InputNum;   // 输入值
    float PointNum;   // 指定值
    float LastError;  // 上一次计算误差(微分需要)
    float NowError;   // 当前计算误差
    float Integral;   // 积分值
    float Kp;         // 初始化比例系数
    float Ki;         // 初始化积分系数
    float Kd;         // 初始化微分系数
    float Output;     // 输出的变化量大小
} PID; // 定义一个PID object

void initial() {
    PID.InputNum = 0;
    PID.PointNum = 90;
    PID.LastError = 0;
    PID.NowError = 0;
    PID.Integral = 0;
    PID.Kp = 2;
    PID.Ki = 0;
    PID.Kd = 0.2;
    PID.Output = 0;
} // 初始化PID的参数

float ComputePID() {
    PID.NowError = PID.PointNum - PID.InputNum;
    // 累计误差，积分值
    PID.Integral += PID.NowError;
    // PID计算
    PID.Output = PID.Kp * PID.NowError + PID.Ki * PID.Integral + PID.Kd * gy;
    PID.LastError = PID.NowError;
    PID.InputNum += PID.Output;
    value = PID.Output + PID.InputNum; // 电机输出
    return value;
} // 计算PID数值

/*******************************************************/
// 一阶滤波
void Yijielvbo(float angle_m, float gyro_m) {
    angle1 = K1 * angle_m + (1 - K1) * (angle1 + gyro_m * dt); // 得到一个滤波后的值
}

void getangle() {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);
    ax = accel.acceleration.x / accLBS;
    ay = accel.acceleration.y / accLBS;
    az = accel.acceleration.z / accLBS;
    gx = gyro.gyro.x / groLBS;
    gy = gyro.gyro.y / groLBS;
    gz = gyro.gyro.z / groLBS;
    angleax = atan2(ax, az) * 57.3;
    Yijielvbo(angleax, -gy); // 此时会返回一个滤波以后的值angle1，将其作为PID的输入
}

void setup() {
    Serial.begin(115200); // 串口初始化
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }
    initial(); // PID初始化
    myStepper.setSpeed(20);
    // 设置定时器，每隔timeChange毫秒执行getangle函数
    Ticker timer(getangle, timeChange);
}

void loop() {
    PID.InputNum = angle1;
    myStepper.step(a);
    delay(500);
}
