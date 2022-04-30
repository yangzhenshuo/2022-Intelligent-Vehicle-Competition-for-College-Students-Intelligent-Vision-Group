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

SystemSettingsTypedef SystemSettings;//ϵͳ��Ϣ
CarInfoTypedef CarInfo;//С����Ϣ
ControlPidTypedef SpeedControlPid = {9.0,0.5,0.0};//�ٶȿ���pid������ʼ��
ControlPidTypedef AngleControlPid = {0.6,0.1,0};              //�Ƕ�pid����
ControlPidTypedef PositionControlPid = {0.005, 0.005, 0};     //λ��pid����
CarPulseTypedef CarPulse = {0,0,0,0};//��������ֵ
GyroOffsetTypedef GyroOffset = {0.0,0.0,0.0};//��������Ʈ��ʼ������
ICMTypedef icm = {0.0,0.0,0.0,0.0,0.0,0.0};//ICM���ݳ�ʼ��
speedTypedef speed = {0.0,0.0};//�ٶȳ�ʼ��
positionTypedef position = {0.2,0.2};//λ�ó�ʼ����
coordinateTypedef coordinate = {1,1};//��ʼ����
float dtt = 0.001;                                   //����ʱ��
/***********************************************************
 * @brief ����ʱ������ʼ��
 * @param
 * @return
 ***********************************************************/
static inline void CarInfoInit(void)
{
  CarInfo.IsRun = 'F'; //�Ƿ�����
  // CarInfo.IsCameraDetectRun = 'T';     //����ͷ����Ƿ�����
  CarInfo.IsMotorStalled = 'F'; //����Ƿ��ת
  // CarInfo.IsMotorDiffrientialOn = 'T'; //��������Ƿ���
  CarInfo.IsAiOn = 'F'; //ʶ��ģʽ�Ƿ��

  CarInfo.SpeedSet_x = 0;  // x���ٶ�����
  CarInfo.SpeedSet_y = 0;  // y���ٶ�����
  CarInfo.SpeedSet_z = 0;  // z���ٶ�����
  CarInfo.RealSpeed = 0;   //��ʵ�ٶ�
  CarInfo.AngleSet = 0;    //�Ƕ�����
  CarInfo.PositionSet = 0; //λ������

  CarInfo.pitch = 0.0;
  CarInfo.roll = 0.0;
  CarInfo.yaw = 0.0; //��ʵ�Ƕ�

  CarInfo.current_angle = 0.0;

  CarInfo.BinaryThreshold = 70; //��ֵ����ֵ
  CarInfo.RunDistance = 0;      //С�����о���

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
  SystemSettings.PostureReportEnable = 'F';     //�Ƿ�������̬�ϱ�
  SystemSettings.ImageStatusReportEnable = 'F'; //�Ƿ�����ͼ����ģʽ�ϱ�
  SystemSettings.AiEnable = 'F';                //�Ƿ���ʶ��
  SystemSettings.FuzzyEnable = 'T';             //�Ƿ������Ƕ�ģ������
  SystemSettings.ChangeIEnable = 'F';           //�Ƿ������ٶȱ���ֿ���

  SystemSettings.BinaryMethod = 0; //��ֵ������(��ʼ��ֵ�������������䷨��

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
  /*************��ֵ**************/
  Binary_threshold(SystemSettings.BinaryMethod);
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
  //����ͷ��ת���ʼ��
  mt9v03x_csi_init();
  //��̬������icm20602��ʼ��
  icm20602_init_spi();
	
  //��������Ʈ��ʼ��
 
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
int software_init(void)
{
  buzzer_thread_init();  //�������̳߳�ʼ��
  display_thread_init(); //��ʾ�̳߳�ʼ��
  button_init();         //������ⶨʱ����ʼ��
  timer_pit_init();//��ʱ����ʼ��
  // camera_thread_init();
  return 0;
}
