
/**
  *		这份代码目前还有很多地方没写完
  *     以及很多地方没有分类和写注释，简直是一坨(自我评价)
  *		写的途中遇到了很多小问题
  *		看之后有时间再更新吧
  *     版本V1.14
  **/
  
#include "stm32f10x.h"    
#include "OLED.h"     
#include "Delay.h"  
#include "menu.h"
#include "AD.h"

#include "Serial2.h"  
#include "string.h"                          //字符串判断函数库
uint8_t  Up=0 , Down=0 , Back=0 , Enter=0 ; //定义虚拟按键；上，下，返回，确定 //0为未按下，1为按下//暂时未使用
uint8_t  connect=0 ;                       //连接标志位，1为连接成功，0为未连接

#include "inv_mpu.h"            //mpu6050陀螺仪传感器
float Pitch,Roll,Yaw;          //定义3个储存角度的变量
short gyroX,gyroY,gyroZ,accelX,accelY,accelZ; 
uint8_t TLYPW=0;             //陀螺仪开启状态标志位


#include "key.h"

#include "MyRTC.h"
#include "biaopan.h"

#include "MP3_tf.h"
#include "W25Q128_SPI1.h"



void STM32_rouse(void);
void MP3_USART(void);

extern uint8_t GUODUsn;
int main(void)
{
	    Key_Init();
	  
		delay_init();												       //延时函数初始化
	
		OLED_Init();                                          		     //oled显示屏初始化
	
		OLED_WriteCommand(0x81);	//设置对比度
		OLED_WriteCommand(0x00);	//0x00~0xFF
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   //重映射配置,用PB3，PB4，PA15做普通IO，PA13&14用于SWD调试
			
		biaopan_io_init();
	
// 		MPU6050_DMP_Init();                       	  			//陀螺仪初始化
		
     	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//配置NVIC为分组2
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
		AD_Init();				                      //AD转换器初始化
		
        MyRTC_Init();							    //RTC初始化

		STM32_rouse();                            //唤醒休眠所用的中断引脚
		
		int8_t p;                               //存储按键值变量
		
		uint8_t XIUMIAN=0;                    //临时的主屏幕休眠时间变量
		
		W25Q128_Init();
		
		
		GUODUsn=1;
		   while(GUODUsn)			
		   { 
			   biaopan();
				delay_ms(4);		   
			   OLED_transition(1);	   
		   }


   while(1)
   {  	
	   

		 biaopan();  //刷新表盘
	   
   
	   p = menu_Roll_event();	//获取按键
	   
	   if(XIUMIAN>200){    //轮循等待关机时间
		    p = -1;
			XIUMIAN=0;
	   }
	   else{ XIUMIAN++; }
	   
	   if( p == 1)
	   { 
		   XIUMIAN=0;
		   GUODUsn=1;
		   OLED_transition(0);
		   

		   main_menu2(); //进入菜单2
		   
		   
		   GUODUsn=1;
       }	 
       else if( p == -1)
	   { 
		   XIUMIAN=0;
		   GUODUsn=1;
		   OLED_transition(0);
		   
		   
	       Power_OFF(); //关机
		   
		   
		   GUODUsn=1;
		   			   
		   
	   }	   
	   
	   while(GUODUsn)			//这一段是为了防止开机后立即就去判断按键，防止误触发
	   { 
		   biaopan();
			delay_ms(4);		   
		   OLED_transition(1);	   
	   }
	   
	   
	   p = menu_Enter_event();	   
	   if( p ==1 )
	   { 
		   XIUMIAN=0;
		   GUODUsn=1;
		   OLED_transition(0);
		   
		   main_menu1(); //进入菜单1
		   
		   GUODUsn=1;
	   }
		else if(p==2)	  
		{	
			
			GUODUsn=1;
			OLED_transition(0);
			
			
				
			/*进入停止*/
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);		
		
			OLED_Clear();				//清空显存数组
			OLED_Update();				//更新显示，清屏
			
			OLED_WriteCommand(0xAE);	//0xAF开启显示，0xAE为休眠
			
			OLED_WriteCommand(0xAD);	//设置充电泵
			OLED_WriteCommand(0x8A);    //0x8B为开启，0x8A为关闭充电泵
			
			/*使能WKUP引脚*/
			PWR_WakeUpPinCmd(ENABLE);						//使能位于PA0的WKUP引脚，WKUP引脚上升沿唤醒待机模式
			PWR_EnterSTANDBYMode();						//STM32进入停止模式，并等待指定的唤醒事件（WKUP上升沿或RTC闹钟）
		}			
   }


}



void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)		//外部中断0号线触发的中断
	{
		
		EXTI_ClearITPendingBit(EXTI_Line0);		//清除中断标志位
																								
	}
}




void STM32_rouse(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//开启AFIO的时钟，外部中断必须开启AFIO的时钟
		
	/*AFIO选择中断引脚*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);//将外部中断的0号线映射到GPIOA，即选择PA0为外部中断引脚
	
	/*EXTI初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;					//选择配置外部中断的0号线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//指定外部中断线为上升沿触发
	EXTI_Init(&EXTI_InitStructure);								//将结构体变量交给EXTI_Init，配置EXTI外设
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;		    //选择配置NVIC的0线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//指定NVIC线路的响应优先级为2
	NVIC_Init(&NVIC_InitStructure);		
}


