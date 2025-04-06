#include "stm32f10x.h"   // Device header
#include "Delay.h"
#include "key.h"
#include "oled.h"
#include "led.h"
#include "EXTI.h"
#include "PWM.h"
#include "MPU6050.h"
#include "Mohony.h"
#include "ADC.h"

int i=1,s1,m1,h1,CRR1;
float Yaw,Pitch,Roll;
extern u8 res;
extern int s,m,h,CRR;
short ax,ay,az,gx,gy,gz;
int Key_Num;
void (*current_operation_index)();


void Love()
{
	i=1;
	OLED_ShowString(2,7,"Nothing");
}
void Start()
{
/*	int i;
	OLED_SetCursor(2,0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x01);
			OLED_WriteData(0x0f);
		}*/
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
	OLED_ShowString(2,7,"Start");
}
void Surprise()
{
	OLED_ShowString(1,1,"Surprise");
	OLED_ShowString(2,1,"TimeSet");
	OLED_ShowString(3,1,"PWM->LED");
	OLED_ShowString(1,10,"<<");
	
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void Time()
{
	TIM_Cmd(TIM2,ENABLE);
	s1=m1=h1=0;
	OLED_ShowString(1,1,"Surprise");
	OLED_ShowString(2,1,"TimeSet");
	OLED_ShowString(3,1,"PWM->LED");
	OLED_ShowString(2,10,"<<");
	
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void Time_change_s()
{
	TIM_Cmd(TIM2,DISABLE);
	Exti_Init();
	s1=1,h1=m1=0;
	OLED_ShowNum(2,11,s,2);
	OLED_ShowString(2,7,":");
	OLED_ShowNum(2,8,m,2);
	OLED_ShowString(2,10,":");
	OLED_ShowNum(2,5,h,2);
	OLED_ShowString(3,11,"*");
}
void Time_change_m()
{
	Exti_Init();
	m1=1,s1=h1=0;
	OLED_ShowNum(2,11,s,2);
	OLED_ShowString(2,7,":");
	OLED_ShowNum(2,8,m,2);
	OLED_ShowString(2,10,":");
	OLED_ShowNum(2,5,h,2);
	OLED_ShowString(3,8,"*");
}
void Time_change_h()
{
	Exti_Init();
	h1=1,s1=m1=0;
	OLED_ShowNum(2,11,s,2);
	OLED_ShowString(2,7,":");
	OLED_ShowNum(2,8,m,2);
	OLED_ShowString(2,10,":");
	OLED_ShowNum(2,5,h,2);
	OLED_ShowString(3,5,"*");
}

void Setting()
{
	CRR1=0;
	OLED_ShowString(1,1,"Surprise");
	OLED_ShowString(2,1,"TimeSet");
	OLED_ShowString(3,1,"PWM->LED");
	OLED_ShowString(3,10,"<<");
	
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void MPU()
{
	i=1;
	OLED_ShowString(1,1,"MPU6050");
	OLED_ShowString(2,1,"BlueTooth");
	OLED_ShowString(1,10,"<<");
	
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void BlueTooth()
{
	res=0;
	OLED_ShowString(1,1,"MPU6050");
	OLED_ShowString(2,1,"BlueTooth");
	OLED_ShowString(2,10,"<<");
	
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);

}

void blue()
{
	if(res=='1')
	{
		LED2_Turn();
	}else if(res=='2')
	{
		LED1_Turn();
	}
	OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void LED()
{
	CRR1=1;
	Key_Num=Key_getnum();
	PWM_SetCompare1(CRR);
	OLED_ShowNum(1,6,CRR,3);
	OLED_ShowString(1,1,"PWM:");
	
	/*if(Key_Num==1)
	{
		LED2_Turn();
		OLED_ShowString(1,1,"Red  ");
	}
	if(Key_Num==2)
	{
		LED1_Turn();
		OLED_ShowString(1,1,"White");
	}*/
}


void Angle()
{
	i=1;
	OLED_ShowString(1,1,"Angle");
	OLED_ShowString(2,1,"Accelerat");
	OLED_ShowString(1,11,"<<");
		OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void Acc()
{
	i=1;
	OLED_ShowString(1,1,"Angle");
	OLED_ShowString(2,1,"Accelerat");
	OLED_ShowString(2,11,"<<");
		OLED_ShowNum(4,15,s,2);
	OLED_ShowString(4,11,":");
	OLED_ShowNum(4,12,m,2);
	OLED_ShowString(4,14,":");
	OLED_ShowNum(4,9,h,2);
}

void Angle_number()
{

	MPU6050_Init();

	OLED_ShowString(1,1,"Yaw:");
	OLED_ShowString(2,1,"Pitch:");
	OLED_ShowString(3,1,"Roll:");
	while (i==1)
	{
		MPU6050_ReadData(&ax, &ay, &az, &gx, &gy, &gz);
		Prepare_Data();
		Imu_Update(gx,gy,gz,ax,ay,az);
		OLED_ShowSignedNum(1, 7, Yaw/2, 3);
		OLED_ShowSignedNum(2, 7, Pitch, 3);
		OLED_ShowSignedNum(3, 7, Roll, 3);
		if((Key_Num=Key_getnum())==4)
			i--;
	}
}

void Acc_number()
{
	MPU6050_Init();
	OLED_ShowString(1,1,"Gx:");
	OLED_ShowString(2,1,"Gy:");
	OLED_ShowString(3,1,"Gz:");
	while (i==1)
	{
		MPU6050_ReadData(&ax, &ay, &az, &gx, &gy, &gz);
		Prepare_Data();
		Imu_Update(gx,gy,gz,ax,ay,az);
		OLED_ShowSignedNum(1, 5, gx, 3);
		OLED_ShowSignedNum(2, 5, gy, 3);
		OLED_ShowSignedNum(3, 5, gz, 3);
		if((Key_Num=Key_getnum())==4)
			i--;
	}

}
typedef struct
{
	unsigned char current;
	unsigned char next;
	unsigned char enter;
	unsigned char back;
	void(*current_operation)(void);
}Menu_table;

uint8_t func_index = 0;

Menu_table table[100]=
{
	{0,0,1,0,(*Start)},
	
		{1,2,6,0,(*Surprise)},
		{2,3,7,0,(*Time)},
		{3,4,10,0,(*Setting)},
		{4,5,11,0,(*MPU)},
		{5,1,13,0,(*BlueTooth)},
		
			{6,6,6,1,(*Love)},
			{7,8,7,2,(*Time_change_s)},
			{8,9,8,2,(*Time_change_m)},
			{9,7,9,2,(*Time_change_h)},
			{10,10,10,3,(*LED)},
			{11,12,14,4,(*Angle)},
			{12,11,15,4,(*Acc)},
			{13,13,13,5,(*blue)},
				
				{14,14,14,11,(*Angle_number)},
				{15,15,15,12,(*Acc_number)}
};

void  Menu_key_set(void)
{
	switch(Key_Num=Key_getnum())
	{
		//case 1: func_index=table[func_index].current;OLED_Clear();break;
		case 1: func_index=table[func_index].next;OLED_Clear();break;
		case 2: func_index=table[func_index].enter;OLED_Clear();break;
		case 3: func_index=table[func_index].back;OLED_Clear();break;
		default:break;
	}
  current_operation_index=table[func_index].current_operation;	
  (*current_operation_index)();
}
