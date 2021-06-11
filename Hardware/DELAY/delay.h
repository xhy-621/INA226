#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f10x.h"

void delay_init(unsigned char sysclock);
void delay_us(unsigned int nus);
void delay_ms(unsigned int nms);

#endif
