/*
 * System.c
 * �������ȫ�ֹ���
 * Author: ����˶
 */

#include "System.h"
#include "motor.h"
#include "encoder.h"
#include "display.h"
#include "button.h"
#include "timer_pit.h"
#include "buzzer.h"
#include "openart_mini.h"
#include "smotor.h"
#include "image.h"
#include "TSP.h"
#include "communicate.h"
#include "server.h"

SystemSettingsTypedef SystemSettings;                     //ϵͳ��Ϣ
CarInfoTypedef CarInfo;                                   //С����Ϣ
ControlPidTypedef SpeedControlPid = {15.0, 2.0, 0.0};     //�ٶȿ���pid������ʼ��
ControlPidTypedef AngleControlPid = {5.0, 0, 40};         //�Ƕ�pid����
ControlPidTypedef PositionControlPid = {0.03, 0.03, 0.1}; //λ��pid����
CarPulseTypedef CarPulse = {0, 0, 0, 0};                  //��������ֵ
GyroOffsetTypedef GyroOffset = {0.0, 0.0, 0.0};           //��������Ʈ��ʼ������
ICMTypedef icm = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};          // ICM���ݳ�ʼ��
speedTypedef speed = {0.0, 0.0};                          //�ٶȳ�ʼ��
positionTypedef position = {94, 120};                     //λ�ó�ʼ��
coordinateTypedef coordinate = {1, 1};                    //��ʼ����
float dtt = 0.020;                                        //����ʱ��

extern rt_timer_t timer1;
/***********************************************************
 * @brief ����ʱ������ʼ��
 * @param
 * @return
 ***********************************************************/
static inline void CarInfoInit(void)
{
  CarInfo.SpeedSet_x = 0; // x���ٶ�����
  CarInfo.SpeedSet_y = 0; // y���ٶ�����
  CarInfo.SpeedSet_z = 0; // z���ٶ�����

  CarInfo.RealSpeed = 0;   //��ʵ�ٶ�
  CarInfo.AngleSet = 0;    //�Ƕ�����
  CarInfo.PositionSet = 0; //λ������

  CarInfo.pitch = 0.0;
  CarInfo.roll = 0.0;
  CarInfo.yaw = 0.0; //��ʵ�Ƕ�

  CarInfo.delet1 = 0;
  CarInfo.delet2 = 0;
  CarInfo.delet3 = 0;
  CarInfo.delet4 = 0;

  CarInfo.current_angle = 0.0;

  CarInfo.BinaryMethod = 0; //��ֵ������(��ʼ��ֵ�������������䷨��

  CarInfo.BinaryThreshold = 200; //��ֵ����ֵ
  CarInfo.RunDistance = 0;       //С�����о���

  //ģ���
  DotTypedef Dot[20] = {{1, 1}, {7, 8}, {18, 22}, {25, 29}, {13, 34}, {8, 13}, {21, 10}, {10, 6}, {1, 6}, {2, 15}, {10, 10}, {22, 7}, {25, 3}, {29, 20}};
}
/***********************************************************
 * @brief ���ó�ʼ��
 * @param
 * @return
 ***********************************************************/
static inline void SystemSettingsInit(void)
{
  SystemSettings.IsFound_dot = 'T';             //�Ƿ�ʼĿ���Ѱ��
  SystemSettings.IsAiOn = 'F';                  //ʶ��ģʽ�Ƿ��
  SystemSettings.Binary_start = 'T';     //��ֵ��һ����ȡ
  SystemSettings.ImageStatusReportEnable = 'F'; //�Ƿ�����ͼ����ģʽ�ϱ�
  SystemSettings.AiEnable = 'F';                //�Ƿ���ʶ��
  SystemSettings.FuzzyEnable = 'T';             //�Ƿ������Ƕ�ģ������
  SystemSettings.ChangeIEnable = 'F';           //�Ƿ������ٶȱ���ֿ���

  SystemSettings.ApriltagSearchSpeed = 10; //����Apriltag�ٶ�
}
/***********************************************************
 * @brief ������Ϣ��ʼ��
 * @param
 * @return
 ***********************************************************/
void CarInformation_init(void)
{
  /********����ʱ������ʼ����ʼ��*********/
  CarInfoInit();
  /*************���ó�ʼ��**************/
  SystemSettingsInit();
}
/***********************************************************
 * @brief Ӳ���豸��ʼ��
 * @param
 * @return
 ***********************************************************/
int hardware_init(void)
{
  //�����ʼ��
  motor_init();
  //��������ʼ��
  encoder_init();
  //��̬������icm20602��ʼ��
  icm20602_init_spi();
  //����ͷ��ת���ʼ��
  mt9v03x_csi_init();
  // OpenArt��ʼ��
  // openart_mini_init();
  //����ת����ͨ�ų�ʼ��
  seekfree_wireless_init();
  return 0;
}
/***********************************************************
 * @brief �����ʼ��������̻߳�ʱ����ʼ��
 * @param
 * @return
 ***********************************************************/
int thread_init(void)
{
  buzzer_thread_init();  //�������̳߳�ʼ�������ȼ�20��
  display_thread_init(); //��ʾ�̳߳�ʼ��(���ȼ�31)
  server_thread_init();  //�����̳߳�ʼ�������ȼ�11��
  BinaryRenew_thread_init();//��ֵ�������̣߳����ȼ�12��
  return 0;
}
/***********************************************************
 * @brief ��ʱ����ʼ��
 * @param
 * @return
 ***********************************************************/
int timer_init(void)
{
  button_init();     //������ⶨʱ����ʼ��
  timer1_pit_init(); //��ʱ����ʼ��
  return 0;
}
/***********************************************************
 * @brief ϵͳֹͣ
 * @param
 * @return
 ***********************************************************/
void System_stop(void)
{
  rt_timer_stop(timer1);
  MotorStopped();
}