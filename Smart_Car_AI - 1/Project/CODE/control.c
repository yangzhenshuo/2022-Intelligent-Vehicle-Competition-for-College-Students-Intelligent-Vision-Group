/*
 * control.c
 * �����㷨 
 * Author: han
 */

#include "control.h"
#include "motor.h"
#include "SEEKFREE_ICM20602.h"
#include "encoder.h"
#include "projectmath.h"
#include "System.h"
#include "position.h"

#define MOTOR_PWM_MAX (900.0 * 50) //������PWM

/*************�ٶȿ��Ʋ���******************/
static float f_LeftSpeed = 0;       //��ǰ����ٶ�
static float f_RightSpeed = 0;      //��ǰ���ٶ�
static float r_LeftSpeed = 0;       //������ٶ�
static float r_RightSpeed = 0;      //�Һ����ٶ�
static uint8 f_LeftStallCount = 0;  //��ǰ�����ת����
static uint8 f_RightStallCount = 0; //��ǰ�����ת����
static uint8 r_LeftStallCount = 0;  //�������ת����
static uint8 r_RightStallCount = 0; //�Һ�����ת����

static float ChangeIA = 100; //����ֿ���Aϵ��
static float ChangeIB = 10;  //����ֿ���Bϵ��

/********��ǰ��********/
static float SpeedSet_L1=0;                //��ǰ�������ٶ�
static float f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
static float f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

static float f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
static float f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

/********�����********/
static float SpeedSet_L2=0;                //����������ٶ�
static float r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
static float r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

static float r_LeftSpeedControlBais = 0; //������ٶȿ�������
static float r_LeftSpeedControlOut = 0;  //������ٶȻ����

/********��ǰ��********/
static float SpeedSet_R1=0;                //��ǰ�������ٶ�
static float f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
static float f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ

static float f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
static float f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

/********�Һ���********/
static float SpeedSet_R2=0;                //�Һ��������ٶ�
static float r_RightSpeedDeltaPrev = 0;     //�Һ���ǰһ���ٶ�ƫ��
static float r_RightSpeedDeltaPrevPrev = 0; //�Һ���ǰǰһ���ٶ�ƫ

static float r_RightSpeedControlBais = 0; //�Һ����ٶȿ�������
static float r_RightSpeedControlOut = 0;  //�Һ����ٶȻ����

static float MotorDifferential = 0; //����÷������
/*************���ٶȿ��Ʋ���******************/
static float AngleDeltaPrev = 0;  //ǰһ�ν��ٶ�ƫ��
static float AngleDeltaPrevPrev = 0;  //ǰǰһ�ν��ٶ�ƫ��
static float AngleControlBais =0;    //���ٶ��������
/*************λ�ÿ��Ʋ���******************/
static float PositionControlOut=0;   //λ�������
static float PositionDeviationIntegrate=0;   //�ۼ�λ�ò�
static float PositionDeltaPrev=0;     //ǰһ��λ��ƫ��
/***********************************************************
 * @brief ��λ���Ʊ�����������pid������
 * @param
 * @return
***********************************************************/
void ResetControlArgs(void)
{
    /***��λ�ٶȿ��Ʊ���***/

    f_LeftSpeed = 0;       //��ǰ����ٶ�
    f_RightSpeed = 0;      //��ǰ���ٶ�
    r_LeftSpeed = 0;       //������ٶ�
    r_RightSpeed = 0;      //�Һ����ٶ�
	
	  CarInfo.IsMotorStalled = 'F';//�������ת
    CarInfo.IsMotorDiffrientialOn = 'T';//������ٿ���
    f_LeftStallCount = 0;  //��ǰ�����ת����
    f_RightStallCount = 0; //��ǰ�����ת����
    r_LeftStallCount = 0;  //�������ת����
    r_RightStallCount = 0; //�Һ�����ת����
	 /*************���ٶȿ��Ʋ���******************/
    AngleDeltaPrev = 0;  //ǰһ�ν��ٶ�ƫ��
	  AngleDeltaPrevPrev = 0;  //ǰǰһ�ν��ٶ�ƫ��
	  AngleControlBais =0;     //���ٶ��������
	  /*************λ�ÿ��Ʋ���******************/
	  PositionControlOut=0;   //λ�������
    PositionDeviationIntegrate=0;   //�ۼ�λ�ò�
    PositionDeltaPrev=0;     //ǰһ��λ��ƫ��
   /********��ǰ��********/
   static float SpeedSet_L1=0;                //��ǰ�������ٶ�
   static float f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
   static float f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

   static float f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
   static float f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

   /********�����********/
   static float SpeedSet_L2=0;                //����������ٶ�
   static float r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
   static float r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

   static float r_LeftSpeedControlBais = 0; //������ٶȿ�������
   static float r_LeftSpeedControlOut = 0;  //������ٶȻ����

   /********��ǰ��********/
   static float SpeedSet_R1=0;                //��ǰ�������ٶ�
   static float f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
   static float f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ

   static float f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
   static float f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

   /********�Һ���********/
   static float SpeedSet_R2=0;                //�Һ��������ٶ�
   static float r_RightSpeedDeltaPrev = 0;     //�Һ���ǰһ���ٶ�ƫ��
   static float r_RightSpeedDeltaPrevPrev = 0; //�Һ���ǰǰһ���ٶ�ƫ

   static float r_RightSpeedControlBais = 0; //�Һ����ٶȿ�������
   static float r_RightSpeedControlOut = 0;  //�Һ����ٶȻ����
}
/***********************************************************
 * @brief С����̬����
 * @param
 * @return
***********************************************************/
static inline void CarPostureCal(void)
{   
  //	  gyro_offset_init();//��������Ʈ��ʼ��
//    location();       //�ɼ�����
	   get_icm20602_accdata_spi();  //��ȡ���ٶȼ�����   
	   get_icm20602_gyro_spi();    // ��ȡ�����ǽ��ٶ�
	  float alpha=0.3;
	  icm.acc_x = (((float) icm_acc_x) * alpha) / 4096 + icm.acc_x * (1 - alpha);
    icm.acc_y = (((float) icm_acc_y) * alpha) / 4096 + icm.acc_y * (1 - alpha);
    icm.acc_z = (((float) icm_acc_z) * alpha) / 4096 + icm.acc_z * (1 - alpha);
    icm.gyro_x = (float) icm_gyro_x - GyroOffset.x; 
    icm.gyro_y = (float) icm_gyro_y - GyroOffset.y;
    icm.gyro_z = (float) icm_gyro_z - GyroOffset.z;	   
}
/***********************************************************
 * @brief ��ȡ����ٶ��Լ��������о��루������ת��⣩���ٶȶ�Ӧ��160 -> 3m/s
 * @param
 * @return
***********************************************************/
void GetMotorSpeed(void)
{
    /****�ٶȼ��㼰�������о���****/
    EncoderPulseGet(); //��ȡ������������
    f_LeftSpeed = (float)CarPulse.L1;
    f_RightSpeed = (float)CarPulse.R1;
	  r_LeftSpeed = (float)CarPulse.L2;
    r_RightSpeed = (float)CarPulse.R2;
    //CarInfo.RunDistance += (CarPulse.L1 + CarPulse.R1) / 2; //����С�����о���
    //CarInfo.RealSpeed = (f_LeftSpeed + f_RightSpeed) * 0.5;   //����С���ٶ�
    /**********��ת���***********/
//    if (CarInfo.IsRun == 'T' && (CarInfo.SpeedSet_z > 100 || CarInfo.SpeedSet_z < -100))
//    {
//        if (f_LeftSpeed <= 3 && f_LeftSpeed >= -3)
//        {
//            if (++f_LeftStallCount >= 100) //ǰ��������ת��������500ms
//            {
//                CarInfo.IsMotorStalled = 'T';
//            }
//        }
//        else
//        {
//            f_LeftStallCount = 0; //ǰ��������ת����
//        }
//				if (r_LeftSpeed <= 3 && r_LeftSpeed >= -3)
//        {
//            if (++r_LeftStallCount >= 100) //����������ת��������500ms
//            {
//                CarInfo.IsMotorStalled = 'T';
//            }
//        }
//        else
//        {
//            r_LeftStallCount = 0; //����������ת����
//        }
//        if (f_RightSpeed <= 3 && f_RightSpeed >= -3)
//        {
//            if (++f_RightStallCount >= 100) //ǰ���ҵ����ת��������500ms
//            {
//                CarInfo.IsMotorStalled = 'T';
//            }
//        }
//        else
//        {
//            f_RightStallCount = 0; //ǰ���ҵ����ת����
//        }
//				if (r_RightSpeed <= 3 && r_RightSpeed >= -3)
//        {
//            if (++r_RightStallCount >= 100) //�����ҵ����ת��������500ms
//            {
//                CarInfo.IsMotorStalled = 'T';
//            }
//        }
//        else
//        {
//            r_RightStallCount = 0; //�����ҵ����ת����
//        }
//    }
}
/***********************************************************
 * @brief ����ֺ���
 * @param delta ƫ��
 * @return �����ϵ��
***********************************************************/
static inline float ChangeI(float delta)
{
    if (delta <= ChangeIB) //Сƫ�����û�������
    {
        return 1.0;
    }
    else if (delta > ChangeIA + ChangeIB) //��ƫ��ȡ����������
    {
        return 0.0;
    }
    else
    {
        return (ChangeIA + ChangeIB - delta) / ChangeIA;
    }
}
/***********************************************************
 * @brief ���ٶȿ��Ƽ���,����ʽ,Pi����,�����
 * @param
 * @return
***********************************************************/
static inline void AngleControlCal(void)
{
	  float AngleDelta;                                  //���ٶ�ƫ��
    AngleDelta = CarInfo.AngleSet - icm.gyro_z; //����ƫ����
        AngleControlBais = AngleControlPid.P * ( AngleDelta- AngleDeltaPrev); //����ʽPID����
    if (AngleControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            AngleControlBais += ChangeI(DetAbs(CarInfo.AngleSet , icm_gyro_z)) * AngleControlPid.I * AngleDelta;
        }
        else
        {
            AngleControlBais += SpeedControlPid.I * AngleDelta;
        }
    }
    if (AngleControlPid.D != 0)
    {
        AngleControlBais += AngleControlPid.D * (AngleDelta + AngleDeltaPrevPrev - 2 * AngleDeltaPrev);
    }
    AngleDeltaPrevPrev = AngleDeltaPrev; //����Ϊ���ϴ�ƫ��
    AngleDeltaPrev = AngleDelta;             //����Ϊ�ϴ�ƫ�� 
		CarInfo.SpeedSet_z += AngleControlBais;  //�������
	  CarInfo.delet1=CarInfo.AngleSet;
    CarInfo.delet2=icm.gyro_z;
}
/***********************************************************
 * @brief ���ٶȿ������
 * @param
 * @return
***********************************************************/
void AngleControl()
{ 
	CarPostureCal();
	AngleControlCal();
}
/***********************************************************
 * @brief �ٶȿ��ƽ���
 * @param
 * @return
***********************************************************/
static inline void Speedmath(float x, float y, float z)
{
//	if(z>100) z= 100;
//   else if(z < -100) z = -100;   // ��ת���ٶ��޷�
	SpeedSet_L1=x+y-z*0.2;
	SpeedSet_R1=x-y+z*0.2;
	SpeedSet_L2=x-y-z*0.2;
	SpeedSet_R2=x+y+z*0.2;
}
/***********************************************************
 * @brief �ٶȿ��Ƽ��㣬����ʽ��pid���ƣ������
 * @param
 * @return
***********************************************************/
static inline void SpeedControlCal(void)
{
    float SpeedDelta1; //��ǰ���ٶ�ƫ��
	  float SpeedDelta2; //������ٶ�ƫ��
	  float SpeedDelta3; //��ǰ���ٶ�ƫ��
	  float SpeedDelta4; //�Һ����ٶ�ƫ��
    /**************��ǰ�ֿ��Ƽ���*************/
    SpeedDelta1 = SpeedSet_L1 - f_LeftSpeed; //����ƫ����
    //if (CarInfo.IsMotorDiffrientialOn == 'T')
    //SpeedDelta += MotorDifferential * (MotorDifferential >= 0 ? AdjustablePars.par_8 : 1);
    f_LeftSpeedControlBais = SpeedControlPid.P * (SpeedDelta1 - f_LeftSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            f_LeftSpeedControlBais += ChangeI(DetAbs(SpeedSet_L1 , f_LeftSpeed)) * SpeedControlPid.I * SpeedDelta1;
        }
        else
        {
            f_LeftSpeedControlBais += SpeedControlPid.I * SpeedDelta1;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        f_LeftSpeedControlBais += SpeedControlPid.D * (SpeedDelta1 + f_LeftSpeedDeltaPrevPrev - 2 * f_LeftSpeedDeltaPrev);
    }
    f_LeftSpeedDeltaPrevPrev = f_LeftSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    f_LeftSpeedDeltaPrev = SpeedDelta1;             //����Ϊ�ϴ�ƫ��                                                                      //������һ��ƫ��
    f_LeftSpeedControlOut += f_LeftSpeedControlBais;
		    /**************����ֿ��Ƽ���*************/
    SpeedDelta2 = SpeedSet_L2 - r_LeftSpeed; //����ƫ����
//    if (CarInfo.IsMotorDiffrientialOn == 'T')
//    SpeedDelta += MotorDifferential * (MotorDifferential >= 0 ? AdjustablePars.par_8 : 1);
    r_LeftSpeedControlBais = SpeedControlPid.P * (SpeedDelta2 - r_LeftSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            r_LeftSpeedControlBais += ChangeI(DetAbs(SpeedSet_L2 , r_LeftSpeed)) * SpeedControlPid.I * SpeedDelta2;
        }
        else
        {
            r_LeftSpeedControlBais += SpeedControlPid.I * SpeedDelta2;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        r_LeftSpeedControlBais += SpeedControlPid.D * (SpeedDelta2 + r_LeftSpeedDeltaPrevPrev - 2 * r_LeftSpeedDeltaPrev);
    }
    r_LeftSpeedDeltaPrevPrev = r_LeftSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    r_LeftSpeedDeltaPrev = SpeedDelta2;             //����Ϊ�ϴ�ƫ��                                                                      //������һ��ƫ��
    r_LeftSpeedControlOut += r_LeftSpeedControlBais;//�������
    /**************��ǰ�ֿ��Ƽ���*************/
    SpeedDelta3 = SpeedSet_R1 - f_RightSpeed; //����ƫ����
   // if (CarInfo.IsMotorDiffrientialOn == 'T')
     //   SpeedDelta -= MotorDifferential * (MotorDifferential < 0 ? AdjustablePars.par_8 : 1);
    f_RightSpeedControlBais = SpeedControlPid.P * (SpeedDelta3 - f_RightSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            f_RightSpeedControlBais += ChangeI(DetAbs(SpeedSet_R1, f_RightSpeed)) * SpeedControlPid.I * SpeedDelta3;
        }
        else
        {
            f_RightSpeedControlBais += SpeedControlPid.I * SpeedDelta3;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        f_RightSpeedControlBais += SpeedControlPid.D * (SpeedDelta3 + f_RightSpeedDeltaPrevPrev - 2 * f_RightSpeedDeltaPrev);
    }
    f_RightSpeedDeltaPrevPrev = f_RightSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    f_RightSpeedDeltaPrev = SpeedDelta3;              //����Ϊ�ϴ�ƫ��                                                                     
    f_RightSpeedControlOut += f_RightSpeedControlBais; //�������
 /**************�Һ��ֿ��Ƽ���*************/
    SpeedDelta4= SpeedSet_R2 - r_RightSpeed; //����ƫ����
   // if (CarInfo.IsMotorDiffrientialOn == 'T')
     //   SpeedDelta -= MotorDifferential * (MotorDifferential < 0 ? AdjustablePars.par_8 : 1);
    r_RightSpeedControlBais = SpeedControlPid.P * (SpeedDelta4 - r_RightSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            r_RightSpeedControlBais += ChangeI(DetAbs(SpeedSet_R2, r_RightSpeed)) * SpeedControlPid.I * SpeedDelta4;
        }
        else
        {
            r_RightSpeedControlBais += SpeedControlPid.I * SpeedDelta4;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        r_RightSpeedControlBais += SpeedControlPid.D * (SpeedDelta4 + r_RightSpeedDeltaPrevPrev - 2 * r_RightSpeedDeltaPrev);
    }
    r_RightSpeedDeltaPrevPrev = r_RightSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    r_RightSpeedDeltaPrev = SpeedDelta4;              //����Ϊ�ϴ�ƫ��                                                                     //�����?��?��?��???2?
    r_RightSpeedControlOut += r_RightSpeedControlBais; //�������
}

/***********************************************************
 * @brief ������������
 * @param
 * @return
***********************************************************/
static inline void MotorControlOut(void)
{
    if (f_LeftSpeedControlOut < -MOTOR_PWM_MAX) //ǰ���޷�
    {
        f_LeftSpeedControlOut = -MOTOR_PWM_MAX;
    }
    else if (f_LeftSpeedControlOut > MOTOR_PWM_MAX)
    {
        f_LeftSpeedControlOut = MOTOR_PWM_MAX;
    }
    if (f_RightSpeedControlOut < -MOTOR_PWM_MAX)
    {
        f_RightSpeedControlOut = -MOTOR_PWM_MAX;
    }
    else if (f_RightSpeedControlOut > MOTOR_PWM_MAX)
    {
        f_RightSpeedControlOut = MOTOR_PWM_MAX;
    }
		if (r_LeftSpeedControlOut < -MOTOR_PWM_MAX) //�����޷�
    {
        r_LeftSpeedControlOut = -MOTOR_PWM_MAX;
    }
    else if (r_LeftSpeedControlOut > MOTOR_PWM_MAX)
    {
        r_LeftSpeedControlOut = MOTOR_PWM_MAX;
    }
    if (r_RightSpeedControlOut < -MOTOR_PWM_MAX)
    {
        r_RightSpeedControlOut = -MOTOR_PWM_MAX;
    }
    else if (r_RightSpeedControlOut > MOTOR_PWM_MAX)
    {
        r_RightSpeedControlOut = MOTOR_PWM_MAX;
    }
motor_control((int32)(f_RightSpeedControlOut), (int32)(f_LeftSpeedControlOut), (int32)(r_RightSpeedControlOut), (int32)(r_LeftSpeedControlOut));
}
/***********************************************************
 * @brief �ٶȿ���
 * @param
 * @return
***********************************************************/
void SpeedControl(void)
{
    GetMotorSpeed();
	  Speedmath(CarInfo.SpeedSet_x,CarInfo.SpeedSet_y,CarInfo.SpeedSet_z);
//    if (CarInfo.IsOutGarage == 'F' && CarInfo.IsAiOn == 'F' && 0 < CarInfo.RealSpeed && CarInfo.RealSpeed < 50 && CarInfo.SpeedSet_z > 100)
//    {
//        f_LeftSpeedControlOut = f_RightSpeedControlOut = 15000;
//			  r_LeftSpeedControlOut = r_RightSpeedControlOut = 15000;
//    }
//    else
//    {
     SpeedControlCal();
//    }
    MotorControlOut();
}
/***********************************************************
 * @brief λ�ÿ���
 * @param
 * @return
***********************************************************/
void PositionControl(void)
{
    float PositonDelta;                                       //λ��ƫ��
    PositonDelta = CarInfo.PositionSet - CarInfo.RunDistance; //����ƫ����
	  PositionDeviationIntegrate += PositonDelta;
    PositionControlOut = PositionControlPid.D* PositonDelta + PositionControlPid.I * PositionDeviationIntegrate + PositionControlPid.D * (PositonDelta - PositionDeltaPrev);
	  PositionDeltaPrev = PositonDelta;
    CarInfo.SpeedSet = PositionControlOut;
}