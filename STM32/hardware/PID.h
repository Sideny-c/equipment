#ifndef __PID_H__
#define __PID_H__
#include "math.h"

#define KP 0
#define KI 1
#define KD 2
#define KT 3
#define KB 4
#define KF 5

#define RANK 7

//定义舵机和电机的PID参数结构体
extern PID_new PID_Round,R_Motor_PID,L_Motor_PID,MOTOR_PID, MOTOR1_PID,Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, Distance_PID,CurrentL_PID,CurrentR_PID;  
extern float Garage_in_pid[4],Realize_pid[4],circle_end_pid[4],circle_running_pid[4],circle_in_pid[4],Round_IN_PID[4],increase_pid_3s[4],increase_pid[4],Ramp_pid[4],Straight_pid[4],Garage_pid[4],circle_pid[4],Round_PID[4],MOTOR[4], MOTOR1[4],Angle[4], Ang_Vel[4],Turn_Data[4],Ang_Vel_L[4],Ang_Vel_R[4] ,Turn[5][4],Direct[4],fuzzy_Angle[4],fuzzy_Ang_Vel[4];

typedef struct PID_Error
{
	float SumError;	//误差累计	
	int32 CurrError;		//
	int32 LastError;	//Error[-1]
	int32 PrevError;	//Error[-2]	
	int32 LastPoint;	//Speed[-1]
} PID_Error;

void PID_Error_Init(PID_Error *sptr);	//PID误差初始化
	
void PID_Init(void);	//PID初始化


void PID_SetKp(float Kp);		//P设置
void PID_SetKi(float Ki);		//I设置
void PID_SetKd(float Kd);		//D设置

void PID_SetKpid(float Kp,float Ki,float Kd);		//PID设置


//增量式PID
float PID_Increase(PID_Error *sptr,PID_ *pid,float NowPlace,float Point);

//位置式PID
float PID_Realize(PID_Error *sptr,PID_ *pid,float NowPlace,float Point);

//模糊PID
float fuzzy_PID_Realize(PID_new *sptr, float NowData, float Point,int road_Type);	



//KP
#define Kp_Base Ang_Vel[0]
#define Kd_Base Ang_Vel[2]

#define Kp_wave Ang_Vel[1]

//进行速度修正,速度越大kp越大
//#define speed_Auto_Ratio (0.85+0.15*exp(0.001*iError))    //速度为195值为1.0323   速度为160 值为1.02603 两者差值×4.3 大概相差0.026左右
#define speed_Auto_Ratio  1
#define Kp_NB  Kp_Base*(1-3*Kp_wave)
#define Kp_NM  Kp_Base*(1-2*Kp_wave)
#define Kp_NS  Kp_Base*(1-1*Kp_wave)
#define Kp_ZO  Kp_Base
#define Kp_PS  Kp_Base*(1+1*Kp_wave)
#define Kp_PM  Kp_Base*(1+2*Kp_wave)
#define Kp_PB  Kp_Base*(1+3*Kp_wave)


//KD
#define Kd_NB  Kd_Base
#define Kd_NM  Kd_Base
#define Kd_NS  Kd_Base
#define Kd_ZO  Kd_Base
#define Kd_PS  Kd_Base
#define Kd_PM  Kd_Base
#define Kd_PB  Kd_Base

//备用的Kp
#define Kp_NB_1  -0.18
#define Kp_NM_1  -0.12
#define Kp_NS_1  -0.06
#define Kp_ZO_1  0
#define Kp_PS_1  0.06
#define Kp_PM_1  0.12
#define Kp_PB_1  0.18

//备用的Kd
#define Kd_NB_1  -0.15
#define Kd_NM_1  -0.1
#define Kd_NS_1  -0.05
#define Kd_ZO_1  0
#define Kd_PS_1  0.05
#define Kd_PM_1  0.1
#define Kd_PB_1  0.15

#define NB     -6
#define NM     -4
#define NS     -2
#define ZO      0
#define PS      2
#define PM      4
#define PB      6


#define emax    20
#define emin    0
#define ediffer emax-emin


#endif


