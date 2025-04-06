#include "stm32f10x.h"                  // Device header


void ADC_init(void) 
{
	
	GPIO_InitTypeDef ADC_GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStruct;// �ṹ��Ķ�����Ҫ����RCC�Ŀ��������߱�����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	// ����ADC�����ʱ�Ӳ�����14MHz��������Ҫ��ʱ�ӽ�������
	// ADCʱ�ӷ�Ƶ��������Ƶ��ʱ�Ӵ�APB2�����������Ϊ72MHz��������Ҫ����Ƶ��12MHz��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	
	
	// ʹ�õ��� ADC1   ��ʼ�� PA0 	���ڲɼ�������������ģ�����
	ADC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;// ģ������
	ADC_GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   // ����������ģʽ�����Բ���Ҫ����Speed.
	GPIO_Init(GPIOA,&ADC_GPIO_InitStructure);
	
	// ��ʼ��ADC1
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//ɨ���ͨ�����ǵ�ͨ������ͨ����
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;// ����ת�����ǵ���ת����������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;// ���ݶ��뷽ʽ���Ҷ��롣
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//������������ģ����ⲿ������������Ƶ��������ת����
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//��ADC����Ϊ�������Ƕ�ģ��
	ADC_InitStruct.ADC_NbrOfChannel = 1;//ͨ������
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// ADC����������
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);
	// ADC1��ͨ��0����Ҫת������1������������55.5��
	
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);//��ADC���и�λ
	
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�ADC��λ���
	
	ADC_StartCalibration(ADC1);//У��ADC
	
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADCУ�����
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//����ADC���������

}


float ADC_GetValue(void)
{
	uint32_t Value = 0;
	float ret = 0.0;
	int i=0;
	for(i=0;i<30;i++)
	{
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//ת��������־�� ת����ɸñ�־������
	
		Value += ADC_GetConversionValue(ADC1);
		
	}

	Value /= 30; // ���������ƽ��ֵ
	ret = 30.303 * (3.3 - Value *(3.3/4095));
// �����ϱ�ʾģ��ת����Ĺ�ȣ�4095��ʾ12Ϊ��ADC�ľ��ȣ����ܱ䣩��
// 3.3��ʾ������������ܵ�ѹ
	return ret*1000;

}

/*����� ret�������СС��������һ��С�㷨,
���PA0���ӹ�����������PA0���Ż����е�ѹ�ģ�
����������������һ�����������յ����ߵ�Ӱ�����ı��˵������ֵ��
�Ӷ��ı��˵�ѹ������֪��PA0�ĵ�ѹ�Ϳ�����ȳ��˿̵Ĺ��ߵ�ǿ�ȣ�
������100��Ϊ������ǿ�������0��Ϊ���������������
PA0���������ܵ�����ѹ��3.3V������������Ҫ��3.3V �� 100�Ĺ�ǿ����һ����ȡ�
����100 ���� 3.3 ���� 30.303������3.3V�����һ����λ1��Ӧ�ľ���100�����30.303
��Ҳ��ΪʲôҪ����һ��30.303��ԭ��
*/
