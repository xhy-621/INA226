#include "ina226.h"
#include "delay.h"
#ifdef INA_INSIDE_IIC

void INA_IIC_Delay()
{unsigned char x;
	for(x=1;x>0;x--)
	{
		__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}
void INA_IIC_INIT(void)		//IIC��ʼ��
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

void INA_IIC_SDA_OUT(void)	//SDA����Ϊ���
{
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_INIT.GPIO_Pin=IIC_SDA ;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
}
void INA_IIC_SDA_IN(void)	//SDA����Ϊ����
{
	GPIO_InitTypeDef GPIO_INIT;
	GPIO_INIT.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_INIT.GPIO_Pin=IIC_SDA ;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT,&GPIO_INIT);
}

void INA_IIC_Start(void)	//��ʼ�ź�
{
	IIC_SDA_H;
	IIC_SCL_H;
	INA_IIC_Delay();
	IIC_SDA_L;
	INA_IIC_Delay();
	IIC_SDA_L;
}
void INA_IIC_Stop(void )	//�����ź�
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
bool INA_IIC_ACK_Read(void)	//��ȡӦ���ź�
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
void INA_IIC_ACK_Send(bool ack)	//����Ӧ���ź�
{
	IIC_SCL_L;
	if(ack == true) 
		IIC_SDA_L;
	else IIC_SDA_H;
	IIC_SCL_H;
	INA_IIC_Delay();
	IIC_SCL_L;
}
void INA_IIC_Send_Byte(unsigned char byte)	//IIC����һλ����
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
unsigned char  INA_IIC_Read_Byte(void)	//IIC��ȡһλ����
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

void INA226_Read2Byte(unsigned char reg,unsigned char *data)
{
		INA_IIC_Start();
	INA_IIC_Send_Byte(WRITE_ADDR);	//����INA219��ַs
	INA_IIC_Send_Byte(reg);
	
	INA_IIC_Start();
	INA_IIC_Send_Byte(WRITE_ADDR+0x01);	//����iicΪ��ģʽ
	*data=INA_IIC_Read_Byte();
	data++;
	INA_IIC_ACK_Send(1);
	*data=INA_IIC_Read_Byte();
	INA_IIC_ACK_Send(0);
	INA_IIC_Stop();
}

void INA226_Write2Byte(unsigned int data,unsigned char reg)
{        
	unsigned char data_temp[2];
	data_temp[0]=(unsigned char )(data>>8);
	data_temp[1]=(unsigned char )(data & 0xFF);
	INA_IIC_Start();
	INA_IIC_Send_Byte(WRITE_ADDR);	//����INA226��ַ
	INA_IIC_Send_Byte(reg);				//���ͼĴ�����ַ
	INA_IIC_Send_Byte(data_temp[0]);	//���͸�8λ����
	data++;
	INA_IIC_Send_Byte(data_temp[1])	;	//���͵�8λ����
	INA_IIC_Stop();
}

void INA226_Init(void)
{
	INA_IIC_INIT();
	INA226_Write2Byte(0x4527, Config_Reg);//0100_010_100_100_111 //16��ƽ��,1.1ms,1.1ms,��������������ѹ�����ߵ�ѹ
	INA226_Write2Byte(0x0A00, Calib_Reg);
}
unsigned int INA226_GET_Voltage_MV(void)	//��ȡ��ѹ����λ��mv��
{
	unsigned char data_temp[2];
	INA226_Read2Byte(Bus_V_Reg,data_temp);
	return (int)((((data_temp[0]<<8)+data_temp[1])));	//�õ���ǰ���ߵĵ�ѹֵ
}
unsigned int INA226_GET_Shunt_Reg_MA(void)		//��ȡ���������ϵĵ�ѹ����λ��mA��
{
	unsigned char data_temp[2];

	INA_Read_Byte_s(Shunt_V_Reg,data_temp);
	return (int)((((data_temp[0]<<8)+data_temp[1])));		//�õ��Ĵ�����ֵ�ڳ���ÿλ��Ӧ��ֵ��IAN_I_LSB���õ�ʵ�ʵĵ���
}
unsigned int INA226_GET_Current_MA(void)		//��ȡ��������λ��mA��
{
	unsigned char data_temp[2];

	INA_Read_Byte_s(Current_Reg,data_temp);
	return (int)((((data_temp[0]<<8)+data_temp[1])));		//�õ��Ĵ�����ֵ�ڳ���ÿλ��Ӧ��ֵ��IAN_I_LSB���õ�ʵ�ʵĵ���
}