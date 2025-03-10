/**********************************************************
 * @file main.c
 * @brief use AHT10 detect temperature & humidity
 *        	VDD 	- 3.3V
 *       	GND 	- GND
 * 	  	SCL/SCK - PB8    // IIC时钟信号
 *	  	SDA 	- PB9    // IIC数据信号
 **********************************************************/

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "AHT10.h"
#include "I2C.h"

//主函数	
int main(void)
{
    float temperature,humidity;
	uint16_t tmp;
	delay_init();	    	 //延时函数初始化	  
	
	OLED_Init();		 //初始化OLED  
	OLED_Clear(0);           //清除屏幕
	OLED_ShowString(2, 2, "Humid :",16);	
	OLED_ShowString(2, 4, "Temper:",16);		
    I2C_Initation();
	
    while(1)
    {
		AHT10_Read_Humi_Temp(&humidity,&temperature);
		OLED_ShowNum(60, 2, humidity,3, 16); 
		OLED_ShowString(83, 2, ".",16); 

		tmp = (uint16_t)(humidity*100) % 100;  //小数点后两位
		if (tmp < 10){
			OLED_ShowNum(90, 2, tmp, 2, 16);	
			OLED_ShowString(89, 2,"0",16); 		//个位前面补0	
		} else
		{
			OLED_ShowNum(90, 2, tmp, 2, 16);			
		}
		
		OLED_ShowNum(60, 4, temperature,3, 16); 
		OLED_ShowString(83, 4, ".",16); 		
		tmp = (uint16_t)(temperature*100) % 100;  //小数点后两位
		if (tmp < 10){
			OLED_ShowNum(90, 4, tmp, 2, 16);	
			OLED_ShowString(89, 4,"0",16); 		//个位前面补0	
		} else
		{
			OLED_ShowNum(90, 4, tmp, 2, 16);			
		}
		
		delay_ms(300);
		

    }
}
