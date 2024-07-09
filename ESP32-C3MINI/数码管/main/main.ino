#include "SevSeg.h"
SevSeg sevseg;//给数码管命名
byte set=COMMON_ANODE;//若为共阳极数码管则为COMMON_ANODE 
byte number=4;//四位
byte COM[]={0,21,20,7};
byte pins[]={1,10,5,4,3,2,6,9};
 
 
 
void setup() {
  sevseg.begin(set,number,COM,pins);
  bool disableDecPoint = true;
 
}
void loop() {
  int m=1000;
   sevseg.setNumber(m,0);
   sevseg.refreshDisplay();//刷新显示
   }