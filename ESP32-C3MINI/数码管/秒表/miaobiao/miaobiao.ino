#include "SevSeg.h"

SevSeg sevseg;  // 创建数码管对象
byte set = COMMON_ANODE;  // 如果是共阳极数码管则为COMMON_ANODE
byte number = 4;  // 数码管位数
byte COM[] = {0, 21, 20, 7};  // 数码管的COM引脚
byte pins[] = {1, 10, 5, 4, 3, 2, 6, 9};  // 数码管的段选引脚

unsigned long startTime = 0;  // 计时开始时间
unsigned long elapsedTime = 0;  // 经过的时间

void setup() {
  sevseg.begin(set, number, COM, pins);  // 初始化数码管
  bool disableDecPoint = true;  // 禁用小数点
  startTime = millis();  // 初始化开始时间为当前时间
}

void loop() {
  elapsedTime = millis() - startTime;  // 计算经过的时间
  
  // 将毫秒转换为秒
  int seconds = elapsedTime / 1000;

  // 显示在数码管上
  sevseg.setNumber(seconds, 0);  // 设置秒数
  sevseg.refreshDisplay();  // 刷新显示
}
