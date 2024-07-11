#include "video_app.h"
#include "stm32f10x.h"               
#include "MP3_tf.h"
#include "Serial3.h"
#include "Serial2.h"
#include "Delay.h"  
#include "menu.h"
#include "W25Q128_SPI1.h"


extern uint8_t OLED_DisplayBuf[8][128];

//因为不同视频播放逻辑基本一样，只是偏移地址计算不一样，所以写了个通用的一起用
void SHOW_VIDEO(uint32_t Address ,uint16_t mpg)//总帧量 , 读取数据起始地址
{	
		int8_t p;		
	    p0=0,p1=0;
		
		uint8_t temp[3];   //文件头(3字节)
		uint32_t mydata=0; //总数据量(不包括文件头)
	    uint32_t datasite=3; //帧数据偏移地址

	
		OLED_Clear();
		OLED_Update(); 
		OLED_WriteCommand(0x81);	//设置对比度
		OLED_WriteCommand(0xFF);	//0x00~0xFF
	
		W25Q128_ReadData( Address , temp , 3 );
	
		if(temp[0]!=0xAA) //检测数据头是否存在
		{	
			OLED_ShowString(0,20,"未检测到文件",OLED_6X8);
			OLED_Update(); 
			
			delay_ms(500);
			GUODUsn=1;
			OLED_transition(0);
			return;
		}
		
		mydata= (temp[2]<< 8) |  temp[1];  //获取总帧数量
		
		OLED_ShowString(0,0,"帧数:30FPS",OLED_6X8);
		OLED_ShowString(0,13,"帧量:",OLED_6X8);
		OLED_ShowNum(38,15,mydata,4,OLED_6X8);
		
		mydata *= 1024; 				 //计算总数据量
		
		OLED_ShowString(0,26,"数据量(字节):",OLED_6X8);
		OLED_ShowNum(0,40,mydata,7,OLED_6X8);
		
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1){
			
			OLED_ShowString(0,52,"MP3已开启",OLED_6X8);
			
			Serial2_Init();
			
			MP3CMD(  0x06 , 0x00 ,  mp3volume );
			delay_ms(50);
			MP3CMD(  0x0F , 0x00 ,  mpg );	
			
		}		
		else OLED_ShowString(0,52,"MP3未开启",OLED_6X8);
		
		OLED_ShowString(92,52,"读取中",OLED_6X8);		//这里实际上是为了掩盖mp3音乐的延迟
		
		OLED_Update(); 
		
			delay_ms(1800);				//音乐播放延迟，根据实际微调
			delay_ms(1800);
			delay_ms(1800);
			delay_ms(1550);
		
		
		
	 uint16_t temm=15;

	while(1)
	{
		
		W25Q128_ReadData( Address+datasite , OLED_DisplayBuf[0] , 1024 );//读取一帧到显存
		datasite+=1024;       //地址偏移到下一帧
		
		delay_ms(temm);		//帧间隔，根据实际微调
		
		OLED_Update();  		
        OLED_transition(1);
		
		
		p=menu_Roll_event(); //微调帧间隔
		if (p==1){			
			temm++;
		}	
		else if(p == -1){
		    temm--;
		}
		
			
		p=menu_Enter_event();  //手动退出|停止
		if (p==1){
		
			do
			{ 
				OLED_Update();  		
				p=menu_Enter_event(); 
				
				if(p == 2){
			
					if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
					{	
						MP3_starts(0);
						Serial2_Power_OFF();
					
					}
					GUODUsn=1;
					OLED_transition(0);
					return;	
				}
				
			
			}
			while(p !=1 ); 
			
			
		}
		else if(p == 2){
			
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
			{	
				MP3_starts(0);
				Serial2_Power_OFF();
			
			}
			GUODUsn=1;
			OLED_transition(0);
			return;	
		}
		
		
		if(datasite > mydata) //是否播放完毕
		{	
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==1)
			{	
				
				MP3_starts(0);				
				Serial2_Power_OFF();
			
			}
			OLED_Clear();
			OLED_ShowString(0,20,"文件播放完毕",OLED_6X8);
			OLED_Update(); 
			delay_ms(500);
			GUODUsn=1;
			OLED_transition(0);
			return;
		}
	}

	
}

uint32_t VIDEO_SITE(uint32_t Address) //跟据输入的视频地址计算下一个视频地址
{
		uint8_t temp[3];   //文件头(3字节)
	    uint32_t mydata=0; //总数据量(不包括文件头)
		W25Q128_ReadData( Address , temp , 3 );

			mydata= (temp[2]<< 8) |  temp[1];  //获取总帧数量	
			mydata*=1024;			
			Address += (mydata+3) ;
		
	return Address;	
}


void VIDEO_Bad_apple(void)
{
		uint32_t Address=163840;//视频文件数据起始地址 0x28000	
	
		SHOW_VIDEO(Address,0x0101); //顺便播放tf卡里01文件夹的01歌曲
}

void VIDEO_CXUK(void)
{			
		SHOW_VIDEO( VIDEO_SITE(163840) ,0x0102);
}


void VIDEO_custom(void)
{				
		SHOW_VIDEO( (VIDEO_SITE( VIDEO_SITE(163840) )+4096)  ,0x0103);
}										//最好加1扇区空闲，不然可能误擦除


//同上，写了个通用的一起用
void WRITE_VIDEO(uint32_t Address)//数据起始地址
{	
	
		int8_t p;
	
		Serial3_Init();
		
	    p0=0,p1=0 ,we=0;
		
		uint8_t temp[3]={0xAA,0,0};   //文件头(3字节)
	    uint32_t datasite=3;    //帧数据偏移地址
	
	
		OLED_WriteCommand(0x81);	//设置对比度
		OLED_WriteCommand(0xFF);	//0x00~0xFF
	
		OLED_Clear();
		OLED_ShowString(0,20,"开放烧录中",OLED_6X8);
		OLED_Update(); 
	
		

	while(1)
	{
		if(we)//如果有数据
		{	
			W25Q128_unlimited_Program( Address+datasite , OLED_DisplayBuf[0] , 1024 );//烧录一帧到显存
			datasite+=1024;       //地址偏移到下一帧
			we=0;
		}
		
		
		p=menu_Enter_event();
		if (p){
		
			if( (datasite-3) != 0 ) //如果有数据
			{	
				temp[1]= ((datasite-3)/1024) & 0xFF;    
				temp[2]= ( ( (datasite-3) /1024 ) >> 8 ) & 0xFF;  //已存帧数
				
				W25Q128_unlimited_Program( Address , temp , 3 );	//写入已存头数据
				
				OLED_Clear();
				OLED_ShowString(0,20,"文件烧录完",OLED_6X8);
				OLED_Update(); 
				delay_ms(500);
				GUODUsn=1;
				OLED_transition(0);
				return;	
			}
			else
			{
				OLED_Clear();
				OLED_ShowString(0,20,"未检测到烧录数据流",OLED_6X8);
				OLED_Update(); 
				delay_ms(500);
				GUODUsn=1;
				OLED_transition(0);
				return;	
			}
				
		}	
		
	}
	

}


void VIDEO_write_Bad_apple(void)
{
//		uint32_t Address=163840;//数据起始地址 0x28000	
		WRITE_VIDEO(163840);	
}

void VIDEO_write_CXUK(void)
{	
		WRITE_VIDEO( VIDEO_SITE(163840) );	
}



void VIDEO_write_custom(void)
{	
		uint8_t temp[3];   //文件头(3字节)
		uint32_t data,pdata=0; 		
		double mydata=0; //总数据量(不包括文件头)
	
		data=( VIDEO_SITE( VIDEO_SITE(163840) )+4096 );
		
		
		W25Q128_ReadData( data , temp , 3 );
		
		if(temp[0] == 0xAA) //如果有数据
		{
			mydata= (temp[2]<< 8) |  temp[1];  //获取总帧数量	
			mydata*=1024;				   	  //计算数据量	

			mydata /= 4096;                 //得到需删除扇区数
			mydata++;
			OLED_Clear();
			OLED_ShowString(0,20,"擦除中",OLED_6X8);
			OLED_Update(); 
			
				OLED_WriteCommand(0x81);	//设置对比度
				OLED_WriteCommand(0xFF);	//0x00~0xFF

			
			while( pdata < mydata )
			{	
				W25Q128_SectorErase( data+(pdata*4096) );
				pdata++;					
			}
		}
	
	    WRITE_VIDEO( data );	
}										

void VIDEO_erase(void)
{	
		int8_t p;
		GUODUsn=1;
			uint32_t data = 163840; 

		OLED_Clear();
		OLED_ShowString(0,20,"擦除中",OLED_6X8);
		OLED_Update();
	
				
				OLED_WriteCommand(0x81);	//设置对比度
				OLED_WriteCommand(0xFF);	//0x00~0xFF

		while(1)
		{
			
			
			W25Q128_SectorErase( data );
			
			OLED_ShowNum(0,40,data,8,OLED_6X8);
			
			OLED_Update();
			
			data+=4096;	
			
			
			
			p=menu_Enter_event();
			if(data>0xFFFFFF) p=1 ;
		
			if (p)
			{
					
				OLED_ShowString(0,20,"擦除停止",OLED_6X8);
				OLED_Update(); 
				delay_ms(1000);
				GUODUsn=1;
				OLED_transition(0);
				return;	
			}	
		}

}


void VIDEO_streaming(void)
{	
		int8_t p;
		GUODUsn=1;
	
		Serial3_Init();
		
	    p0=0,p1=0 ;
	
		OLED_Clear();

	
	while(1)
	{
		
		OLED_Update(); 
		
		OLED_transition(1);
		
		p=menu_Enter_event();
		if (p){
			
			Serial3_Power_OFF();
			GUODUsn=1;
			OLED_transition(0);
			return;	
		}	
		
	}
}

	
	

