#include "iic.h"

void IIC_Delay()
{unsigned char x;
	for(x=1;x>0;x--)
	{
		__NOP();__NOP();__NOP();__NOP();__NOP();
	}

}
void IIC_INIT(void)		//IIC初始化
{
	GPIO_InitTypeDef GPIO_INIT;
	RCC_APB2PeriphClockCmd(IIC_RCC,ENABLE);
	
	GPIO_INIT.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_INIT.GPIO_Pin=IIC_SDA | IIC_SCL;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
	
	IIC_SDA_H;
	IIC_SCL_H;
}

void IIC_SDA_OUT(void)	//SDA配置为输出
{
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_INIT.GPIO_Pin=IIC_SDA ;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
}
void IIC_SDA_IN(void)	//SDA配置为输入
{
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_INIT.GPIO_Pin=IIC_SDA ;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
}

void IIC_Start(void)	//开始信号
{
	IIC_SDA_H;
	IIC_SCL_H;
	IIC_Delay();
	IIC_SDA_L;
	IIC_Delay();
	IIC_SDA_L;
}
void IIC_Stop(void )	//结束信号
{
	IIC_SDA_L;
	IIC_SCL_H;
	IIC_Delay();
	IIC_SDA_H;
	IIC_Delay();
	IIC_SCL_L;
	
	IIC_SCL_H;
	IIC_SDA_H;
}
bool IIC_ACK_Read(void)	//读取应答信号
{
	bool ack;
	IIC_SDA_H;
	IIC_SDA_IN();

	IIC_SCL_H;
	IIC_Delay();
	if(IIC_SDA_READ() == SET) ack=false;
	else ack=true;
	IIC_SCL_L;
	IIC_SDA_OUT();
	return ack;
}
void IIC_ACK_Send(bool ack)	//发送应答信号
{
	IIC_SCL_L;
	if(ack == true) 
		IIC_SDA_L;
	else IIC_SDA_H;
	IIC_SCL_H;
	IIC_Delay();
	IIC_SCL_L;
}
void IIC_Send_Byte(unsigned char byte)	//IIC发送一位数据
{
	unsigned char i;
	IIC_SCL_L;
	for(i=0;i<8;i++)
	{
		if(byte & 0x80 )	IIC_SDA_H;
		else IIC_SDA_L;
		IIC_SCL_H;
		IIC_Delay();
		IIC_SCL_L;
		IIC_Delay();
		byte<<=1;
	}
	IIC_ACK_Read();
}
unsigned char  IIC_Read_Byte(void)	//IIC读取一位数据
{
	unsigned char i,byte=0;
	IIC_SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL_H;
		byte<<=1;
		if(IIC_SDA_READ() == SET) byte |= 0x01;
		else byte &= 0xFE;
		IIC_SCL_L;
		IIC_Delay();
	}
	IIC_SDA_OUT();
	return byte;
}