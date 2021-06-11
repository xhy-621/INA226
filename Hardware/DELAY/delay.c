#include "delay.h"

unsigned int t_us,t_ms;
void delay_init(unsigned char sysclock)
{
	SysTick->CTRL &=0xfffffffb;
	t_us=sysclock/8;
	t_ms=t_us*1000;
}
void delay_us(unsigned int nus)
{
	unsigned int temp;
	SysTick->LOAD = t_us*nus;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x01;
	do
	{
		temp=SysTick->CTRL;
	}while(temp & 0x01 && !(temp & 0x10000));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0x00;
}

void delay_ms(unsigned int nms)
{
	unsigned int temp;
	SysTick->LOAD = t_ms*nms;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x01;
	do
	{
		temp=SysTick->CTRL;
	}while(temp & 0x01 && !(temp & 0x10000));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0x00;
}
