/**********************************************************
 * @file  delay.c
 * @brief delay function
**********************************************************/

#include "delay.h"
#include "sys.h"
 
 static u8  fac_us=0;	//us延时倍乘数
static u16 fac_ms=0;	//ms延时倍乘数

//初始化延时函数
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);    //选择外部时钟 HCLK/8
    fac_us = SystemCoreClock/8000000;                        //为系统时钟的1/8
    fac_ms = (u16)fac_us*1000;                               //每个ms需要的systick时钟数
}								    

//延时nus，nus 为要延时的微秒数.	

void delay_us(u32 nus)
{		
	u32 temp;
    SysTick->LOAD = nus*fac_us;                 //时间载入
    SysTick->VAL = 0x00;                        //清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;  //开始倒数
    do
    {
        temp = SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));       //等待时间到达
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //关闭计数器
    SysTick->VAL = 0x00;                        //关闭计数器
}

//延时 nms，nms 为要延时的毫秒数
//nms的范围:SysTick->LOAD为24位暂存器,最大延时为: nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz, 单位为ms
//对72MHz条件下,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
    u32 temp;
    SysTick->LOAD = (u32)nms*fac_ms;            //時間载入(SysTick->LOAD为24bit)
    SysTick->VAL = 0x00;                        //清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;  //开始倒數
    do
    {
        temp=SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));       //等待时间到达
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //关闭计数器
    SysTick->VAL = 0x00;                        //关闭计数器
} 

//延时 s 秒，s 为延时秒数，最大值为65535
void delay_s(u16 s)			
{ 	 		  	  
	while( s-- != 0)
	{
		delay_ms(1000);		//等待1000毫秒的延时
	}
}
