#ifndef CODE_SYSTEM_H
#define CODE_SYSTEM_H

#include "headfile.h"

int hardware_init(void);
int software_init(void);

typedef struct
{
	float x;
	float y;
	float z;
}GyroOffsetTypedef;//���Ư��

typedef struct
{
	float acc_x;
	float acc_y;
	float acc_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
}ICMTypedef;//ICM����

typedef struct
{
	float x;
	float y;
}speedTypedef;//���ٶȼƵó����ٶ�

typedef struct
{
	float x;
	float y;
}positionTypedef;//��ǰʵʱλ��

typedef struct
{
	int x;
	int y;
}coordinateTypedef;//λ�����껯

typedef struct
{
	uint8 IsRun;                 //�Ƿ�����
  uint8 IsOutGarage;           //�Ƿ����
  uint8 IsCameraDetectRun;     //����ͷ����Ƿ�����
  uint8 IsMotorStalled;        //����Ƿ��ת
  uint8 IsMotorDiffrientialOn; //��������Ƿ���

  uint8 IsAiOn;      //ʶ��ģʽ�Ƿ��
  uint8 IsCounterOn; //�����Ӷ�ʱ���Ƿ��
  uint8 SecCount;    //��ʱ������ֵ���룩

  float SpeedSet;  //�ٶ�����
  float RealSpeed; //��ʵ�ٶ�
  float AngleSet;  //�Ƕ�����
  float RealAngle; //��ʵ�Ƕ�
	float pitch;//������
	float roll;//�����
	float yaw;//�����

  int32 PositionSet; //λ������

  uint8 BinaryThreshold; //��ֵ����ֵ
  int32 RunDistance;     //С�����о���
}CarInfoTypedef;

typedef struct
{
	int16 L1;//ǰ���ֵı�����ֵ
	int16 L2;//�����ֵı�����ֵ
	int16 R1;//ǰ���ֵı�����ֵ
	int16 R2;//�����ֵı�����ֵ
}CarPulseTypedef;

typedef struct
{
	float P;
	float I;
	float D;
}ControlPidTypedef;

typedef struct
{
  uint8 WirelessEnable; //�Ƿ��������ߴ���
  uint8 ScreenEnable;   //�Ƿ�������ʾ��

  uint8 PostureReportEnable;     //�Ƿ�������̬�ϱ�
  uint8 ImageStatusReportEnable; //�Ƿ�����ͼ����ģʽ�ϱ�
  uint8 AiEnable;                //�Ƿ���ʶ��
  uint8 FuzzyEnable;             //�Ƿ������Ƕ�ģ������
  uint8 ChangeIEnable;           //�Ƿ������ٶȱ���ֿ���

  uint8 BinaryMethod; //��ֵ������

  uint16 StraightSpeed;       //ֱ���ٶ�
  uint16 InTurnSpeed;         //�����ٶ�
  uint16 OutTurnSpeed;        //�����ٶ�
  uint16 InRampSpeed;         //���µ��ٶ�
  uint16 OutRampSpeed;        //���µ��ٶ�
  uint16 InForkSpeed;         //�������ٶ�
  uint16 OutForkSpeed;        //�������ٶ�
  uint16 InCircleSpeed;       //��Բ���ٶ�
  uint16 OutCircleSpeed;      //��Բ���ٶ�
  uint16 ApriltagSearchSpeed; //����Apriltag�ٶ�
  uint16 TargetSearchSpeed;   //���������ٶ�

} SystemSettingsTypedef;

extern float dtt;
extern GyroOffsetTypedef GyroOffset;
extern ICMTypedef icm;
extern speedTypedef speed;
extern positionTypedef position;
extern coordinateTypedef coordinate;
extern CarInfoTypedef CarInfo;
extern CarPulseTypedef CarPulse;
extern ControlPidTypedef SpeedControlPid;
extern SystemSettingsTypedef SystemSettings;

static inline void CarInfoInit(void);
static inline void SystemSettingsInit(void);

#endif