#ifndef __MPU6050_H
#define __MPU6050_H
#include "myiic.h"

#define MPU_SAMPLE_RATE_REG 0X19
#define MPU_CFG_REG 0X1A
#define MPU_GYRO_CFG_REG 0X1B
#define MPU_ACCEL_CFG_REG 0X1C
#define MPU_TEMP_OUTH_REG 0X41
#define MPU_GYRO_XOUTH_REG 0X43
#define MPU_ACCEL_XOUTH_REG 0X3B
#define MPU_PWR_MGMT1_REG 0X6B
#define MPU_INT_EN_REG 0X38
#define MPU_USER_CTRL_REG 0X6A
#define MPU_FIFO_EN_REG 0X23
#define MPU_INTBP_CFG_REG 0X37
#define MPU_DEVICE_ID_REG 0X75
#define MPU_PWR_MGMT1_REG 0X6B
#define MPU_PWR_MGMT2_REG 0X6C

// if AD0->GND,MPU_ADDR=0x68.else 0x69
#define MPU_ADDR 0X68

uint8_t MPU_Init(void);
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data);
uint8_t MPU_Read_Byte(uint8_t reg);

short MPU_Get_Temperature(void);
uint8_t MPU_Get_Gyroscope(short *gx, short *gy, short *gz);
uint8_t MPU_Get_Accelerometer(short *ax, short *ay, short *az);

void MPU6500_readGyro_Acc(int16_t *gyro, int16_t *acc);
void MPU6500_Init_Offset(void);

#endif