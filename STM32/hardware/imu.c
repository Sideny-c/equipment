#include "imu.h"
#include "mpu6050.h"
#include "Mahony.h"
#include "delay.h"
#include "stdio.h"

static IMU_Info imu;

#define OFFSET_COUNT 100
#define Buf_SIZE 10
int16_t MPU6500_FIFO[6][Buf_SIZE];
static uint8_t Wr_Index = 0; // FIFO

static float Pitch_offset;
static float Roll_offset;
static float Yaw_offset;

// put the data into the fifo
void MPU6500_NewVal(int16_t *buf, int16_t val)
{
	buf[Wr_Index] = val;
}

// calculate the average val in fifo
static int16_t MPU6500_GetAvg(int16_t *buf)
{
	int i;
	int32_t sum = 0;
	for (i = 0; i < Buf_SIZE; i++)
		sum += buf[i];
	sum = sum / Buf_SIZE;
	return (int16_t)sum;
}

// get mpu data after slide Average filtering
static void MPU6500_readGyro_Acc(int16_t *gyro, int16_t *acc)
{
	static short buf[6];
	static int16_t gx, gy, gz;
	static int16_t ax, ay, az;

	MPU_Get_Gyroscope(&buf[0], &buf[1], &buf[2]);

	MPU_Get_Accelerometer(&buf[3], &buf[4], &buf[5]);

	MPU6500_NewVal(&MPU6500_FIFO[0][0], buf[0]);
	MPU6500_NewVal(&MPU6500_FIFO[1][0], buf[1]);
	MPU6500_NewVal(&MPU6500_FIFO[2][0], buf[2]);

	MPU6500_NewVal(&MPU6500_FIFO[3][0], buf[3]);
	MPU6500_NewVal(&MPU6500_FIFO[4][0], buf[4]);
	MPU6500_NewVal(&MPU6500_FIFO[5][0], buf[5]);

	Wr_Index = (Wr_Index + 1) % Buf_SIZE;

	gx = MPU6500_GetAvg(&MPU6500_FIFO[0][0]);
	gy = MPU6500_GetAvg(&MPU6500_FIFO[1][0]);
	gz = MPU6500_GetAvg(&MPU6500_FIFO[2][0]);

	gyro[0] = gx - Roll_offset;
	gyro[1] = gy - Pitch_offset;
	gyro[2] = gz - Yaw_offset;

	ax = MPU6500_GetAvg(&MPU6500_FIFO[3][0]);
	ay = MPU6500_GetAvg(&MPU6500_FIFO[4][0]);
	az = MPU6500_GetAvg(&MPU6500_FIFO[5][0]);

	acc[0] = ax;
	acc[1] = ay;
	acc[2] = az;
}

// need to get the mpu6050 offset when start the mpu6050
static void MPU6500_Init_Offset(void)
{
	unsigned int i;
	int16_t temp[3], temp2[3];
	int32_t tempgx = 0, tempgy = 0, tempgz = 0;
	int32_t tempax = 0, tempay = 0, tempaz = 0;
	Pitch_offset = 0;
	Roll_offset = 0;
	Yaw_offset = 0;

	// wait the mpu
	for (i = 0; i < 100; i++)
	{
		delay_ms(10);
		MPU6500_readGyro_Acc(temp, temp2);
	}

	// read the mpu data for calculate the offset
	for (i = 0; i < OFFSET_COUNT; i++)
	{
		delay_ms(10);
		MPU6500_readGyro_Acc(temp, temp2);
		tempgx += temp[0];
		tempgy += temp[1];
		tempgz += temp[2];

		tempax += temp2[0];
		tempay += temp2[1];
		tempaz += temp2[2];
	}

	Pitch_offset = tempgy / OFFSET_COUNT;
	Roll_offset = tempgx / OFFSET_COUNT;
	Yaw_offset = tempgz / OFFSET_COUNT;

	printf("Pitch_offset = %f,Roll_offset = %f,Yaw_offset = %f\r\n", Pitch_offset, Roll_offset, Yaw_offset);
}

uint8_t IMU_Init(void)
{
	uint8_t err;
	err = MPU_Init();
	if (err != 0)
	{
		return err;
	}
	delay_ms(100);
	MPU6500_Init_Offset();

	delay_ms(100);
	return err;
}

// Get Imu values.values[0-2]:gyro data,values[3-5]:acc data
static void Get_IMU_Values(float *values)
{
	int i = 0;
	int16_t gyro[3], acc[3];

	MPU6500_readGyro_Acc(&gyro[0], &acc[0]);

	for (; i < 3; i++)
	{
		values[i] = ((float)gyro[i]) / 16.4f; // gyro range: +-2000; adc accuracy 16 bits: 2^16=65536; 65536/4000=16.4; so  1^-> 16.4
		values[3 + i] = (float)acc[i];
	}
}

#define PI 3.14159265358979f

// asin
static float safe_asin(float v)
{
	if (isnan(v))
	{
		return 0.0f;
	}
	if (v >= 1.0f)
	{
		return PI / 2;
	}
	if (v <= -1.0f)
	{
		return -PI / 2;
	}
	return asin(v);
}

void IMU_Update(void)
{
	static float q[4];
	float Values[6];
	Get_IMU_Values(Values);

	// change angle to radian,the calculate the imu with Mahony
	MahonyAHRSupdateIMU(Values[0] * PI / 180, Values[1] * PI / 180, Values[2] * PI / 180,
						Values[3], Values[4], Values[5]);

	// save Quaternion
	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;

	imu.ax = Values[3];
	imu.ay = Values[4];
	imu.az = Values[5];

	imu.Pitch_v = Values[0];
	imu.Roll_v = Values[1];
	imu.Yaw_v = Values[2];

	// calculate the imu angle with quaternion
	imu.Roll = (atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), 1 - 2.0f * (q[1] * q[1] + q[2] * q[2]))) * 180 / PI;
	imu.Pitch = -safe_asin(2.0f * (q[0] * q[2] - q[1] * q[3])) * 180 / PI;
	imu.Yaw = -atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 180 / PI; // yaw
}

IMU_Info *IMU_GetInfo(void)
{
	return &imu;
}
