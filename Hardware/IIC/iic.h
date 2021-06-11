#ifndef _IIC_H_
#define _IIC_H_
#include "stm32f10x.h"
#include "stdbool.h"
#define IIC_PORT	GPIOA
#define IIC_RCC 	  RCC_APB2Periph_GPIOA
#define IIC_SDA GPIO_Pin_5
#define IIC_SDA_H	GPIO_SetBits(IIC_PORT,IIC_SDA)
#define IIC_SDA_L	GPIO_ResetBits(IIC_PORT,IIC_SDA)
#define IIC_SDA_READ()	GPIO_ReadInputDataBit(GPIOA,IIC_SDA)

#define IIC_SCL GPIO_Pin_6
#define IIC_SCL_H	GPIO_SetBits(IIC_PORT,IIC_SCL)
#define IIC_SCL_L	GPIO_ResetBits(IIC_PORT,IIC_SCL)
#define IIC_SCL_READ()	GPIO_ReadInputDataBit(GPIOA,IIC_SCL)

void IIC_INIT(void);
void IIC_Start(void);
void IIC_Stop(void );
bool IIC_ACK_Read(void);
void IIC_ACK_Send(bool ack);
void IIC_Send_Byte(unsigned char byte);
unsigned char  IIC_Read_Byte(void);
#endif
