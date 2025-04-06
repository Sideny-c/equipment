#include "stm32f10x.h"
#include "math"
#include "PID.h"

#define int32 int32_t

//�������͵����PID�����ṹ��
PID_new PID_Round,R_Motor_PID,L_Motor_PID,MOTOR_PID,MOTOR1_PID, Angle_PID, Ang_Vel_PID, Direct_PID, Turn_PID, Distance_PID,CurrentL_PID,CurrentR_PID;

/***********************************************************************************************************
											����ʽ��λ��ʽPID
************************************************************************************************************/

typedef struct PID_{
    
    float P;                          /* ����ϵ��     */
    float I;                          /* ����ϵ��     */
    float D;                          /* ΢��ϵ��     */
	
	float   limit;                     /* ��������     */ 
    float   Point;                     /* Ŀ��ֵ       */
    long    SumError;                  /* �ۼ����     */
    float   LastError;                 /* Error[-1]    */
    float   PrevError;                 /* Error[-2]    */
                                                                        
}PID_;

PID_ __PID;
PID_ *PID = &__PID;

/***********************************************************************************************************
*FunctionName    : PID_Error_Init()
*Description     : PID����ʼ��
*EntryParameter  : PID���ṹ��ָ��
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
** Descriptions		:   PID���в�������
** input parameters	:   *LPID:PID�ṹ��ָ��                         
** output parameters:   ��
** Returned value	:   ��
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
** Descriptions		:   ����Pֵ
** input parameters	:   Kp                       
** output parameters:   ��
** Returned value	:   ��
*********************************************************************************************************/ 

void PID_SetKp(float Kp) 
{
    PID->P     = Kp; 
}

/*********************************************************************************************************
** Function name	:   PID_SetKi
** Descriptions		:   ����Iֵ
** input parameters	:   Ki                         
** output parameters:   ��
** Returned value	:   ��
*********************************************************************************************************/ 

void PID_SetKi(float Ki) 
{
    PID->I     = Ki;
}

/*********************************************************************************************************
** Function name	:   PID_SetKd
** Descriptions		:   ����Dֵ
** input parameters	:   Kd                         
** output parameters:   ��
** Returned value	:   ��
*********************************************************************************************************/ 

void PID_SetKd(float Kd) 
{
    PID->D     = Kd;
}

/*********************************************************************************************************
** Function name	:   PID_SetKpid
** Descriptions		:   ����PIDֵ
** input parameters	:   Kd                         
** output parameters:   ��
** Returned value	:   ��
*********************************************************************************************************/ 

void PID_SetKpid(float Kp,float Ki,float Kd) 
{
    PID->P     = Kp;
    PID->I     = Ki;
    PID->D     = Kd;
}

/*********************************************************************************************************
** Function name	:  PID_Increase
** Descriptions		:  ����ʽPID����
** input parameters	:  PID�ṹ��ָ�룬PID��������ǰλ�ã�Ԥ��λ��                       
** output parameters:  ��
** Returned value	:  ��
*********************************************************************************************************/ 

float PID_Increase(PID_Error *sptr,PID_ *pid,float NowPlace,float Point)
{
	static float NowError;	//��ǰ���
	static int32 Inc;		//���ó���ʵ������
	
	NowError = Point - NowPlace; 	//���㵱ǰ���
	
	Inc = pid->P * (NowError - sptr->LastError) 						//����P
		 +pid->I * NowError												//����I
		 +pid->D * (NowError - 2 * sptr->LastError + sptr->PrevError;	//΢��D
	
	sptr->PrevError = sptr->LastError;		//����[Error-2]
	sptr->LastError = NowError;				//����[Error-1]
	
	return Inc;
}

/*********************************************************************************************************
** Function name	:  PID_Realize
** Descriptions		:  λ��ʽPID����
** input parameters	:  PID�ṹ��ָ�룬PID��������ǰλ�ã�Ԥ��λ��                       
** output parameters:  ��
** Returned value	:  ��
*********************************************************************************************************/ 

float PID_Realize(PID_Error *sptr,PID_ *pid,float NowPlace,float Point)
{
	static float NowError;		//��ǰ���
	static int32 Realize;		//ʵ�����
	
	NowError = Point - NowPlace;	//���㵱ǰ���
	
	sptr->SumError += pid->I * NowError; 	//������
	sptr->SumError = sptr->SumError > pid->limit ? pid->limit : sptr->SumError;
	sptr->SumError = sptr->SumError < -pid->limit ? -pid->limit : sptr->SumError; 	//�����޷�
	
	Realize =  pid->P * NowError						//����P
			 + sptr->SumError							//����I
			 + pid->D * (NowError - sptr->LastError);	//΢��D	
	
	sptr->LastError = NowError;				//����[Error-1]
	
	return Realize;		//����ʵ��ֵ
}


/*********************************************************************************************************
											  ģ��PID
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
    qValue[1]=3*ec/(ediffer); //����-6��6
}


static void membership_Calculate (float *ms,float qv,int *index)//����ec��e�������Ⱥ���
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
** Descriptions		:  ģ��PID����
** input parameters	:  PID�ṹ��ָ�룬��ǰ��ֵ��Ԥ����ֵ                       
** output parameters:  ��
** Returned value	:  ��
*********************************************************************************************************/ 

float fuzzy_PID_Realize(PID_new *sptr, float NowData, float Point,int road_Type)
{
    //��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
        float iError,   			// ��ǰ���
             Realize;   			// ���ó���ʵ������
        float qValue[2]={0,0};      //�洢ģ����e��e_Pre�Ķ�ά����
        float msE[2]={0,0};         //ƫ�������Ⱥ���ֵ
        //fuzzy��ض���
        int index_E[2]={0,0};  	    //ƫ������������
        int index_EC[2]={0,0};      //ƫ����������������
        float msEC[2]={0,0};        //ƫ������������
        float kp_Index=0;
        float kd_Index=0;
        iError = Point - NowData;   // ���㵱ǰ���
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

        //����fuzzy����
        error_Fuzzier( iError,sptr->LastError,qValue);
        membership_Calculate(msE,qValue[0],index_E);  //����E�������Ⱥ���ֵ
        membership_Calculate(msEC,qValue[1],index_EC);//����E_pre�������Ⱥ���ֵ

        //���ķ���ģ��
        if(road_Type==1)
        {
        kp_Index= msE[0]*((msEC[0]*Kp_fuzzy[index_E[0]][index_EC[0]])+(msEC[1]*Kp_fuzzy[index_E[0]][index_EC[1]]))
                + msE[1]*((msEC[0]*Kp_fuzzy[index_E[1]][index_EC[0]])+(msEC[1]*Kp_fuzzy[index_E[1]][index_EC[1]]));

//        kd_Index= msE[0]*((msEC[0]*Kd_fuzzy[index_E[0]][index_EC[0]])+(msEC[1]*Kd_fuzzy[index_E[0]][index_EC[1]]))
//                + msE[1]*((msEC[0]*Kd_fuzzy[index_E[1]][index_EC[0]])+(msEC[1]*Kd_fuzzy[index_E[1]][index_EC[1]]));
      //kp_Index=Kp_Base;
        kd_Index=Kd_Base;
        }

        Realize = kp_Index * iError + kd_Index * (iError - sptr->LastError)+DirKdd * mpu_gyro_z ;//   ʹ��KIʱ�� + sptr->SumError�����л�������

        sptr->PrevError = sptr->LastError;  // ����ǰ�����,û�õ�
        sptr->LastError = iError;           // �����ϴ����
        sptr->LastData  = NowData;          // �����ϴ�����
        return Realize; // ����ʵ��ֵ
}


