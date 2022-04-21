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
static float f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
static float f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

static float f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
static float f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

/********�����********/
static float r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
static float r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

static float r_LeftSpeedControlBais = 0; //������ٶȿ�������
static float r_LeftSpeedControlOut = 0;  //������ٶȻ����

/********��ǰ��********/
static float f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
static float f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ

static float f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
static float f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

/********�Һ���********/
static float r_RightSpeedDeltaPrev = 0;     //�Һ���ǰһ���ٶ�ƫ��
static float r_RightSpeedDeltaPrevPrev = 0; //�Һ���ǰǰһ���ٶ�ƫ

static float r_RightSpeedControlBais = 0; //�Һ����ٶȿ�������
static float r_RightSpeedControlOut = 0;  //�Һ����ٶȻ����

static float MotorDifferential = 0; //����÷������

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
	
    /********��ǰ��********/
    f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
    f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

    f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
    f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

    /********�����********/
    r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
    r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

    r_LeftSpeedControlBais = 0; //������ٶȿ�������
    r_LeftSpeedControlOut = 0;  //������ٶȻ����

    /********��ǰ��********/
    f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
    f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ
    
		f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
    f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

    /********�Һ���********/
    r_RightSpeedDeltaPrev = 0;     //�Һ���ǰһ���ٶ�ƫ��
    r_RightSpeedDeltaPrevPrev = 0; //�Һ���ǰǰһ���ٶ�ƫ

    r_RightSpeedControlBais = 0; //�Һ����ٶȿ�������
    r_RightSpeedControlOut = 0;  //�Һ����ٶȻ����
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
    CarInfo.RunDistance += (CarPulse.L1 + CarPulse.R1) / 2; //����С�����о���
    CarInfo.RealSpeed = (f_LeftSpeed + f_RightSpeed) * 0.5;   //����С���ٶ�
    /**********��ת���***********/
    if (CarInfo.IsRun == 'T' && (CarInfo.SpeedSet > 100 || CarInfo.SpeedSet < -100))
    {
        if (f_LeftSpeed <= 3 && f_LeftSpeed >= -3)
        {
            if (++f_LeftStallCount >= 100) //ǰ��������ת��������500ms
            {
                CarInfo.IsMotorStalled = 'T';
            }
        }
        else
        {
            f_LeftStallCount = 0; //ǰ��������ת����
        }
				if (r_LeftSpeed <= 3 && r_LeftSpeed >= -3)
        {
            if (++r_LeftStallCount >= 100) //����������ת��������500ms
            {
                CarInfo.IsMotorStalled = 'T';
            }
        }
        else
        {
            r_LeftStallCount = 0; //����������ת����
        }
        if (f_RightSpeed <= 3 && f_RightSpeed >= -3)
        {
            if (++f_RightStallCount >= 100) //ǰ���ҵ����ת��������500ms
            {
                CarInfo.IsMotorStalled = 'T';
            }
        }
        else
        {
            f_RightStallCount = 0; //ǰ���ҵ����ת����
        }
				if (r_RightSpeed <= 3 && r_RightSpeed >= -3)
        {
            if (++r_RightStallCount >= 100) //�����ҵ����ת��������500ms
            {
                CarInfo.IsMotorStalled = 'T';
            }
        }
        else
        {
            r_RightStallCount = 0; //�����ҵ����ת����
        }
    }
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
 * @brief �ٶȿ��Ƽ��㣬����ʽ��pid���ƣ������
 * @param
 * @return
***********************************************************/
static inline void SpeedControlCal(void)
{
    float SpeedDelta; //�ٶ�ƫ��
    /**************��ǰ�ֿ��Ƽ���*************/
    SpeedDelta = CarInfo.SpeedSet - f_LeftSpeed; //����ƫ����
//    if (CarInfo.IsMotorDiffrientialOn == 'T')
//    SpeedDelta += MotorDifferential * (MotorDifferential >= 0 ? AdjustablePars.par_8 : 1);
    f_LeftSpeedControlBais = SpeedControlPid.P * (SpeedDelta - f_LeftSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            f_LeftSpeedControlBais += ChangeI(DetAbs((CarInfo.SpeedSet + MotorDifferential), f_LeftSpeed)) * SpeedControlPid.I * SpeedDelta;
        }
        else
        {
            f_LeftSpeedControlBais += SpeedControlPid.I * SpeedDelta;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        f_LeftSpeedControlBais += SpeedControlPid.D * (SpeedDelta + f_LeftSpeedDeltaPrevPrev - 2 * f_LeftSpeedDeltaPrev);
    }
    f_LeftSpeedDeltaPrevPrev = f_LeftSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    f_LeftSpeedDeltaPrev = SpeedDelta;             //����Ϊ�ϴ�ƫ��                                                                      //������һ��ƫ��
    f_LeftSpeedControlOut += f_LeftSpeedControlBais;
		    /**************����ֿ��Ƽ���*************/
    SpeedDelta = CarInfo.SpeedSet - r_LeftSpeed; //����ƫ����
//    if (CarInfo.IsMotorDiffrientialOn == 'T')
//    SpeedDelta += MotorDifferential * (MotorDifferential >= 0 ? AdjustablePars.par_8 : 1);
    r_LeftSpeedControlBais = SpeedControlPid.P * (SpeedDelta - r_LeftSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            r_LeftSpeedControlBais += ChangeI(DetAbs((CarInfo.SpeedSet + MotorDifferential), r_LeftSpeed)) * SpeedControlPid.I * SpeedDelta;
        }
        else
        {
            r_LeftSpeedControlBais += SpeedControlPid.I * SpeedDelta;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        r_LeftSpeedControlBais += SpeedControlPid.D * (SpeedDelta + r_LeftSpeedDeltaPrevPrev - 2 * r_LeftSpeedDeltaPrev);
    }
    r_LeftSpeedDeltaPrevPrev = r_LeftSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    r_LeftSpeedDeltaPrev = SpeedDelta;             //����Ϊ�ϴ�ƫ��                                                                      //������һ��ƫ��
    r_LeftSpeedControlOut += r_LeftSpeedControlBais;//�������
    /**************��ǰ�ֿ��Ƽ���*************/
    SpeedDelta = CarInfo.SpeedSet - f_RightSpeed; //����ƫ����
   // if (CarInfo.IsMotorDiffrientialOn == 'T')
     //   SpeedDelta -= MotorDifferential * (MotorDifferential < 0 ? AdjustablePars.par_8 : 1);
    f_RightSpeedControlBais = SpeedControlPid.P * (SpeedDelta - f_RightSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            f_RightSpeedControlBais += ChangeI(DetAbs((CarInfo.SpeedSet - MotorDifferential), f_RightSpeed)) * SpeedControlPid.I * SpeedDelta;
        }
        else
        {
            f_RightSpeedControlBais += SpeedControlPid.I * SpeedDelta;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        f_RightSpeedControlBais += SpeedControlPid.D * (SpeedDelta + f_RightSpeedDeltaPrevPrev - 2 * f_RightSpeedDeltaPrev);
    }
    f_RightSpeedDeltaPrevPrev = f_RightSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    f_RightSpeedDeltaPrev = SpeedDelta;              //����Ϊ�ϴ�ƫ��                                                                     //�����?��?��?��???2?
    f_RightSpeedControlOut += f_RightSpeedControlBais; //�������
 /**************�Һ��ֿ��Ƽ���*************/
    SpeedDelta = CarInfo.SpeedSet - r_RightSpeed; //����ƫ����
   // if (CarInfo.IsMotorDiffrientialOn == 'T')
     //   SpeedDelta -= MotorDifferential * (MotorDifferential < 0 ? AdjustablePars.par_8 : 1);
    r_RightSpeedControlBais = SpeedControlPid.P * (SpeedDelta - r_RightSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            r_RightSpeedControlBais += ChangeI(DetAbs((CarInfo.SpeedSet - MotorDifferential), r_RightSpeed)) * SpeedControlPid.I * SpeedDelta;
        }
        else
        {
            r_RightSpeedControlBais += SpeedControlPid.I * SpeedDelta;
        }
    }
    if (SpeedControlPid.D != 0)
    {
        r_RightSpeedControlBais += SpeedControlPid.D * (SpeedDelta + r_RightSpeedDeltaPrevPrev - 2 * r_RightSpeedDeltaPrev);
    }
    r_RightSpeedDeltaPrevPrev = r_RightSpeedDeltaPrev; //����Ϊ���ϴ�ƫ��
    r_RightSpeedDeltaPrev = SpeedDelta;              //����Ϊ�ϴ�ƫ��                                                                     //�����?��?��?��???2?
    r_RightSpeedControlOut += r_RightSpeedControlBais; //�������
}

/***********************************************************
 * @brief ǰ�ֵ�����������
 * @param
 * @return
***********************************************************/
static inline void f_MotorControlOut(void)
{
    if (f_LeftSpeedControlOut < -MOTOR_PWM_MAX) //����ʽ�޷�
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
    SetMotorPwmAndDir((int32)(f_LeftSpeedControlOut), (int32)(f_RightSpeedControlOut));
}
/***********************************************************
 * @brief ���ֵ�����������
 * @param
 * @return
***********************************************************/
static inline void r_MotorControlOut(void)
{
    if (r_LeftSpeedControlOut < -MOTOR_PWM_MAX) //����ʽ����޷�
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
    SetMotorPwmAndDir((int32)(r_LeftSpeedControlOut), (int32)(r_RightSpeedControlOut));
}

/***********************************************************
 * @brief �ٶȿ���
 * @param
 * @return
***********************************************************/
void SpeedControl(void)
{
    GetMotorSpeed();
    if (CarInfo.IsOutGarage == 'F' && CarInfo.IsAiOn == 'F' && 0 < CarInfo.RealSpeed && CarInfo.RealSpeed < 50 && CarInfo.SpeedSet > 100)
    {
        f_LeftSpeedControlOut = f_RightSpeedControlOut = 15000;
			  r_LeftSpeedControlOut = r_RightSpeedControlOut = 15000;
    }
    else
    {
        SpeedControlCal();
    }
    f_MotorControlOut();
		r_MotorControlOut();
}