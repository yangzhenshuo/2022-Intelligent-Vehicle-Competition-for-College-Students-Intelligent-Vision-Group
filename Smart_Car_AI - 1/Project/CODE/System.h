#ifndef CODE_SYSTEM_H
#define CODE_SYSTEM_H

#include "headfile.h"

int hardware_init(void);
int thread_init(void);
int timer_init(void);

static inline void CarInfoInit(void);
static inline void SystemSettingsInit(void);
void CarInformation_init(void);
void SystemStart(void);
void System_stop(void);

typedef struct
{
  float x;
  float y;
  float z;
} GyroOffsetTypedef; //���Ư��

typedef struct
{
  float acc_x;
  float acc_y;
  float acc_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
} ICMTypedef; // ICM����

typedef struct
{
  float x;
  float y;
} speedTypedef; //���ٶȼƵó����ٶ�

typedef struct
{
  float x;
  float y;
} positionTypedef; //��ǰʵʱλ��

typedef struct
{
  int x;
  int y;
} coordinateTypedef; //λ�����껯

typedef struct
{
  float SpeedSet_x; // x���ٶ�����
  float SpeedSet_y; // y���ٶ�����
  float SpeedSet_z; // z���ٶ�����
  float SpeedSet;   //ƽ���ٶ�����
  float delet1;
	float delet2;
	float delet3;
	float delet4;
  float RealSpeed; //��ʵ�ٶ�
  float AngleSet;  //�Ƕ�����
  float pitch;     //������
  float roll;      //�����
  float yaw;       //�����

  double current_angle; //��ǰ����ƫ��Ƕ�

  int32 PositionSet; //λ������

  uint8 BinaryMethod; //��ֵ������

  uint8 BinaryThreshold; //��ֵ����ֵ
  int32 RunDistance;     //С�����о���
} CarInfoTypedef;

typedef struct
{
  int16 L1; //ǰ���ֵı�����ֵ
  int16 L2; //�����ֵı�����ֵ
  int16 R1; //ǰ���ֵı�����ֵ
  int16 R2; //�����ֵı�����ֵ
} CarPulseTypedef;

typedef struct
{
  float P;
  float I;
  float D;
} ControlPidTypedef;

typedef struct
{
  uint8 IsFound_dot;             //�Ƿ�ʼĿ���Ѱ��
  uint8 IsAiOn;                  //ʶ��ģʽ�Ƿ��
  uint8 Binary_start;     //��һ������ֵ
  uint8 ImageStatusReportEnable; //�Ƿ�����ͼ����ģʽ�ϱ�
  uint8 AiEnable;                //�Ƿ���ʶ��
  uint8 FuzzyEnable;             //�Ƿ������Ƕ�ģ������
  uint8 ChangeIEnable;           //�Ƿ������ٶȱ���ֿ���

  uint16 ApriltagSearchSpeed; //����Apriltag�ٶ�

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
extern ControlPidTypedef AngleControlPid;
extern ControlPidTypedef PositionControlPid;
extern SystemSettingsTypedef SystemSettings;

#endif