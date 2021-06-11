#include "stm32f10x.h"
#include "delay.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_iwdg.h"
#include "iic.h"
#include "ina219.h"
//#include "ina226.h"
#include "stdio.h"
#define LED_ON	GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define LED_OFF	GPIO_SetBits(GPIOC,GPIO_Pin_13)
void INA219_V_I_P_data(void);//INA219得到的所有数据
void LED_INIT(void );

unsigned int voltage_mv,Current_ma,Power_mw,R_mv;

float voltage_mv_float,Current_ma_float,Power_mw_float,R_mv_float;

int main(void)
{
//	unsigned char i;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	
	delay_init(72);
	LED_INIT();
//	IWDG_INIT();
//	TIM2_INIT();
//	INA_Init();
    INA226_Init();
	
	
	
		

while(1)
{	
	LED_ON;
	delay_ms(300);
	LED_OFF;
	delay_ms(300);
	voltage_mv=INA226_GET_Voltage_MV();
	voltage_mv_float=(float)voltage_mv*1.25;
	Current_ma=INA226_GET_Current_MA();
	Current_ma_float=(float)Current_ma_float*0.02;
}

			
}
void INA219_V_I_P_data(void)//INA219得到的所有数据
{
		voltage_mv=INA_GET_Voltage_MV();	//得到电压（mV）
		delay_ms(300);
		voltage_mv_float=(float)voltage_mv/1000;	//转换为浮点型电压（V）
		delay_ms(300);
		Current_ma=INA_GET_Current_MA();	//得到电流（mA）
		Current_ma_float=(float)Current_ma;	////得到浮点型电流（V）
		Power_mw=INA_GET_Power_MW();//得到功率（mW）
		Power_mw_float=(float)Power_mw;//得到浮点型功率（mW）

}

void LED_INIT(void )//c8t6 LED初始化
{	
	GPIO_InitTypeDef GPIO_INIT;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_INIT.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_INIT.GPIO_Pin=GPIO_Pin_13;
	GPIO_INIT.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_INIT);
}
