#include "MP3_tf.h"
#include "biaopan.h"
#include "stm32f10x.h"               
#include "menu.h"
#include "Delay.h"  
#include "Serial2.h"

uint8_t mp3repetition1 = 0;

uint8_t mp3play1 = 0;	//播放标志位
uint8_t mp3while = 0;	//循环播放标志位
uint8_t mp3PLSD = 0;	//播完即停标志位

uint8_t mp3volume= 10;   //MP3音量

uint8_t mp3chapter= 0 ;   //MP3歌曲位

uint8_t mp3chapterMAX= 28;   //MP3歌曲位最大数

uint8_t mp3data= 0 ;   //MP3需发送数据位



void MP3_starts(uint8_t e) //MP3是否开始播放
{	
	if(e)
	{	
		MP3CMD(  0x06 , 0x00 ,  mp3volume );  //设置音量
		delay_ms(10);
//		MP3CMD(  0x12 , 0x00 ,  mp3chapter );
		MP3CMD(  0x0D , 0x00 ,  0x00 );     //开始播放
		delay_ms(200);
		mp3play1 = 1;
	}
	else
	{
		MP3CMD(  0x0E , 0x00 ,  0x00 );  //停止播放
		delay_ms(200);
		mp3play1 = 0;
	}
}

void MP3_chin(uint8_t e) //MP3曲目增加，减少
{
	if(e)
	{
		mp3chapter++;
		if(mp3chapter > mp3chapterMAX) mp3chapter=0;		
		mp3play1 = 1;
		mp3data=1;
	}
	else
	{
		mp3chapter--;
		if(mp3chapter > mp3chapterMAX) mp3chapter=mp3chapterMAX;
		mp3play1 = 1;
		mp3data=1;
	}	
}





void MP3_Init(void)
{
	//扬声器使能//
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
	
	Serial2_Init();


}

void MP3_Power_OFF(void)
{		
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
	Serial2_Power_OFF();
	
}


uint8_t Send_buf[8];
void MP3CMD(uint8_t CMD ,uint8_t feedback , uint16_t data)
{
    Send_buf[0] = 0x7e;    //头
    Send_buf[1] = 0xff;    //保留字节 
    Send_buf[2] = 0x06;    //长度
    Send_buf[3] = CMD;     //控制指令
    Send_buf[4] = feedback;//是否需要反馈
    Send_buf[5] = (uint8_t)(data >> 8);//data
    Send_buf[6] = (uint8_t)(data);     //data
    Send_buf[7] = 0xef;    //尾
	
	for (u8 i = 0; i < 8; i ++)		//遍历数组
	{
		Serial2_SendByte(Send_buf[i]);	  //依次调用Serial_SendByte发送每个字节数据
	}
}


extern uint8_t GUODUsn;

void MP3os(void)
{
	GUODUsn=1;
	int8_t p;
	uint16_t XIUMIAN=2;                    //临时的屏幕休眠时间变量
	while(1)
	{
		OLED_Clear();	
			
		  biaopan_byd(90,0);
		  biaopan_Timer_small(0,0);	
			
		  if(mp3play1)OLED_ShowImage(40, 27, 16, 16,mp3play);
		  else OLED_ShowImage(40, 27, 16, 16,mp3stop);
			
		  OLED_ShowImage(18, 27, 16, 16,mp3right);
		  OLED_ShowImage(62, 27, 16, 16,mp3left);
		
		 OLED_ShowNum(0, 56, mp3chapter, 3, OLED_6X8);
		 OLED_ShowNum(20, 56, mp3chapterMAX, 3, OLED_6X8);

		if(mp3PLSD) OLED_ShowString(104,52,"单停",OLED_6X8);
		else if(mp3while) OLED_ShowString(104,52,"循环",OLED_6X8);
		else OLED_ShowString(104,52,"顺序",OLED_6X8);

		
		
	    OLED_Update();	 
		
		OLED_transition(1);
	
		
		if(XIUMIAN>400){    //轮循等待关机时间
			
			GUODUsn=1;
			OLED_transition(0);
			OLED_Clear();				//清空显存数组
			OLED_Update();				//更新显示，清屏
			
			OLED_WriteCommand(0xAE);	//0xAF开启显示，0xAE为休眠
			
			OLED_WriteCommand(0xAD);	//设置充电泵
			OLED_WriteCommand(0x8A);    //0x8B为开启，0x8A为关闭充电泵
			
			XIUMIAN=0;
	   }
	   else if(XIUMIAN==1)  
		{ 	
			OLED_WriteCommand(0xAD);	//设置充电泵
			OLED_WriteCommand(0x8B);    //0x8B为开启，0x8A为关闭充电泵
			
			OLED_WriteCommand(0xAF);	//0xAF开启显示，0xAE为休眠
				
			XIUMIAN++;
			
			GUODUsn=1;
		}
		else if(XIUMIAN>1) XIUMIAN++;
				
		
		p = menu_Roll_event(); 
	   if( p == 1){ 
		   
		   if(XIUMIAN){
			   MP3_chin(1);
		       XIUMIAN=2;
		   }
		   else XIUMIAN++;
	   }	 
       else if( p == -1){
		   
		   if(XIUMIAN){
		       MP3_chin(0);
			   XIUMIAN=2;
		   }
		   else XIUMIAN++;
	   }	
	   else if(XIUMIAN>25 | XIUMIAN==0) 
	   {
			if(	mp3data)
			{
				   MP3CMD(  0x06 , 0x00 ,  mp3volume );  //设置音量
					delay_ms(50);
				   MP3CMD(  0x12 , 0x00 ,  mp3chapter );//开始播放
				   mp3data=0;
			}
	   }
	   
	   p=menu_Enter_event();
		if(p==1)
		{   
		   if(XIUMIAN)
		   {
			    XIUMIAN=2;
			    if(mp3play1)
				{
					MP3_starts(0);
				}
				else
				{
					MP3_starts(1);
				}
			}
		   else XIUMIAN++;
		}
		else if ( p==2 )
		{
			XIUMIAN++;			
			if(XIUMIAN==1)
			{			
				OLED_WriteCommand(0xAF);	//0xAF开启显示，0xAE为休眠
				OLED_WriteCommand(0xAD);	//设置充电泵
				OLED_WriteCommand(0x8B);    //0x8B为开启，0x8A为关闭充电泵
			}
			else
			{
				MP3CMD(  0x19 , 0x00 ,  0x00 );
				GUODUsn=1;
				OLED_transition(0);
				return;
			}	
		}
	}
	
}



void MP3ping(void)
{
	  MP3CMD(  0x06 , 0x00 ,  mp3volume );
}

