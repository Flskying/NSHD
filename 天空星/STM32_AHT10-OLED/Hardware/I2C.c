#include "I2C.h"
#include "delay.h"

#define SDA GPIO_Pin_7
#define SCL GPIO_Pin_6
#define I2C_Prot GPIOB

#define SDA_High 	GPIO_SetBits(I2C_Prot,SDA)
#define SDA_Low 	GPIO_ResetBits(I2C_Prot,SDA)
#define SCL_High 	GPIO_SetBits(I2C_Prot,SCL)
#define SCL_Low 	GPIO_ResetBits(I2C_Prot,SCL)

void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = SDA;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(I2C_Prot,&GPIO_InitStructure);	
}

void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = SDA;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(I2C_Prot,&GPIO_InitStructure);	
}

//初始化IIC
void I2C_Initation(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能GPIOB時鐘
	   
	GPIO_InitStructure.GPIO_Pin = SDA|SCL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽輸出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_Prot, &GPIO_InitStructure);
	GPIO_SetBits(I2C_Prot,SDA|SCL); 	//PB6,PB7 輸出高
}

//IIC起始信号
void I2C_Start(void)
{
	SDA_OUT();     	//SDA设置为输出
	SDA_High;	
	SCL_High;
	delay_us(4);
	SDA_Low;		//START:当SCL为高电平时候，SCL拉低 
	delay_us(4);
	SCL_Low;		//准备发送或接收 
}

//IIC结束信号
void I2C_Stop(void)
{
	SDA_OUT();		//SDA配置为输出
	SDA_Low; 		//STOP：当SCL为低电平时候，SDA由低变为高
 	delay_us(4);
	SCL_High;	
	delay_us(4);
	SDA_High;		//发送IIC结束信号
	delay_us(4);							   	
}

u8 I2C_Write_Ack(void)
{
	u8 TimeAck = RESET;
	SDA_IN();
	SCL_High;
	delay_us(2);
	
	while(GPIO_ReadInputDataBit(I2C_Prot,SDA))
	{
		if(++TimeAck > 250)
		{
			I2C_Stop();
			return 1;
		}
	}
	SCL_Low;
	delay_us(2);	
	return 0;
}

// ack=0时, 不产生ACK应答, ack=1时候,产生ACK应答	
void I2C_Is_Ack(u8 ack)
{
	SCL_Low;
	SDA_OUT();
	if(ack)
		SDA_Low;	
	else
		SDA_High;	
	delay_us(2);
	SCL_High;		
	delay_us(2);
	SCL_Low;
}
					 				     
//IIC发送一位数据
void I2C_Send_Byte(u8 txd)
{                        
    SDA_OUT(); 	    
	SCL_Low;		//拉低时钟开始传输
    for(u8 t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			SDA_High;
		else
			SDA_Low;
		txd<<=1; 	  
		delay_us(2);   
		SCL_High;	
		delay_us(2); 
		SCL_Low;	
		delay_us(2);
    }	 
} 


//读一位数据，ack=1时，发送ACK，ack=0，发送nACK   
u8 I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();	//SDA设置为输入
    for(i=0;i<8;i++ )
	{
		SCL_High;			
		delay_us(2);
        receive<<=1;
        if(READ_SDA)receive++;   
		SCL_Low;			
		delay_us(1); 
    } 
	
    if (!ack)
		I2C_Is_Ack(0);
    else
		I2C_Is_Ack(1);	
    return receive;
}


//读取一位数据
u8 I2C_Read_Data(void)
{
	u8 Data = RESET;
	SDA_IN();		
	for(u8 i=0;i<8;i++)
	{
		SCL_High;
		delay_us(2);
		Data <<= 1; 
		
		if(GPIO_ReadInputDataBit(I2C_Prot,SDA) == SET)
		{
			Data |= 0x01;
		}
		
		SCL_Low;
		delay_us(2);
	}	
	return Data;
}

