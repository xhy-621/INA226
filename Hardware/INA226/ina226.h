#ifndef _INA226_H_
#define _INA226_H_
#include "stm32f10x.h"
#include "stdbool.h"
#define READ_ADDR                  0x81	 //A1=GND，A2=GND // R=1, W=0
#define WRITE_ADDR                 0x80

#define Config_Reg                 0x00
#define Shunt_V_Reg                0x01
#define Bus_V_Reg                  (u8)0x02
#define Power_Reg                  0x03
#define Current_Reg                0x04
#define Calib_Reg                  0x05
#define Mask_En_Reg                0x06
#define Alert_Reg                  0x07
#define Man_ID_Reg                 0xFE  //0x5449
#define ID_Reg                     0xFF  //0x2260

#define INA_INSIDE_IIC	//使用自带的io模拟iic
#ifdef INA_INSIDE_IIC	
		#define IIC_PORT	GPIOA																				//IIC IO端口
		#define IIC_RCC 	RCC_APB2Periph_GPIOA												//端口时钟
		#define IIC_SDA GPIO_Pin_5																		//SDA引脚
		#define IIC_SDA_H	GPIO_SetBits(IIC_PORT,IIC_SDA)							//拉高SDA
		#define IIC_SDA_L	GPIO_ResetBits(IIC_PORT,IIC_SDA)						//拉低SDA
		#define IIC_SDA_READ()	GPIO_ReadInputDataBit(GPIOA,IIC_SDA)	//读取SDA

		#define IIC_SCL GPIO_Pin_6																		//SCL引脚
		#define IIC_SCL_H	GPIO_SetBits(IIC_PORT,IIC_SCL)							//拉高SCL
		#define IIC_SCL_L	GPIO_ResetBits(IIC_PORT,IIC_SCL)						//拉低SCL
#endif

void INA226_Read2Byte(unsigned char reg,unsigned char *data);
void INA226_Write2Byte(unsigned int data,unsigned char reg);
void INA226_Init(void);
unsigned int INA226_GET_Voltage_MV(void);	//获取电压（单位：mv）
unsigned int INA226_GET_Current_MA(void);
unsigned int INA226_GET_Shunt_Reg_MA(void);	
#endif
