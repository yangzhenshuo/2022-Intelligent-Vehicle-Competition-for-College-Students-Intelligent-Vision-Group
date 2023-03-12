#ifndef CODE_SYSTEM_H
#define CODE_SYSTEM_H

#include "headfile.h"
#define dtt 0.005
#define DIAN_PIN B13

int hardware_init(void);
int thread_init(void);
int timer_init(void);
int communication_init(void);

void stop_car(void);
void CarInformation_init(void);
void gyro_offset_init(void);
void smotor_up(void);
void smotor_down(void);
void smotor_down1(void);

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
	float white_proportion;//�׵�ռ��
	
  double RunDistance1;   //С�����о���
  double RunDistance2;
  int dotnum;              //��������Ŀ��������
	int goal_x;				//Ŀ��Ƕ�
	int goal_y;			//Ŀ�����
	
	int num;          //����Ѱ�ҵ�Ϊ�ڼ���
  float distance1;
  float distance2;
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
  uint8 Is_control;              //�Ƿ�������
  uint8 Binary_start;            //��һ������ֵ
  uint8 IsCarry;                 //���˱�־λ
  uint8 Is_arrival;              //�Ƿ񵽴��趨λ��
  uint8 FuzzyEnable;             //�Ƿ������Ƕ�ģ������
  uint8 ChangeIEnable;           //�Ƿ������ٶȱ���ֿ���
	uint8 Iscorrect;
} SystemSettingsTypedef;

//Ŀ������Ϣ
typedef struct
{
  uint8 x;
  uint8 y;
	uint8 flag;
} DotTypedef;

extern GyroOffsetTypedef GyroOffset;
extern ICMTypedef icm;
extern speedTypedef speed;
extern positionTypedef position;
extern coordinateTypedef orignal_pos;
extern coordinateTypedef coordinate;
extern CarInfoTypedef CarInfo;
extern CarPulseTypedef CarPulse;
extern ControlPidTypedef SpeedControlPid;
extern ControlPidTypedef AngleControlPid;
extern ControlPidTypedef PositionPid_y;
extern ControlPidTypedef PositionPid_x;
extern ControlPidTypedef CorrectControlPid;
extern SystemSettingsTypedef SystemSettings;
extern DotTypedef dot[20];
extern int dot_num;
extern int rec[4][2];                                          //���ο�
//�¼���
extern rt_event_t event1;
extern rt_event_t run_event;
//�����ź���
extern rt_sem_t server1_sem;
extern rt_sem_t server2_sem;
extern rt_sem_t server3_sem;
extern rt_sem_t server4_sem;
//�����ź���
extern rt_sem_t key1_sem;
extern rt_sem_t key2_sem;
extern rt_sem_t key3_sem;
extern rt_sem_t key4_sem;
#endif