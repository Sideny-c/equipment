#include "stm32f10x.h"   // Device header
#include "Delay.h"

void Key_InitB(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
}

void Key_InitA(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
}

uint8_t Key_getnum(void)
{
	uint8_t Key_Num=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)==0);
		Delay_ms(20);
		Key_Num=1;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)==0);
		Delay_ms(20);
		Key_Num=2;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)==0);
		Delay_ms(20);
		Key_Num=3;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==0)
	{
		Delay_ms(50);
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==0);
		Delay_ms(50);
		Key_Num=4;
	}
	return Key_Num;
}
