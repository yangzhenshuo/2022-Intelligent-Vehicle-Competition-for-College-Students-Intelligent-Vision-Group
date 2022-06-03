#ifndef CODE_SYSTEM_H
#define CODE_SYSTEM_H

#include "headfile.h"

int hardware_init(void);
int thread_init(void);
int timer_init(void);
int communication_init(void);

void CarInformation_init(void);

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

  int32 PositionSet; //λ������

  uint8 BinaryMethod; //��ֵ������

  uint8 BinaryThreshold; //��ֵ����ֵ
  double RunDistance1;   //С�����о���
  double RunDistance2;
  int dotnum;              //��������Ŀ��������
	float goal_angle;				//Ŀ��Ƕ�
	float goal_ditance;			//Ŀ�����
	
  uint8 Iscorrect;
  float distance1;
  float distance2;
	float angle;
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
  uint8 Is_rotation;             //�Ƿ���ת�ǿ���
  uint8 Binary_start;            //��һ������ֵ
  uint8 Complete; 
  uint8 Is_straight;             //�Ƿ���ֱ�п���
  uint8 FuzzyEnable;             //�Ƿ������Ƕ�ģ������
  uint8 ChangeIEnable;           //�Ƿ������ٶȱ���ֿ���

} SystemSettingsTypedef;

//Ŀ������Ϣ
typedef struct
{
  uint8 x;
  uint8 y;
	uint8 flag;
} DotTypedef;

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
extern DotTypedef dot[20];
extern int dot_num;
extern rt_event_t event1;
extern rt_event_t run_event;
extern rt_sem_t server1_sem; //·���滮�ź���
extern rt_sem_t server2_sem;
extern rt_sem_t server3_sem;

#endif