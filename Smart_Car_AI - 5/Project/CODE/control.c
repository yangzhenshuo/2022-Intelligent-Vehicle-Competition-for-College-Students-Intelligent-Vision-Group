#include "control.h"
#include "motor.h"
#include "SEEKFREE_ICM20602.h"
#include "encoder.h"
#include "projectmath.h"
#include "System.h"
#include "position.h"
#include "image.h"

#define MOTOR_PWM_MAX (500.0 * 50) //������PWM

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
static float SpeedSet_L1 = 0;              //��ǰ�������ٶ�
static float f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
static float f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

static float f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
static float f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

/********�����********/
static float SpeedSet_L2 = 0;              //����������ٶ�
static float r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
static float r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

static float r_LeftSpeedControlBais = 0; //������ٶȿ�������
static float r_LeftSpeedControlOut = 0;  //������ٶȻ����

/********��ǰ��********/
static float SpeedSet_R1 = 0;               //��ǰ�������ٶ�
static float f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
static float f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ

static float f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
static float f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

/********�Һ���********/
static float SpeedSet_R2 = 0;               //�Һ��������ٶ�
static float r_RightSpeedDeltaPrev = 0;     //�Һ���ǰһ���ٶ�ƫ��
static float r_RightSpeedDeltaPrevPrev = 0; //�Һ���ǰǰһ���ٶ�ƫ

static float r_RightSpeedControlBais = 0; //�Һ����ٶȿ�������
static float r_RightSpeedControlOut = 0;  //�Һ����ٶȻ����

static float MotorDifferential = 0; //����÷������
/*************���ٶȿ��Ʋ���******************/
static float AngleDeltaPrev = 0;   //ǰһ�ν��ٶ�ƫ��
static float AngleControlBais = 0; //���ٶ��������
/*************λ�ÿ��Ʋ���******************/
static float PositionControlOut_x = 0;         // x��λ�������
static float PositionDeviationIntegrate_x = 0; // x���ۼ�λ�ò�
static float PositionDeltaPrev_x = 0;          // x��ǰһ��λ��ƫ��
static float PositionControlOut_y = 0;         // y��λ�������
static float PositionDeviationIntegrate_y = 0; // y���ۼ�λ�ò�
static float PositionDeltaPrev_y = 0;          // y��ǰһ��λ��ƫ��
/***********************************************************
 * @brief ��λ���Ʊ�����������pid������
 * @param
 * @return
 ***********************************************************/
void ResetControlArgs(void)
{
    /***��λ�ٶȿ��Ʊ���***/

    f_LeftSpeed = 0;                  //��ǰ����ٶ�
    f_RightSpeed = 0;                 //��ǰ���ٶ�
    r_LeftSpeed = 0;                  //������ٶ�
    r_RightSpeed = 0;                 //�Һ����ٶ�
    f_LeftStallCount = 0;             //��ǰ�����ת����
    f_RightStallCount = 0;            //��ǰ�����ת����
    r_LeftStallCount = 0;             //�������ת����
    r_RightStallCount = 0;            //�Һ�����ת����
                                      /*************���ٶȿ��Ʋ���******************/
    AngleDeltaPrev = 0;               //ǰһ�ν��ٶ�ƫ��
    AngleControlBais = 0;             //���ٶ��������
                                      /*************λ�ÿ��Ʋ���******************/
    PositionControlOut_x = 0;         // x��λ�������
    PositionDeviationIntegrate_x = 0; // x���ۼ�λ�ò�
    PositionDeltaPrev_x = 0;          // x��ǰһ��λ��ƫ��
    PositionControlOut_y = 0;         // y��λ�������
    PositionDeviationIntegrate_y = 0; // y���ۼ�λ�ò�
    PositionDeltaPrev_y = 0;          // y��ǰһ��λ��ƫ��
    /********��ǰ��********/
    static float SpeedSet_L1 = 0;              //��ǰ�������ٶ�
    static float f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
    static float f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

    static float f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
    static float f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

    /********�����********/
    static float SpeedSet_L2 = 0;              //����������ٶ�
    static float r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
    static float r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

    static float r_LeftSpeedControlBais = 0; //������ٶȿ�������
    static float r_LeftSpeedControlOut = 0;  //������ٶȻ����

    /********��ǰ��********/
    static float SpeedSet_R1 = 0;               //��ǰ�������ٶ�
    static float f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
    static float f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ

    static float f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
    static float f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

    /********�Һ���********/
    static float SpeedSet_R2 = 0;               //�Һ��������ٶ�
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
//    get_icm20602_accdata_spi(); //��ȡ���ٶȼ�����
    get_icm20602_gyro_spi();    // ��ȡ�����ǽ��ٶ�
//    float alpha = 0.3;
    float x, y, z;
    x = (float)icm_gyro_x - GyroOffset.x;
    y = (float)icm_gyro_y - GyroOffset.y;
    z = (float)icm_gyro_z - GyroOffset.z;
    if (x <= 10 && x >= -10)
        x = 0; //�Բ�ֵ�����˲�
    if (y <= 10 && y >= -10)
        y = 0;
    if (z <= 10 && z >= -10)
        z = 0;
//    icm.acc_x = (((float)icm_acc_x) * alpha) / 4096 + icm.acc_x * (1 - alpha);
//    icm.acc_y = (((float)icm_acc_y) * alpha) / 4096 + icm.acc_y * (1 - alpha);
//    icm.acc_z = (((float)icm_acc_z) * alpha) / 4096 + icm.acc_z * (1 - alpha);
    icm.gyro_x = x / 16.4;
    icm.gyro_y = y / 16.4;
    icm.gyro_z = z / 16.4;
    if (CarInfo.yaw > 360 || CarInfo.yaw < -360)
        CarInfo.yaw = 0;
    CarInfo.yaw += icm.gyro_z * dtt;
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
    CarInfo.RunDistance1 += (CarPulse.L1 + CarPulse.R1 + CarPulse.L2 + CarPulse.R2) / 4; //����С��x�����о���
    CarInfo.RunDistance2 += (CarPulse.L1 - CarPulse.R1 - CarPulse.L2 + CarPulse.R2) / 4; //����С��y�����о���
		CarInfo.delet1 = CarPulse.L1;
		CarInfo.delet2 = CarPulse.R1;
		CarInfo.delet3 = CarPulse.L2;
		CarInfo.delet4 = CarPulse.R2;
    // CarInfo.RealSpeed = (f_LeftSpeed + f_RightSpeed) * 0.5;   //����С���ٶ�
}
/***********************************************************
 * @brief �ٶȱ���ֺ���
 * @param delta ƫ��
 * @return �����ϵ��
 ***********************************************************/
static inline float SpeedChangeI(float delta)
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
 * @brief �Ƕȿ��Ƽ���,λ��ʽ,Pi����,�����
 * @param
 * @return
 ***********************************************************/
static inline void AngleControlCal(void)
{
    float AngleDelta; 	//���ٶ�ƫ��
	  if(CarInfo.Iscorrect=='F')
    {
			AngleDelta = CarInfo.AngleSet - CarInfo.yaw;
		}			//����ƫ����
		else AngleDelta=CarInfo.angle;
    CarInfo.SpeedSet_z = AngleControlPid.P * AngleDelta + AngleControlPid.D * (AngleDelta - AngleDeltaPrev); //λ��ʽPD����
    AngleDeltaPrev = AngleDelta;	//����Ϊ�ϴ�ƫ��
}
/***********************************************************
 * @brief �Ƕȿ������
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
    if (z > 400)
        z = 400;
    else if (z < -400)
        z = -400; // ��ת���ٶ��޷�
    SpeedSet_L1 = x + y - z * 0.2;
    SpeedSet_R1 = x - y + z * 0.2;
    SpeedSet_L2 = x - y - z * 0.2;
    SpeedSet_R2 = x + y + z * 0.2;
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
    // if (CarInfo.IsMotorDiffrientialOn == 'T')
    // SpeedDelta += MotorDifferential * (MotorDifferential >= 0 ? AdjustablePars.par_8 : 1);
    f_LeftSpeedControlBais = SpeedControlPid.P * (SpeedDelta1 - f_LeftSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            f_LeftSpeedControlBais += SpeedChangeI(DetAbs(SpeedSet_L1, f_LeftSpeed)) * SpeedControlPid.I * SpeedDelta1;
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
    f_LeftSpeedDeltaPrev = SpeedDelta1;              //����Ϊ�ϴ�ƫ��                                                                      //������һ��ƫ��
    f_LeftSpeedControlOut += f_LeftSpeedControlBais;
    /**************����ֿ��Ƽ���*************/
    SpeedDelta2 = SpeedSet_L2 - r_LeftSpeed;                                           //����ƫ����
    r_LeftSpeedControlBais = SpeedControlPid.P * (SpeedDelta2 - r_LeftSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            r_LeftSpeedControlBais += SpeedChangeI(DetAbs(SpeedSet_L2, r_LeftSpeed)) * SpeedControlPid.I * SpeedDelta2;
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
    r_LeftSpeedDeltaPrev = SpeedDelta2;              //����Ϊ�ϴ�ƫ��                                                                      //������һ��ƫ��
    r_LeftSpeedControlOut += r_LeftSpeedControlBais; //�������
    /**************��ǰ�ֿ��Ƽ���*************/
    SpeedDelta3 = SpeedSet_R1 - f_RightSpeed;                                            //����ƫ����
    f_RightSpeedControlBais = SpeedControlPid.P * (SpeedDelta3 - f_RightSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            f_RightSpeedControlBais += SpeedChangeI(DetAbs(SpeedSet_R1, f_RightSpeed)) * SpeedControlPid.I * SpeedDelta3;
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
    f_RightSpeedDeltaPrev = SpeedDelta3;               //����Ϊ�ϴ�ƫ��
    f_RightSpeedControlOut += f_RightSpeedControlBais; //�������
                                                       /**************�Һ��ֿ��Ƽ���*************/
    SpeedDelta4 = SpeedSet_R2 - r_RightSpeed;          //����ƫ����

    r_RightSpeedControlBais = SpeedControlPid.P * (SpeedDelta4 - r_RightSpeedDeltaPrev); //����ʽPID����
    if (SpeedControlPid.I != 0)
    {
        if (SystemSettings.ChangeIEnable == 'T')
        {
            r_RightSpeedControlBais += SpeedChangeI(DetAbs(SpeedSet_R2, r_RightSpeed)) * SpeedControlPid.I * SpeedDelta4;
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
    r_RightSpeedDeltaPrev = SpeedDelta4;               //����Ϊ�ϴ�ƫ��                                                                     //�����?��?��?��???2?
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
    Speedmath(CarInfo.SpeedSet_x, CarInfo.SpeedSet_y, CarInfo.SpeedSet_z);

    SpeedControlCal();
    MotorControlOut();
}
/***********************************************************
 * @brief λ�ÿ���
 * @param
 * @return
 ***********************************************************/
void PositionControl(void)
{
    float PositionDelta_x; //λ��ƫ��
    if (CarInfo.Iscorrect == 'F')
    {
        PositionDelta_x = position.x * 200 - (CarInfo.RunDistance1 / 210) * 5.5 * PI;
    } //����ƫ����
    else
        PositionDelta_x = CarInfo.distance1;
    PositionDeviationIntegrate_x += PositionDelta_x*5;
    if (PositionDeviationIntegrate_x > 20)       // 100//200
        PositionDeviationIntegrate_x = 20;       // 100//200
    else if (PositionDeviationIntegrate_x < -20) // 100//200
        PositionDeviationIntegrate_x = -20;      // 100//2000
    PositionControlOut_x = PositionControlPid.P * PositionDelta_x + PositionControlPid.I * PositionDeviationIntegrate_x + PositionControlPid.D * (PositionDelta_x - PositionDeltaPrev_x);
    PositionDeltaPrev_x = PositionDelta_x;
    if (PositionControlOut_x > 80)
    {
        PositionControlOut_x = 80;
    }
    else if (PositionControlOut_x < -80)
    {
        PositionControlOut_x = -80;
    }
    CarInfo.SpeedSet_x = PositionControlOut_x;
    float PositionDelta_y; //λ��ƫ��
    if (CarInfo.Iscorrect == 'F')
    {
        PositionDelta_y = position.y * 200 - (CarInfo.RunDistance2 / 220) * 5.5 * PI; //����ƫ����
    }
    else
        PositionDelta_y = CarInfo.distance2*5;
    PositionDeviationIntegrate_y += PositionDelta_y;
    if (PositionDeviationIntegrate_y > 20)       // 100//200
        PositionDeviationIntegrate_y = 20;       // 100//200
    else if (PositionDeviationIntegrate_y < -20) // 100//200
        PositionDeviationIntegrate_y = -20;      // 100//2000
    PositionControlOut_y = PositionControlPid.P * PositionDelta_y + PositionControlPid.I * PositionDeviationIntegrate_y + PositionControlPid.D * (PositionDelta_y - PositionDeltaPrev_y);
    PositionDeltaPrev_y = PositionDelta_y;
    if (PositionControlOut_y > 80)
    {
        PositionControlOut_y = 80;
    }
    else if (PositionControlOut_y < -80)
    {
        PositionControlOut_y = -80;
    }
    CarInfo.SpeedSet_y = PositionControlOut_y;
	  
}