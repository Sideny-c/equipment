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

//�������͵����PID�����ṹ��
extern PID_new PID_Round,R_Motor_PID,L_Motor_PID,MOTOR_PID, MOTOR1_PID,Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, Distance_PID,CurrentL_PID,CurrentR_PID;  
extern float Garage_in_pid[4],Realize_pid[4],circle_end_pid[4],circle_running_pid[4],circle_in_pid[4],Round_IN_PID[4],increase_pid_3s[4],increase_pid[4],Ramp_pid[4],Straight_pid[4],Garage_pid[4],circle_pid[4],Round_PID[4],MOTOR[4], MOTOR1[4],Angle[4], Ang_Vel[4],Turn_Data[4],Ang_Vel_L[4],Ang_Vel_R[4] ,Turn[5][4],Direct[4],fuzzy_Angle[4],fuzzy_Ang_Vel[4];

typedef struct PID_Error
{
	float SumError;	//����ۼ�	
	int32 CurrError;		//
	int32 LastError;	//Error[-1]
	int32 PrevError;	//Error[-2]	
	int32 LastPoint;	//Speed[-1]
} PID_Error;

void PID_Error_Init(PID_Error *sptr);	//PID����ʼ��
	
void PID_Init(void);	//PID��ʼ��


void PID_SetKp(float Kp);		//P����
void PID_SetKi(float Ki);		//I����
void PID_SetKd(float Kd);		//D����

void PID_SetKpid(float Kp,float Ki,float Kd);		//PID����


//����ʽPID
float PID_Increase(PID_Error *sptr,PID_ *pid,float NowPlace,float Point);

//λ��ʽPID
float PID_Realize(PID_Error *sptr,PID_ *pid,float NowPlace,float Point);

//ģ��PID
float fuzzy_PID_Realize(PID_new *sptr, float NowData, float Point,int road_Type);	



//KP
#define Kp_Base Ang_Vel[0]
#define Kd_Base Ang_Vel[2]

#define Kp_wave Ang_Vel[1]

//�����ٶ�����,�ٶ�Խ��kpԽ��
//#define speed_Auto_Ratio (0.85+0.15*exp(0.001*iError))    //�ٶ�Ϊ195ֵΪ1.0323   �ٶ�Ϊ160 ֵΪ1.02603 ���߲�ֵ��4.3 ������0.026����
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

//���õ�Kp
#define Kp_NB_1  -0.18
#define Kp_NM_1  -0.12
#define Kp_NS_1  -0.06
#define Kp_ZO_1  0
#define Kp_PS_1  0.06
#define Kp_PM_1  0.12
#define Kp_PB_1  0.18

//���õ�Kd
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


