#include "stm32f10x.h"                  // Device header


void ADC_init(void) 
{
	
	GPIO_InitTypeDef ADC_GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStruct;// 结构体的定义需要先于RCC的开启，否者报警告
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	// 由于ADC的最大时钟不超过14MHz，所以需要把时钟降下来。
	// ADC时钟分频器；六分频；时钟从APB2总线来，最大为72MHz，所以需要六分频后12MHz。
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	
	
	// 使用的是 ADC1   初始化 PA0 	用于采集光敏传感器的模拟输出
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;// 模拟输入
	ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   // 由于是输入模式，所以不需要配置Speed.
	GPIO_Init(GPIOA,&ADC_GPIO_InitStructure);
	
	// 初始化ADC1
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//扫描多通道还是单通道？单通道。
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;// 连续转换还是单次转换？连续。
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;// 数据对齐方式。右对齐。
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//定义用于启动模拟的外部触发器到常规频道的数字转换。
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//将ADC配置为独立还是多模？
	ADC_InitStruct.ADC_NbrOfChannel = 1;//通道数量
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// ADC规则组配置
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	// ADC1，通道0，需要转换数是1，采样周期是55.5。
	
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);//对ADC进行复位
	
	while(ADC_GetResetCalibrationStatus(ADC1));//等待ADC复位完成
	
	ADC_StartCalibration(ADC1);//校验ADC
	
	while(ADC_GetCalibrationStatus(ADC1));//等待ADC校验完成
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//开启ADC的软件触发

}


float ADC_GetValue(void)
{
	uint32_t Value = 0;
	float ret = 0.0;
	int i=0;
	for(i=0;i<30;i++)
	{
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//转换结束标志， 转换完成该标志就清零
	
		Value += ADC_GetConversionValue(ADC1);
		
	}

	Value /= 30; // 这里求的是平均值
	ret = 30.303 * (3.3 - Value *(3.3/4095));
// 整体上表示模数转换后的光度；4095表示12为的ADC的精度（不能变）；
// 3.3表示这个引脚最大承受电压
	return ret*1000;

}

/*这里的 ret的运算就小小的运用了一下小算法,
如果PA0不接光敏传感器，PA0引脚还是有电压的，
（光敏传感器就是一个光敏电阻收到光线的影响来改变了电阻的阻值，
从而改变了电压，所以知道PA0的电压就可以类比出此刻的光线的强度）
我们以100作为光线最强的情况，0作为光线最弱的情况。
PA0引脚所承受的最大电压是3.3V，所以我们需要将3.3V 与 100的光强进行一个类比。
所以100 除以 3.3 等于 30.303。所以3.3V里面的一个单位1对应的就是100里面的30.303
这也是为什么要乘以一个30.303的原因。
*/
