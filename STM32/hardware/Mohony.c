#include "stm32f10x.h"   // Device header
#include "math.h"
#include "MPU6050.h"


#define Acc_Gain 0.0001220f			//���ٶ�ת����λ(��ʼ�����ٶȼ�����+-4g������mpu6050�����ݼĴ�����16λ�ģ�LSBa = 2*4 / 65535.0)
#define Gyro_Gain 0.0609756f		//���ٶ�ת��Ϊ�Ƕ�(LSBg = 2000*2 / 65535)
#define Gyro_Gr 0.0174f			//���ٶ�ת���ɻ���(3.1415 / 180 * LSBg)
#define G 9.80665f					// m/s^2

extern short ax,ay,az;
extern short gx,gy,gz;


static float invSqrt(float x) 		//���ټ��� 1/Sqrt(x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}


void Prepare_Data(void)
{
	MPU6050_ReadData(&ax, &ay, &az, &gx, &gy, &gz);		//��ȡ���ٶȼ�ԭʼֵ
								//��ȡ���ٶ�ԭʼֵ
	//float recipNorm;
	//�����ٶ�ԭʼADֵת��Ϊm/s^2
	/*recipNorm = invSqrt(ax*ax+ay*ay+az*az);
	ax*=recipNorm;
	ay*=recipNorm;
	az*=recipNorm;*/
	ax = (float)ax * Acc_Gain * G;
	ay = (float)ay * Acc_Gain * G;
	az = (float)az * Acc_Gain * G;
	
	//��������ADֵת��Ϊ ����/s
	gx = (float)gx * Gyro_Gr;
	gy = (float)gy * Gyro_Gr;
	gz = (float)gz * Gyro_Gr;
}



#define Kp 1.50f
#define Ki 0.01
#define halfT 0.0025f						//�������ڵ�һ�룬��λs

extern float Yaw,Pitch,Roll;				
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;		//��Ԫ��
float exInt = 0, eyInt = 0, ezInt = 0;		//������������ۼƻ���


void Imu_Update(float gx,float gy,float gz,float ax,float ay,float az)
{

	float vx,vy,vz;							//ʵ���������ٶ�
	float ex,ey,ez;							//�����������
	float norm;
	
 	float q0q0 = q0*q0;
 	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
 	float q1q2 = q1*q2;
 	float q1q3 = q1*q3;
	float q2q2 = q2*q2;
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;
	
	//if(ax*ay*az == 0)
		//return;
	
	//���ٶȼƲ�������������(��������ϵ)
	norm = invSqrt(ax*ax + ay*ay + az*az);			//֮ǰ����д��invSqrt(ax*ax + ay+ay + az*az)�Ǵ���ģ������޸Ĺ�����
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm;
	
	//��Ԫ���Ƴ���ʵ����������(��������ϵ)
	vx = 2*(q1q3 - q0q2);												
  	vy = 2*(q0q1 + q2q3);
  	vz = q0q0 - q1q1 - q2q2 + q3q3;
	
	//������
	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);
	
	//���������Ϊ���ٶ�
	exInt = exInt + ex * Ki*0.005;
	eyInt = eyInt + ey * Ki*0.005;
	ezInt = ezInt + ez * Ki*0.005;
	
	//���ٶȲ���
	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;
	
	//������Ԫ��
  	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;	
	
	//��λ����Ԫ��
  	norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  	q0 = q0 * norm;
  	q1 = q1 * norm;
  	q2 = q2 * norm;  
  	q3 = q3 * norm;
	
	//��Ԫ������ŷ����
	Yaw = atan2(2.0 * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3)* 57.3;
	Pitch = -asin(2.0 * (q1q3 - q0q2))* 57.3;
	Roll = atan2(2.0 * q2q3 + 2 * q0q1, q0q0 - q1q1 - q2q2 + q3q3)* 57.3;
}
