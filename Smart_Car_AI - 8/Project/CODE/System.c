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

SystemSettingsTypedef SystemSettings;                   //ϵͳ��Ϣ
CarInfoTypedef CarInfo;                                 //С����Ϣ
ControlPidTypedef SpeedControlPid = {15.0,7,0.0};   //�ٶȿ���pid������ʼ��
ControlPidTypedef AngleControlPid = {4.2, 0, 25};       //�Ƕ�pid����
ControlPidTypedef PositionControlPid = {0.15,0,0}; //λ��pid����
ControlPidTypedef CorrectControlPid={0.4,0,3};        //����pid����
CarPulseTypedef CarPulse = {0, 0, 0, 0};                //��������ֵ
GyroOffsetTypedef GyroOffset = {0.0, 0.0, 0.0};         //��������Ʈ��ʼ������
ICMTypedef icm = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};        // ICM���ݳ�ʼ��
speedTypedef speed = {0.0, 0.0};                        //�ٶȳ�ʼ��
positionTypedef position = {0,0};                      //λ���趨ֵ��ʼ��
coordinateTypedef orignal_pos = {1,1};                //��ʼλ��ֵ
coordinateTypedef coordinate = {0, 0};                  //����ǰλ������
DotTypedef dot[20];                                     //Ŀ������
float dtt = 0.020;                                      //����ʱ��
//�¼�
rt_event_t event1;
rt_event_t run_event;
//�����ź���
rt_sem_t server1_sem;
rt_sem_t server2_sem;
rt_sem_t server3_sem;
//�����ź���
rt_sem_t key1_sem;
rt_sem_t key2_sem;
rt_sem_t key3_sem;
rt_sem_t key4_sem;
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

  CarInfo.delet1 = 0.0;
  CarInfo.delet2 = 0.0;
  CarInfo.delet3 = 0.0;
  CarInfo.delet4 = 0.0;

  CarInfo.goal_x = 0;
	CarInfo.goal_y = 0;

  CarInfo.BinaryMethod = 4; //��ֵ������(��ʼ��ֵ�������������䷨��
	CarInfo.white_proportion = 0.0;
  CarInfo.BinaryThreshold = 0; //��ֵ����ֵ
  CarInfo.RunDistance1 = 0.0;    //С�����о���
  CarInfo.RunDistance2 = 0.0;
	CarInfo.dotnum = 0;
  CarInfo.num = 0;
	CarInfo.angle=0.0;       //�����ǶȲ�ֵ
  CarInfo.distance1 = 0.0; //�����y���ֵ
  CarInfo.distance2 = 0.0; //�����x���ֵ
}
/***********************************************************
 * @brief �������������ƫ��
 * @param
 * @return
***********************************************************/
void gyro_offset_init(void)
{
	for (uint16_t i = 0; i < 100; ++i) 
	{
     get_icm20602_gyro_spi();    // ��ȡ�����ǽ��ٶ�
     GyroOffset.x += icm_gyro_x;
     GyroOffset.y += icm_gyro_y;
     GyroOffset.z += icm_gyro_z;
     systick_delay_ms(5);    // ��� 1Khz
   }

   GyroOffset.x /= 100;
   GyroOffset.y /= 100;
   GyroOffset.z /= 100;
}
/***********************************************************
 * @brief ���ó�ʼ��
 * @param
 * @return
 ***********************************************************/
static inline void SystemSettingsInit(void)
{
  SystemSettings.IsFound_dot = 'F';             //�Ƿ�ʼĿ���Ѱ��
  SystemSettings.IsAiOn = 'F';                  //ʶ��ģʽ�Ƿ��
  SystemSettings.Binary_start = 'T';            //��ֵ��һ����ȡ
  SystemSettings.Is_control = 'F';             //�Ƿ�������
  SystemSettings.Complete = 'F'; 
  SystemSettings.Is_arrival = 'T';              //�Ƿ���ʶ��
  SystemSettings.FuzzyEnable = 'T';             //�Ƿ������Ƕ�ģ������
  SystemSettings.ChangeIEnable = 'T';           //�Ƿ������ٶȱ���ֿ���
	SystemSettings.Iscorrect = 'F'; //������̬�Ƿ��
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
 * @brief �¼����ź����������ʼ��
 * @param
 * @return
 ***********************************************************/
int communication_init(void)
{
	//�����ֵ�������¼�
  event1 = rt_event_create("event1", RT_IPC_FLAG_FIFO);
	//����ֲ��˶��¼�
	run_event = rt_event_create("event1", RT_IPC_FLAG_FIFO);
	//��������ź���
	server1_sem = rt_sem_create("server1", 0, RT_IPC_FLAG_FIFO);
	server2_sem = rt_sem_create("server2", 0, RT_IPC_FLAG_FIFO);
	server3_sem = rt_sem_create("server3", 0, RT_IPC_FLAG_FIFO);
	//�����������ź��������������¾��ͷ��ź���������Ҫʹ�ð����ĵط���ȡ�ź�������
	key1_sem = rt_sem_create("key1", 0, RT_IPC_FLAG_FIFO); 
  key2_sem = rt_sem_create("key2", 0, RT_IPC_FLAG_FIFO);
  key3_sem = rt_sem_create("key3", 0, RT_IPC_FLAG_FIFO);
  key4_sem = rt_sem_create("key4", 0, RT_IPC_FLAG_FIFO);
	return 0;
}
/***********************************************************
 * @brief Ӳ���豸��ʼ��
 * @param
 * @return
 ***********************************************************/
int hardware_init(void)
{
  
  //��������ʼ��
  encoder_init();
  //��̬������icm20602��ʼ��
  icm20602_init_spi();
	gyro_offset_init();
  //����ͷ��ת���ʼ��
  mt9v03x_csi_init();
  // OpenArt��ʼ��
  openart_mini_init();
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
	//�����ʼ��
  motor_init();
  buzzer_thread_init();      //�������̳߳�ʼ�������ȼ�14��
  display_thread_init();     //��ʾ�̳߳�ʼ��(���ȼ�17)
  server1_thread_init();     //����1�̳߳�ʼ�������ȼ�11��
  server2_thread_init();     //����2�̳߳�ʼ�������ȼ�11
	server3_thread_init();
  BinaryRenew_thread_init(); //��ֵ�������̣߳����ȼ�12��
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
	//timer2_pit_init();
  return 0;
}
/***********************************************************
 * @brief ���ʼ��
 * @param
 * @return
 ***********************************************************/
void dot_init(void)
{
	dot[0].x = 2;dot[0].y = 2;dot[0].flag = 1;
	dot[1].x = 1;dot[1].y = 8;dot[1].flag = 1;
	dot[2].x = 9;dot[2].y = 9;dot[2].flag = 1;
	dot[3].x = 6;dot[3].y = 12;dot[3].flag = 1;
	dot[4].x = 4;dot[4].y = 16;dot[4].flag = 1;
}