#include "stm32f10x.h"   // Device header
#include "Delay.h"
#include "led.h"
#include "key.h"
#include "Buzzer.h"
#include "oled.h"
#include "menu.h"
#include "Timer.h"
#include "oled_new.h"
#include "EXTI.h"
#include "PWM.h"
#include "HC05.h"

extern int CRR;
extern u8 res;
int KeyNum,s,m,h;
int main(void)
{
	/*GPIO_InitTypeDef GPIO_InitStructure;
	I2C_Configuration();
		OLED_Init();
		Delay_ms(2000);
		
		OLED_Fill(0xFF);
		Delay_ms(2000);
		
		OLED_Fill(0x00);
		Delay_ms(2000);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_7);
	GPIO_SetBits(GPIOB,GPIO_Pin_6);*/
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)1);强制转换为BitAction枚举类型
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	//LED_Init();
	//Key_InitA();
	//Key_InitB();
	//OLED_Init();
	//Time_Init();
	//Exti_Init();
	//PWM_Init();
	lanya_Init();
	
	
	//Buzzer_Init();
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)1);强制转换为BitAction枚举类型
	while(1)
	{
		//USART_SendData(USART2,1);
		//Delay_ms(1000);
		//Menu_key_set();
	}
}

