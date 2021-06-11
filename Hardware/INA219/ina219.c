#include "ina219.h"



#ifdef INA_INSIDE_IIC
void INA_IIC_Delay()
{unsigned char x;
	for(x=1;x>0;x--)
	{
		__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}
void INA_IIC_INIT(void)		//IIC初始化
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

void INA_IIC_SDA_OUT(void)	//SDA配置为输出
{
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_INIT.GPIO_Pin=IIC_SDA ;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
}
void INA_IIC_SDA_IN(void)	//SDA配置为输入
{
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_INIT.GPIO_Pin=IIC_SDA ;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
}

void INA_IIC_Start(void)	//开始信号
{
	IIC_SDA_H;
	IIC_SCL_H;
	INA_IIC_Delay();
	IIC_SDA_L;
	INA_IIC_Delay();
	IIC_SDA_L;
}
void INA_IIC_Stop(void )	//结束信号
{
	IIC_SDA_L;
	IIC_SCL_H;
	INA_IIC_Delay();
	IIC_SDA_H;
	INA_IIC_Delay();
	IIC_SCL_L;
	
	IIC_SCL_H;
	IIC_SDA_H;
}
bool INA_IIC_ACK_Read(void)	//读取应答信号
{
	bool ack;
	IIC_SDA_H;
	INA_IIC_SDA_IN();

	IIC_SCL_H;
	INA_IIC_Delay();
	if(IIC_SDA_READ() == SET) ack=false;
	else ack=true;
	IIC_SCL_L;
	INA_IIC_SDA_OUT();
	return ack;
}
void INA_IIC_ACK_Send(bool ack)	//发送应答信号
{
	IIC_SCL_L;
	if(ack == true) 
		IIC_SDA_L;
	else IIC_SDA_H;
	IIC_SCL_H;
	INA_IIC_Delay();
	IIC_SCL_L;
}
void INA_IIC_Send_Byte(unsigned char byte)	//IIC发送一位数据
{
	unsigned char i;
	IIC_SCL_L;
	for(i=0;i<8;i++)
	{
		if(byte & 0x80 )	IIC_SDA_H;
		else IIC_SDA_L;
		IIC_SCL_H;
		INA_IIC_Delay();
		IIC_SCL_L;
		INA_IIC_Delay();
		byte<<=1;
	}
	INA_IIC_ACK_Read();
}
unsigned char  INA_IIC_Read_Byte(void)	//IIC读取一位数据
{
	unsigned char i,byte=0;
	INA_IIC_SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL_H;
		byte<<=1;
		if(IIC_SDA_READ() == SET) byte |= 0x01;
		else byte &= 0xFE;
		IIC_SCL_L;
		INA_IIC_Delay();
	}
	INA_IIC_SDA_OUT();
	return byte;
}
#endif

void INA_Read_Byte_s(unsigned char reg,unsigned char *data)	//读两位数据
{
	INA_IIC_Start();
	INA_IIC_Send_Byte(INA219_ADDRESS);	//发送INA219地址s
	INA_IIC_Send_Byte(reg);
	
	INA_IIC_Start();
	INA_IIC_Send_Byte(INA219_ADDRESS+0x01);	//设置iic为读模式
	*data=INA_IIC_Read_Byte();
	data++;
	INA_IIC_ACK_Send(1);
	*data=INA_IIC_Read_Byte();
	INA_IIC_ACK_Send(0);
	INA_IIC_Stop();
}
void INA_REG_Write(unsigned char reg,unsigned int data)	//写寄存器		测试成功
{
	unsigned char data_temp[2];
	data_temp[0]=(unsigned char )(data>>8);
	data_temp[1]=(unsigned char )(data & 0xFF);
	INA_IIC_Start();
	INA_IIC_Send_Byte(INA219_ADDRESS);	//发送INA219地址
	INA_IIC_Send_Byte(reg);							//发送寄存器地址
	INA_IIC_Send_Byte(data_temp[0]);						//发送高8位数据
	data++;
	INA_IIC_Send_Byte(data_temp[1])	;					//发送低8位数据
	INA_IIC_Stop();
}

void INA_Init(void )	
{
	INA_IIC_INIT();
	INA_REG_Write(INA219_REG_CONFIG,INA219_CONFIG_value);
	INA_REG_Write(INA219_REG_CALIBRATION,INA_CAL);
}
unsigned int INA_GET_Voltage_MV(void)	//获取电压（单位：mv）
{
	unsigned char data_temp[2];
	INA_Read_Byte_s(0x02,data_temp);
	return (int)((((data_temp[0]<<8)+data_temp[1]) >> 3)*4);	//右移3为去掉：bit2，CNVR，OVF三位，再乘以 4MV (官方文档规定)，得到当前总线的电压值
}
unsigned int INA_GET_Current_MA(void)		//获取电流（单位：mA）
{
	unsigned char data_temp[2];
	INA_REG_Write(INA219_REG_CONFIG,INA219_CONFIG_value);
	INA_Read_Byte_s(INA219_REG_CURRENT,data_temp);
	return (int)((((data_temp[0]<<8)+data_temp[1]))*IAN_I_LSB);		//得到寄存器的值在乘以每位对应的值（IAN_I_LSB）得到实际的电流
}
unsigned int INA_GET_Power_MW(void)		//获取当前功率（单位：mw）
{
	unsigned char data_temp[2];
	INA_Read_Byte_s(INA219_REG_POWER,data_temp);
	return (int)(((data_temp[0]<<8)+data_temp[1])*INA_Power_LSB);	//得到寄存器的值在乘以每位对应的值（INA_Power_LSB）得到实际的功率
}
