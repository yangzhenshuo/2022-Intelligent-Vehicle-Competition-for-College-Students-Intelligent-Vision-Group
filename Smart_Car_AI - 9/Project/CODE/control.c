#include "control.h"
#include "motor.h"
#include "SEEKFREE_ICM20602.h"
#include "encoder.h"
#include "projectmath.h"
#include "System.h"
#include "position.h"
#include "image.h"

#define MOTOR_PWM_MAX (500.0 * 60) //������PWM
#define Speed_max 80
#define error 10

/*************�ٶȿ��Ʋ���******************/
static float f_LeftSpeed = 0;       //��ǰ����ٶ�
static float f_RightSpeed = 0;      //��ǰ���ٶ�
static float r_LeftSpeed = 0;       //������ٶ�
static float r_RightSpeed = 0;      //�Һ����ٶ�
static uint8 f_LeftStallCount = 0;  //��ǰ�����ת����
static uint8 f_RightStallCount = 0; //��ǰ�����ת����
static uint8 r_LeftStallCount = 0;  //�������ת����
static uint8 r_RightStallCount = 0; //�Һ�����ת����

static float ChangeIA = 2*Speed_max; //����ֿ���Aϵ��
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
/*************�������Ʋ���******************/
static float CorrectControlOut_x = 0;         // x��λ�������
static float CorrectDeviationIntegrate_x = 0; // x���ۼ�λ�ò�
static float CorrectDeltaPrev_x = 0;          // x��ǰһ��λ��ƫ��
static float CorrectControlOut_y = 0;         // y��λ�������
static float CorrectDeviationIntegrate_y = 0; // y���ۼ�λ�ò�
static float CorrectDeltaPrev_y = 0;          // y��ǰһ��λ��ƫ��
/*************����״̬����******************/
static float Xspeed=0;
static float Yspeed=0;
static float LastXspeed=0;
static float LastYspeed=0;
/*************λ��ϵ������******************/
static float x_coe[20] = {0.060, 0.060, 0.065, 0.068, 0.070, 0.072, 0.072, 0.072, 0.073, 0.074, 0.074, 0.075, 0.0752, 0.0756, 0.0758, 0.076, 0.0760, 0.0760, 0.0769, 0.0770};
static float y_coe[20] = {0.060, 0.070, 0.071, 0.075, 0.075, 0.076, 0.078, 0.078, 0.079, 0.080, 0.080, 0.080, 0.0800, 0.0800, 0.0800, 0.080, 0.0805, 0.0805, 0.0805, 0.0806};
float Posvalue_x, Posvalue_y;	
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
		/*************�������Ʋ���******************/
		CorrectControlOut_x = 0;         // x��λ�������
		CorrectDeviationIntegrate_x = 0; // x���ۼ�λ�ò�
		CorrectDeltaPrev_x = 0;          // x��ǰһ��λ��ƫ��
		CorrectControlOut_y = 0;         // y��λ�������
		CorrectDeviationIntegrate_y = 0; // y���ۼ�λ�ò�
		CorrectDeltaPrev_y = 0;          // y��ǰһ��λ��ƫ��
    /********��ǰ��********/
     SpeedSet_L1 = 0;              //��ǰ�������ٶ�
     f_LeftSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
     f_LeftSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ��

     f_LeftSpeedControlBais = 0; //��ǰ���ٶȿ�������
     f_LeftSpeedControlOut = 0;  //��ǰ���ٶȻ����

    /********�����********/
     SpeedSet_L2 = 0;              //����������ٶ�
     r_LeftSpeedDeltaPrev = 0;     //�����ǰһ���ٶ�ƫ��
     r_LeftSpeedDeltaPrevPrev = 0; //�����ǰǰһ���ٶ�ƫ��

     r_LeftSpeedControlBais = 0; //������ٶȿ�������
    r_LeftSpeedControlOut = 0;  //������ٶȻ����

    /********��ǰ��********/
     SpeedSet_R1 = 0;               //��ǰ�������ٶ�
     f_RightSpeedDeltaPrev = 0;     //��ǰ��ǰһ���ٶ�ƫ��
    f_RightSpeedDeltaPrevPrev = 0; //��ǰ��ǰǰһ���ٶ�ƫ

    f_RightSpeedControlBais = 0; //��ǰ���ٶȿ�������
     f_RightSpeedControlOut = 0;  //��ǰ���ٶȻ����

    /********�Һ���********/
    SpeedSet_R2 = 0;               //�Һ��������ٶ�
    r_RightSpeedDeltaPrev = 0;     //�Һ���ǰһ���ٶ�ƫ��
    r_RightSpeedDeltaPrevPrev = 0; //�Һ���ǰǰһ���ٶ�ƫ

    r_RightSpeedControlBais = 0; //�Һ����ٶȿ�������
    r_RightSpeedControlOut = 0;  //�Һ����ٶȻ����
		/*************����״̬����******************/
    Xspeed=0;
    Yspeed=0;
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
    if (x <= error && x >= -error)
        x = 0; //�Բ�ֵ�����˲�
    if (y <= error && y >= -error)
        y = 0;
    if (z <= error && z >= -error)
        z = 0;
//    icm.acc_x = (((float)icm_acc_x) * alpha) / 4096 + icm.acc_x * (1 - alpha);
//    icm.acc_y = (((float)icm_acc_y) * alpha) / 4096 + icm.acc_y * (1 - alpha);
//    icm.acc_z = (((float)icm_acc_z) * alpha) / 4096 + icm.acc_z * (1 - alpha);
    icm.gyro_x = x / 16.4f;
    icm.gyro_y = y / 16.4f;
    icm.gyro_z = z / 16.4f;
    if (CarInfo.yaw > 360 || CarInfo.yaw < -360)
        CarInfo.yaw = 0;
    CarInfo.yaw += icm.gyro_z * dtt;
		CarInfo.delet1 = z;
}
/***********************************************************
 * @brief ��ȡ����ٶ��Լ��������о��루������ת��⣩���ٶȶ�Ӧ��160 -> 3m/s
 * @param
 * @return
 ***********************************************************/
static inline void coefficient(positionTypedef set)
{
	int x, y;
	x = Abs(set.y) - 1;
	y = Abs(set.x) - 1;
	if(x < 0)
	{
		x = 0;
	}
	if(x > 19)
	{
		x = 19;
	}
	if(y < 0)
	{
		y = 0;
	}
	if(y > 19)
	{
		y = 19;
	}
	if(x >= 5 && x <= 10)
	{
		Posvalue_x = x_coe[x] - 0.005;
	}
	else
	{
		Posvalue_x = x_coe[x];
	}
	if(y >= 5 && y <= 10)
	{
		Posvalue_y = y_coe[y] - 0.005;
	}
	else
	{
		Posvalue_y = y_coe[y];
	}
}
void GetMotorSpeed(void)
{
    /****�ٶȼ��㼰�������о���****/
    EncoderPulseGet(); //��ȡ������������
    f_LeftSpeed = (float)CarPulse.L1;
    f_RightSpeed = (float)CarPulse.R1;
    r_LeftSpeed = (float)CarPulse.L2;
    r_RightSpeed = (float)CarPulse.R2;
    coefficient(position);						                            //��ȡϵ��
    Yspeed= (CarPulse.L1 + CarPulse.R1 + CarPulse.L2 + CarPulse.R2 ) / 4; //����С��x�����о���
    Xspeed= (CarPulse.L1 - CarPulse.R1 - CarPulse.L2 + CarPulse.R2 ) / 4; //����С��y�����о���
	  CarInfo.RunDistance1 += Posvalue_y*(Yspeed + LastYspeed)/2;    //С�����о���
    CarInfo.RunDistance2 += Posvalue_x*(Xspeed + LastXspeed)/2;
		coordinate.x = CarInfo.RunDistance2 + orignal_pos.x * 200;
		coordinate.y = CarInfo.RunDistance1 + orignal_pos.y * 200;
	  LastXspeed=Xspeed;
	  LastYspeed=Yspeed;
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
		AngleDelta = CarInfo.AngleSet - CarInfo.yaw;
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
    SpeedSet_L1 = x + y - z*0.2;
    SpeedSet_R1 = x - y + z*0.2;
    SpeedSet_L2 = x - y - z*0.2;
    SpeedSet_R2 = x + y + z*0.2;
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
		CarInfo.delet1 = r_RightSpeedControlOut;
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
    float PositionDelta_x; //y��λ��ƫ��
        PositionDelta_x = position.x * 200 - CarInfo.RunDistance1; //����ƫ����
    PositionControlOut_x = PositionPid_y.P * PositionDelta_x + PositionPid_y.I * PositionDeviationIntegrate_x + PositionPid_y.D * (PositionDelta_x - PositionDeltaPrev_x);
    PositionDeltaPrev_x = PositionDelta_x;
    if (PositionControlOut_x > Speed_max)
    {
        PositionControlOut_x = Speed_max;
    }
    else if (PositionControlOut_x < -Speed_max)
    {
        PositionControlOut_x = -Speed_max;
    }
    CarInfo.SpeedSet_x = PositionControlOut_x;
    float PositionDelta_y; //x��λ��ƫ��
    PositionDelta_y = position.y * 200 - CarInfo.RunDistance2; //����ƫ����
//    PositionDeviationIntegrate_y += PositionDelta_y;
//    if (PositionDeviationIntegrate_y > integral_error)       // 100//200
//        PositionDeviationIntegrate_y = integral_error;       // 100//200
//    else if (PositionDeviationIntegrate_y < -integral_error) // 100//200
//        PositionDeviationIntegrate_y = -integral_error;      // 100//2000
    PositionControlOut_y = PositionPid_x.P * PositionDelta_y + PositionPid_x.I * PositionDeviationIntegrate_y + PositionPid_x.D * (PositionDelta_y - PositionDeltaPrev_y);
    PositionDeltaPrev_y = PositionDelta_y;
    if (PositionControlOut_y > Speed_max)
    {
        PositionControlOut_y = Speed_max;
    }
    else if (PositionControlOut_y < -Speed_max)
    {
        PositionControlOut_y = -Speed_max;
    }
    CarInfo.SpeedSet_y = PositionControlOut_y;
}
/***********************************************************
 * @brief ��������
 * @param
 * @return
 ***********************************************************/
void CorrectControl(void)
{
    float PositionDelta; //λ��ƫ��
	  //����λ��
    PositionDelta = CarInfo.distance1;
//    CorrectDeviationIntegrate_x += PositionDelta;
//    if (CorrectDeviationIntegrate_x > 10)       // 100//200
//        CorrectDeviationIntegrate_x = 10;       // 100//200
//   else if (CorrectDeviationIntegrate_x < -10) // 100//200
//		 CorrectDeviationIntegrate_x = -10;      // 100//2000
    CorrectControlOut_x = CorrectControlPid.P * PositionDelta + CorrectControlPid.I * CorrectDeviationIntegrate_x + CorrectControlPid.D * (PositionDelta - CorrectDeltaPrev_x);
    CorrectDeltaPrev_x = PositionDelta;
    if (CorrectControlOut_x > 70)
    {
        CorrectControlOut_x = 70;
    }
    else if (CorrectControlOut_x < -70)
    {
       CorrectControlOut_x = -70;
    }
    CarInfo.SpeedSet_x = CorrectControlOut_x;
		
		//����λ��
    PositionDelta = CarInfo.distance2;
//    CorrectDeviationIntegrate_y += PositionDelta;
//    if (CorrectDeviationIntegrate_y > 10)       // 100//200
//        CorrectDeviationIntegrate_y = 10;       // 100//200
//    else if (CorrectDeviationIntegrate_y < -10) // 100//200
//        CorrectDeviationIntegrate_y = -10;      // 100//2000
    CorrectControlOut_y = CorrectControlPid.P * PositionDelta + CorrectControlPid.I * CorrectDeviationIntegrate_y + CorrectControlPid.D * (PositionDelta - CorrectDeltaPrev_y);
    CorrectDeltaPrev_y = PositionDelta;
    if (CorrectControlOut_y > 70)
    {
        CorrectControlOut_y = 70;
    }
    else if (CorrectControlOut_y < -70)
    {
       CorrectControlOut_y = -70;
    }
    CarInfo.SpeedSet_y = CorrectControlOut_y;	
}