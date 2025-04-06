#include "stm32f10x.h"
#include "math"
#include "PID.h"

#define int32 int32_t

//定义舵机和电机的PID参数结构体
PID_new PID_Round,R_Motor_PID,L_Motor_PID,MOTOR_PID,MOTOR1_PID, Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, Distance_PID,CurrentL_PID,CurrentR_PID;

/***********************************************************************************************************
											增量式及位置式PID
************************************************************************************************************/

typedef struct PID_{
    
    float P;                          /* 比例系数     */
    float I;                          /* 积分系数     */
    float D;                          /* 微分系数     */
	
	float   limit;                     /* 积分上限     */ 
    float   Point;                     /* 目标值       */
    long    SumError;                  /* 累计误差     */
    float   LastError;                 /* Error[-1]    */
    float   PrevError;                 /* Error[-2]    */
                                                                        
}PID_;

PID_ __PID;
PID_ *PID = &__PID;

/***********************************************************************************************************
*FunctionName    : PID_Error_Init()
*Description     : PID误差初始化
*EntryParameter  : PID误差结构体指针
*ReturnVaule     : None
************************************************************************************************************/

void PID_Error_Init(PID_Error *sptr)
{
	sptr->SumError  = 0;
	sptr->LastError = 0;	//[Error-1]
	sptr->PrevError = 0;	//[Error-2]
	sptr->LastPoint = 0;
}

/*********************************************************************************************************
** Function name	:   PID_Init
** Descriptions		:   PID所有参数清零
** input parameters	:   *LPID:PID结构体指针                         
** output parameters:   无
** Returned value	:   无
*********************************************************************************************************/ 

void PID_Init(void) 
{
    PID->Point     = 0;

    PID->P         = 0;
    PID->I         = 0;
    PID->D         = 0;
    
    PID->SumError  = 0;    
    PID->LastError = 0;
    PID->PrevError = 0;
}

/********************************************************************************************************
** Function name	:   PID_SetKp
** Descriptions		:   设置P值
** input parameters	:   Kp                       
** output parameters:   无
** Returned value	:   无
*********************************************************************************************************/ 

void PID_SetKp(float Kp) 
{
    PID->P     = Kp; 
}

/*********************************************************************************************************
** Function name	:   PID_SetKi
** Descriptions		:   设置I值
** input parameters	:   Ki                         
** output parameters:   无
** Returned value	:   无
*********************************************************************************************************/ 

void PID_SetKi(float Ki) 
{
    PID->I     = Ki;
}

/*********************************************************************************************************
** Function name	:   PID_SetKd
** Descriptions		:   设置D值
** input parameters	:   Kd                         
** output parameters:   无
** Returned value	:   无
*********************************************************************************************************/ 

void PID_SetKd(float Kd) 
{
    PID->D     = Kd;
}

/*********************************************************************************************************
** Function name	:   PID_SetKpid
** Descriptions		:   设置PID值
** input parameters	:   Kd                         
** output parameters:   无
** Returned value	:   无
*********************************************************************************************************/ 

void PID_SetKpid(float Kp,float Ki,float Kd) 
{
    PID->P     = Kp;
    PID->I     = Ki;
    PID->D     = Kd;
}

/*********************************************************************************************************
** Function name	:  PID_Increase
** Descriptions		:  增量式PID控制
** input parameters	:  PID结构体指针，PID参数，当前位置，预期位置                       
** output parameters:  无
** Returned value	:  无
*********************************************************************************************************/ 

float PID_Increase(PID_Error *sptr,PID_ *pid,float NowPlace,float Point)
{
	static float NowError;	//当前误差
	static int32 Inc;		//最后得出的实际增量
	
	NowError = Point - NowPlace; 	//计算当前误差
	
	Inc = pid->P * (NowError - sptr->LastError) 						//比例P
		 +pid->I * NowError												//积分I
		 +pid->D * (NowError - 2 * sptr->LastError + sptr->PrevError;	//微分D
	
	sptr->PrevError = sptr->LastError;		//更新[Error-2]
	sptr->LastError = NowError;				//更新[Error-1]
	
	return Inc;
}

/*********************************************************************************************************
** Function name	:  PID_Realize
** Descriptions		:  位置式PID控制
** input parameters	:  PID结构体指针，PID参数，当前位置，预期位置                       
** output parameters:  无
** Returned value	:  无
*********************************************************************************************************/ 

float PID_Realize(PID_Error *sptr,PID_ *pid,float NowPlace,float Point)
{
	static float NowError;		//当前误差
	static int32 Realize;		//实际输出
	
	NowError = Point - NowPlace;	//计算当前误差
	
	sptr->SumError += pid->I * NowError; 	//误差积分
	sptr->SumError = sptr->SumError > pid->limit ? pid->limit : sptr->SumError;
	sptr->SumError = sptr->SumError < -pid->limit ? -pid->limit : sptr->SumError; 	//积分限幅
	
	Realize =  pid->P * NowError						//比例P
			 + sptr->SumError							//积分I
			 + pid->D * (NowError - sptr->LastError);	//微分D	
	
	sptr->LastError = NowError;				//更新[Error-1]
	
	return Realize;		//返回实际值
}


/*********************************************************************************************************
											  模糊PID
*********************************************************************************************************/ 


float Kp_fuzzy_1[RANK][RANK]=
                   {
                   {Kp_PB_1,Kp_PB_1,Kp_PM_1,Kp_PM_1,Kp_PS_1,Kp_ZO_1,Kp_ZO_1},//NB
                   {Kp_PB_1,Kp_PB_1,Kp_PM_1,Kp_PS_1,Kp_PS_1,Kp_ZO_1,Kp_NS_1},//NM
                   {Kp_PM_1,Kp_PM_1,Kp_PM_1,Kp_PS_1,Kp_ZO_1,Kp_NS_1,Kp_NS_1},//NS
                   {Kp_PM_1,Kp_PM_1,Kp_PS_1,Kp_ZO_1,Kp_NS_1,Kp_NM_1,Kp_NM_1},//ZO
                   {Kp_PS_1,Kp_PS_1,Kp_ZO_1,Kp_NS_1,Kp_NS_1,Kp_NM_1,Kp_NM_1},//PS
                   {Kp_PS_1,Kp_ZO_1,Kp_NS_1,Kp_NM_1,Kp_NM_1,Kp_NM_1,Kp_NB_1},//PM
                   {Kp_ZO_1,Kp_ZO_1,Kp_NM_1,Kp_NM_1,Kp_NM_1,Kp_NB_1,Kp_NB_1} //PB
                   };


float Kd_fuzzy_1[RANK][RANK]=
                   {                                            //e
                   {Kd_PS_1,Kd_NS_1,Kd_NB_1,Kd_NB_1,Kd_NB_1,Kd_NM_1,Kd_PS_1},//NB
                   {Kd_PS_1,Kd_NS_1,Kd_NB_1,Kd_NM_1,Kd_NM_1,Kd_NS_1,Kd_ZO_1},//NM
                   {Kd_ZO_1,Kd_NS_1,Kd_NM_1,Kd_NM_1,Kd_NS_1,Kd_NS_1,Kd_ZO_1},//NS
                   {Kd_ZO_1,Kd_NS_1,Kd_NS_1,Kd_NS_1,Kd_NS_1,Kd_NS_1,Kd_ZO_1},//ZO
                   {Kd_ZO_1,Kd_ZO_1,Kd_ZO_1,Kd_ZO_1,Kd_ZO_1,Kd_ZO_1,Kd_ZO_1},//PS
                   {Kd_PB_1,Kd_NS_1,Kd_PS_1,Kd_PS_1,Kd_PS_1,Kd_PS_1,Kd_PB_1},//PM
                   {Kd_PB_1,Kd_PM_1,Kd_PM_1,Kd_PM_1,Kd_PS_1,Kd_PS_1,Kd_PB_1} //PB
                   };


static void error_Fuzzier(float e,float e_Pre,float qValue[])
{
    float ec=0;
    ec=e-e_Pre;
    qValue[0]=6*e/(ediffer);
    qValue[1]=3*ec/(ediffer); //论域-6，6
}


static void membership_Calculate (float *ms,float qv,int *index)//计算ec和e的隶属度函数
{
  if((qv>=NB)&&(qv<NM))
  {

    index[0]=0;
    index[1]=1;
    ms[0]=-0.5*qv-2.0;  //y=-0.5x-2.0
    ms[1]=1-ms[0];   //y=0.5x+3.0
  }

  else if((qv>=NM)&&(qv<NS))

  {
    index[0]=1;
    index[1]=2;
    ms[0]=-0.5*qv-1.0;  //y=-0.5x-1.0
    ms[1]=1-ms[0];   //y=0.5x+2.0
  }

  else if((qv>=NS)&&(qv<ZO))

  {
    index[0]=2;
    index[1]=3;
    ms[0]=-0.5*qv;      //y=-0.5x
    ms[1]=1-ms[0];   //y=0.5x+1.0
  }

  else if((qv>=ZO)&&(qv<PS))
  {
    index[0]=3;
    index[1]=4;
    ms[0]=-0.5*qv+1.0;  //y=-0.5x+1.0
    ms[1]=1-ms[0];       //y=0.5x

  }

  else if((qv>=PS)&&(qv<PM))
  {
    index[0]=4;
    index[1]=5;
    ms[0]=-0.5*qv+2.0;  //y=-0.5x+2.0
    ms[1]=1-ms[0];   //y=0.5x-1.0
  }

  else if((qv>=PM)&&(qv<=PB))
  {
    index[0]=5;
    index[1]=6;
    ms[0]=-0.5*qv+3.0;  //y=-0.5x+3.0
    ms[1]=1-ms[0];  //y=0.5x-2.0
  }
}

/*********************************************************************************************************
** Function name	:  fuzzy_PID_Realize
** Descriptions		:  模糊PID控制
** input parameters	:  PID结构体指针，当前数值，预期数值                       
** output parameters:  无
** Returned value	:  无
*********************************************************************************************************/ 

float fuzzy_PID_Realize(PID_new *sptr, float NowData, float Point,int road_Type)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
        float iError,   			// 当前误差
             Realize;   			// 最后得出的实际增量
        float qValue[2]={0,0};      //存储模糊后e和e_Pre的二维向量
        float msE[2]={0,0};         //偏差隶属度函数值
        //fuzzy相关定义
        int index_E[2]={0,0};  	    //偏差隶属度索引
        int index_EC[2]={0,0};      //偏差增量隶属度索引
        float msEC[2]={0,0};        //偏差增量隶属度
        float kp_Index=0;
        float kd_Index=0;
        iError = Point - NowData;   // 计算当前误差
        float Kp_fuzzy[RANK][RANK]=
                           {
                           {Kp_PB,Kp_PB,Kp_PM,Kp_PM,Kp_PS,Kp_ZO,Kp_ZO},//NB
                           {Kp_PB,Kp_PB,Kp_PM,Kp_PS,Kp_PS,Kp_ZO,Kp_NS},//NM
                           {Kp_PM,Kp_PM,Kp_PM,Kp_PS,Kp_ZO,Kp_NS,Kp_NS},//NS
                           {Kp_PM,Kp_PM,Kp_PS,Kp_ZO,Kp_NS,Kp_NM,Kp_NM},//ZO
                           {Kp_PS,Kp_PS,Kp_ZO,Kp_NS,Kp_NS,Kp_NM,Kp_NM},//PS
                           {Kp_PS,Kp_ZO,Kp_NS,Kp_NM,Kp_NM,Kp_NM,Kp_NB},//PM
                           {Kp_ZO,Kp_ZO,Kp_NM,Kp_NM,Kp_NM,Kp_NB,Kp_NB} //PB
                           };

                        //eC    NB   ,NM   ,NS   ,ZO   ,PS   ,PM   ,PB
//        float Kd_fuzzy[RANK][RANK]=
//                           {                                            //e
//                           {Kd_PS,Kd_NS,Kd_NB,Kd_NB,Kd_NB,Kd_NM,Kd_PS},//NB
//                           {Kd_PS,Kd_NS,Kd_NB,Kd_NM,Kd_NM,Kd_NS,Kd_ZO},//NM
//                           {Kd_ZO,Kd_NS,Kd_NM,Kd_NM,Kd_NS,Kd_NS,Kd_ZO},//NS
//                           {Kd_ZO,Kd_NS,Kd_NS,Kd_NS,Kd_NS,Kd_NS,Kd_ZO},//ZO
//                           {Kd_ZO,Kd_ZO,Kd_ZO,Kd_ZO,Kd_ZO,Kd_ZO,Kd_ZO},//PS
//                           {Kd_PB,Kd_NS,Kd_PS,Kd_PS,Kd_PS,Kd_PS,Kd_PB},//PM
//                           {Kd_PB,Kd_PM,Kd_PM,Kd_PM,Kd_PS,Kd_PS,Kd_PB} //PB
//                           };

        if(iError<-20)
            iError=-20;
        else if(iError>20)
            iError=20;

        //进入fuzzy部分
        error_Fuzzier( iError,sptr->LastError,qValue);
        membership_Calculate(msE,qValue[0],index_E);  //计算E的隶属度函数值
        membership_Calculate(msEC,qValue[1],index_EC);//计算E_pre的隶属度函数值

        //重心法解模糊
        if(road_Type==1)
        {
        kp_Index= msE[0]*((msEC[0]*Kp_fuzzy[index_E[0]][index_EC[0]])+(msEC[1]*Kp_fuzzy[index_E[0]][index_EC[1]]))
                + msE[1]*((msEC[0]*Kp_fuzzy[index_E[1]][index_EC[0]])+(msEC[1]*Kp_fuzzy[index_E[1]][index_EC[1]]));

//        kd_Index= msE[0]*((msEC[0]*Kd_fuzzy[index_E[0]][index_EC[0]])+(msEC[1]*Kd_fuzzy[index_E[0]][index_EC[1]]))
//                + msE[1]*((msEC[0]*Kd_fuzzy[index_E[1]][index_EC[0]])+(msEC[1]*Kd_fuzzy[index_E[1]][index_EC[1]]));
      //kp_Index=Kp_Base;
        kd_Index=Kd_Base;
        }

        Realize = kp_Index * iError + kd_Index * (iError - sptr->LastError)+DirKdd * mpu_gyro_z ;//   使用KI时需 + sptr->SumError并进行积分清零

        sptr->PrevError = sptr->LastError;  // 更新前次误差,没用到
        sptr->LastError = iError;           // 更新上次误差
        sptr->LastData  = NowData;          // 更新上次数据
        return Realize; // 返回实际值
}


