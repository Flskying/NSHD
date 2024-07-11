#include "biaopan.h"
#include "stm32f10x.h"               
#include "OLED.h"     
#include "MyRTC.h"
#include "menu.h"
#include "AD.h"


//定义电压与电池容量关系的数组
const float battery_voltage[31]={ 6.7, 4.16, 4.15, 4.14, 4.12, 4.10, 4.08, 4.05, 4.03, 3.97, 3.93, 3.90, 3.87, 3.84, 3.81, 3.79, 3.77, 3.76, 3.74, 3.73, 3.72, 3.71, 3.68, 3.66, 3.64, 3.62, 3.60, 3.55, 3.50, 3.45, 3.40};
const uint8_t battery_level[31]={ 255, 255, 255, 255, 255, 100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 48, 47, 46, 45, 44, 43, 42, 41, 40, 30, 24, 18, 12, 6, 0 };

float VIN = 3.31;
float UP_voltage = 4.2;
//获取电池电量并显示在右上角
void biaopan_byd(u8 x,u8 y)
{
	uint16_t ADValue  ;
	
	ADValue = AD_GetValue(ADC_Channel_8);
	
	float voltage = ((float)ADValue * VIN ) / 4095 * 2 ;//得到电池电压（用了两个一样阻值的分压电阻）
	
	
	for(uint8_t i=0;i<31;i++)
	{
		if((voltage<battery_voltage[i]) & (voltage>=battery_voltage[i+1]))
		{
			//线性插值得到电池电量
		   voltage = battery_level[i+1]+( (voltage-battery_voltage[i+1]) * ( (battery_level[i]-battery_level[i+1])/(battery_voltage[i]-battery_voltage[i+1]) ) );
			//此时voltage为电池电量 100 到 0
		   break;
		}
	}
	
	if(voltage < UP_voltage) //防止电量抖动
	{
		UP_voltage=voltage;
	}
	else if(voltage - UP_voltage > 5 )
	{
		UP_voltage=voltage;
	}
	
	if( GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)==0 ) OLED_ShowASCII( x+25, y+1,"--", OLED_6X8 );
	
	OLED_DrawRectangle(x+25, y+1, 12, 7, OLED_UNFILLED);//画电池图案
	OLED_DrawLine(x+37, y+3, x+37, y+5);               //画电池图案

	
	
	if(UP_voltage>9)OLED_DrawLine(x+26, y+2, x+26, y+6);
	if(UP_voltage>19)OLED_DrawLine(x+27, y+2, x+27, y+6);
	if(UP_voltage>29)OLED_DrawLine(x+28, y+2, x+28, y+6);
	if(UP_voltage>39)OLED_DrawLine(x+29, y+2, x+29, y+6);
	if(UP_voltage>49)OLED_DrawLine(x+30, y+2, x+30, y+6);
	if(UP_voltage>59)OLED_DrawLine(x+31, y+2, x+31, y+6);
	if(UP_voltage>69)OLED_DrawLine(x+32, y+2, x+32, y+6);
	if(UP_voltage>79)OLED_DrawLine(x+33, y+2, x+33, y+6);
	if(UP_voltage>89)OLED_DrawLine(x+34, y+2, x+34, y+6);//根据电池电量来绘制格子
	
	if(UP_voltage>99)
	{
		OLED_DrawLine(x+35, y+2, x+35, y+6);
		OLED_ShowNum(x+5, y+1, 100, 3, OLED_6X8);
		
	}
	else OLED_ShowNum(x+11, y+1, UP_voltage, 2, OLED_6X8);	
	
}


//在指定位置显示表盘数字
void bittt(u8 x,u8 y,u8 i,u8 p)
{	
	if(p)
	{
		switch(i)
		{ 
			case 1:OLED_ShowImage(x, y, 13, 30,t1);break;
			case 2:OLED_ShowImage(x, y, 13, 30,t2);break;
			case 3:OLED_ShowImage(x, y, 13, 30,t3);break;
			case 4:OLED_ShowImage(x, y, 13, 30,t4);break;
			case 5:OLED_ShowImage(x, y, 13, 30,t5);break;
			case 6:OLED_ShowImage(x, y, 13, 30,t6);break;
			case 7:OLED_ShowImage(x, y, 13, 30,t7);break;
			case 8:OLED_ShowImage(x, y, 13, 30,t8);break;
			case 9:OLED_ShowImage(x, y, 13, 30,t9);break;
			case 0:OLED_ShowImage(x, y, 13, 30,t0);break;
		 }
	}
	else
	{
		switch(i)
		{ 
			case 1:OLED_ShowImage(x, y, 9, 21,i1);break;
			case 2:OLED_ShowImage(x, y, 9, 21,i2);break;
			case 3:OLED_ShowImage(x, y, 9, 21,i3);break;
			case 4:OLED_ShowImage(x, y, 9, 21,i4);break;
			case 5:OLED_ShowImage(x, y, 9, 21,i5);break;
			case 6:OLED_ShowImage(x, y, 9, 21,i6);break;
			case 7:OLED_ShowImage(x, y, 9, 21,i7);break;
			case 8:OLED_ShowImage(x, y, 9, 21,i8);break;
			case 9:OLED_ShowImage(x, y, 9, 21,i9);break;
			case 0:OLED_ShowImage(x, y, 9, 21,i0);break;
		 }
	}
}



void biaopan_Timer(u8 x,u8 y)//获取时间并在指定位置显示
{

		MyRTC_ReadTime();
		uint16_t i=MyRTC_Time[3];
		uint16_t p= i ;
	
	    if(i  > 12) 
		{
			i = i-12 ;
			p = i ;
			
			OLED_ShowASCII(x+109, y+21, "PM", OLED_6X8);
		} 
		else OLED_ShowASCII(x+109, y+21, "AM", OLED_6X8);
		
	
		i= i%10;
		bittt(x+19,y+0,i,1);
		i=(p-i)/10;
		bittt(x+0,y+0,i,1);
		
		OLED_DrawRectangle(x+37, y+6, 2, 4, OLED_UNFILLED);
		OLED_DrawRectangle(x+37, y+19, 2, 4, OLED_UNFILLED);
		
		i=MyRTC_Time[4];
		p=i;
		i= i%10;
		bittt(x+63,y+0,i,1);
		i=(p-i)/10;
		bittt(x+44,y+0,i,1);
		
		i=MyRTC_Time[5];
		p=i;
		i= i%10;
		bittt(x+95,y+8,i,0);
		i=(p-i)/10;
		bittt(x+81,y+8,i,0);   
}

void biaopan_date(uint8_t x,uint8_t y)
{
		OLED_ShowNum(x, y, MyRTC_Time[0], 4, OLED_6X8);
		OLED_ShowNum(x+26, y, MyRTC_Time[1], 2, OLED_6X8);
	    OLED_ShowNum(x+40, y, MyRTC_Time[2], 2, OLED_6X8);
}

void biaopan_week(uint8_t x,uint8_t y)
{	
	
		uint16_t week = MyRTC_Time[6];
	
		OLED_ShowASCII(x, y, "week:", OLED_6X8);
		switch(week)
		{ 
			case 1:OLED_ShowASCII(x+30, y, "1", OLED_6X8);break;
			case 2:OLED_ShowASCII(x+30, y, "2", OLED_6X8);break;
			case 3:OLED_ShowASCII(x+30, y, "3", OLED_6X8);break;
			case 4:OLED_ShowASCII(x+30, y, "4", OLED_6X8);break;
			case 5:OLED_ShowASCII(x+30, y, "5", OLED_6X8);break;
			case 6:OLED_ShowASCII(x+30, y, "6", OLED_6X8);break;
			case 0:OLED_ShowASCII(x+30, y, "R", OLED_6X8);break;
		 }
	
}
extern uint8_t GUODUsn;

void biaopan_Timer_data(void)
{
	GUODUsn=1;
	
	int8_t p;
	uint8_t i=0;
	uint16_t t=0;
	while(1)
	{
		OLED_Clear();
		OLED_ShowNum(6, 5, MyRTC_Time[0], 4, OLED_6X8);
		OLED_ShowNum(41, 5, MyRTC_Time[1], 2, OLED_6X8);
	    OLED_ShowNum(66, 5, MyRTC_Time[2], 2, OLED_6X8);
		OLED_ShowNum(6, 20, MyRTC_Time[3], 2, OLED_6X8);
		OLED_ShowNum(31, 20, MyRTC_Time[4], 2, OLED_6X8);
	    OLED_ShowNum(56, 20, MyRTC_Time[5], 2, OLED_6X8);
		
			switch(i)
			{ 
				case 0:OLED_ShowASCII(0, 5, "-", OLED_6X8);break;
				case 1:OLED_ShowASCII(35, 5, "-", OLED_6X8);break;
				case 2:OLED_ShowASCII(60, 5, "-", OLED_6X8);break;
				case 3:OLED_ShowASCII(0, 20, "-", OLED_6X8);break;
				case 4:OLED_ShowASCII(25, 20, "-", OLED_6X8);break;
				case 5:OLED_ShowASCII(50, 20, "-", OLED_6X8);break;
			}
			
		OLED_Update();	
		
		OLED_transition(1);

		
		
		p = menu_Roll_event();   
	   if( p == 1){ 
		   
			   t = MyRTC_Time[i];
			   t++;
			   MyRTC_Time[i] = t;
		   
	   }	 
       else if( p == -1){
		   
			   t = MyRTC_Time[i];
			   t--;
			   MyRTC_Time[i] = t;
		   
	   }	
		p=menu_Enter_event();
		if(p==1)
		{                      
			i++;
			if(i>=6)
			{
				MyRTC_SetTime();
				
				GUODUsn=1;
				OLED_transition(0);				
				
				return;
			}
			

		}
		
	}
	
}


void biaopan_Timer_small(u8 x,u8 y)//获取时间并在指定位置显示
{

		MyRTC_ReadTime();
		uint16_t i=MyRTC_Time[3];
	
	    if(i  > 12) 
		{
			i = i-12 ;
			
			OLED_ShowASCII(x+42, y, "PM", OLED_6X8);
		} 
		else OLED_ShowASCII(x+42, y, "AM", OLED_6X8);
		
			
		OLED_ShowNum(x, y, i, 2, OLED_6X8);
			
		i=MyRTC_Time[4];
		
		OLED_ShowNum(x+14, y, i, 2, OLED_6X8);
		
		i=MyRTC_Time[5];
		
		OLED_ShowNum(x+28, y, i, 2, OLED_6X8);
}


extern uint8_t TLYPW;

void biaopan_APP(uint8_t x,uint8_t y)//显示各模块开启状态
{
	uint8_t temp=0;
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
	{	
		OLED_ShowImage(x+temp,y,8,8,APPYYBFsmall);
		temp+=10;
	}
	if(TLYPW)
	{	
		OLED_ShowImage(x+temp,y,8,8,APPTLYsmall);
		temp+=10;
	}
	
	
	if(temp==0) OLED_ShowASCII(0,0,"NO APP",OLED_6X8);

}


void biaopan_io_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;	      //电池充电检测脚
	GPIO_Init(GPIOB,&GPIO_InitStructure);   

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;	   //MP3电源检测脚
	GPIO_Init(GPIOA,&GPIO_InitStructure);   

}


void biaopan(void)
{
		   OLED_Clear();
		   biaopan_APP(0,0);
		   biaopan_byd(89,0);
		   biaopan_Timer(5,18);
		   biaopan_date(0,56);
           biaopan_week(90,57);
		   OLED_Update();	
}


	
	//获取时间并在指定位置显示
