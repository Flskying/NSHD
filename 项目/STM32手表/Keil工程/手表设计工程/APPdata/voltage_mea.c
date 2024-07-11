#include "stm32f10x.h"               
#include "voltage_mea.h"
#include "AD.h"
#include "OLED.h"     
#include "Delay.h"  
#include "menu.h"

#include "biaopan.h"


float RTI = 9.531;
extern  float VIN ;



void voltage_mt(void)
{	
	
	int8_t p ,i;
	GUODUsn=1;
	
	float voltage;
	float liangc;                           //量程
	uint16_t ADValue;
	float GP=0 , GP_target ;                 //条条长度，目标条条长度
	while(1)
	{
	
	//计算电压
		
			
		OLED_Clear();
		
		  biaopan_byd(90,0);
		  biaopan_Timer_small(0,0);	

		
		if(i>25)
		{
			ADValue = AD_GetValue(ADC_Channel_9);
			liangc  =  (float)ADValue / 4095 * 100;
			voltage = ( ( (float)ADValue * VIN ) / 4095 ) * RTI;
			i=0;
		}
		else i++ ;                                       
		
			OLED_ShowFloatNum(33, 16, voltage, 2, 3, OLED_8X16);
			OLED_ShowASCII(89, 24, "V", OLED_6X8);
		
			OLED_ShowFloatNum(0, 56, RTI, 1, 3, OLED_6X8);
		
		    OLED_ShowASCII(50, 56, "Critical:   %", OLED_6X8);
			OLED_ShowNum(104, 56, liangc, 3, OLED_6X8);
		
		GP_target = voltage * 4 ;
		if(GP < GP_target)
		{ 
			if( GP_target - GP > 25 ) GP += 5 ;
			else if( GP_target - GP > 7 ) GP += 2.4 ;
			else if( GP_target - GP > 0 ) GP += 0.5 ;
		}
		if(GP > GP_target)
		{ 	
			if( GP - GP_target > 25 ) GP -= 5 ;
			else if( GP - GP_target > 7 ) GP -= 2.4 ;
			else if( GP - GP_target > 0 ) GP -= 0.5 ;
		}
		if(GP > 80) 	GP=80;		
		
		
		OLED_ClearArea(24, 42, 80, 3);
		OLED_DrawRectangle(24, 42, GP , 3, OLED_FILLED);
		
		OLED_DrawLine(24,49,104 ,49);
		OLED_DrawLine(24,47,24 ,49);
		OLED_DrawLine(44,47,44 ,49);
		OLED_DrawLine(64,47,64 ,49);
		OLED_DrawLine(84,47,84 ,49);
		OLED_DrawLine(104,47,104 ,49);
		
		OLED_Update();	

		
		OLED_transition(1);

		
		p = menu_Roll_event();   
	   if( p == 1){ RTI=RTI+0.001; }	 
       else if( p == -1){  RTI=RTI-0.001; }	
	   
		p=menu_Enter_event();
		if(p)
		{  
			GUODUsn=2;
			OLED_transition(0);
			return;       
		}

	}


}
	

void voltage_waveform(void)
{	
	
	
}


