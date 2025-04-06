#include "myiic.h"
#include "delay.h"
#include "Device/Include/stm32f10x.h"   // Device header

#ifndef IMU_USE_HAL

void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	// fix RCC_AHB1Periph_GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_Pin;
	GPIO_Init(IIC_SCL_GPIO_Port, &GPIO_InitStructure);

	// fix RCC_AHB1Periph_GPIOB
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_Pin;
	GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStructure);

	IIC_SCL(1);
	IIC_SDA(1);
}
#else
// change it
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;

	// fix __HAL_RCC_GPIOB_CLK_ENABLE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_Initure.Pin = IIC_SCL_Pin;
	HAL_GPIO_Init(IIC_SCL_GPIO_Port, &GPIO_Initure);

	// fix __HAL_RCC_GPIOB_CLK_ENABLE
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_Initure.Pin = IIC_SDA_Pin;
	HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_Initure);

	IIC_SCL(1);
	IIC_SDA(1);
}

#endif
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA(1);
	IIC_SCL(1);
	Delay_us(4);
	IIC_SDA(0); // START:when CLK is high,DATA change form high to low
	Delay_us(4);
	IIC_SCL(0);
}

void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL(0);
	IIC_SDA(0);
	Delay_us(4);
	IIC_SCL(1);
	IIC_SDA(1);
	Delay_us(4);
}

u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	SDA_IN();
	IIC_SDA(1);
	Delay_us(1);
	IIC_SCL(1);
	Delay_us(1);
	while (READ_SDA())
	{
		ucErrTime++;
		if (ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);
	return 0;
}

void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(0);
	Delay_us(2);
	IIC_SCL(1);
	Delay_us(2);
	IIC_SCL(0);
}

void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_OUT();
	IIC_SDA(1);
	Delay_us(2);
	IIC_SCL(1);
	Delay_us(2);
	IIC_SCL(0);
}

void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	IIC_SCL(0);
	for (t = 0; t < 8; t++)
	{
		IIC_SDA((txd & 0x80) >> 7);
		txd <<= 1;
		Delay_us(2);
		IIC_SCL(1);
		Delay_us(2);
		IIC_SCL(0);
		Delay_us(2);
	}
}

u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	SDA_IN();
	for (i = 0; i < 8; i++)
	{
		IIC_SCL(0);
		Delay_us(2);
		IIC_SCL(1);
		receive <<= 1;
		if (READ_SDA())
			receive++;
		delay_us(1);
	}
	if (!ack)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}
