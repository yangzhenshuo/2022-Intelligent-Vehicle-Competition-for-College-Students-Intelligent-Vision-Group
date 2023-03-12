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
#include "ProjectMath.h"


SystemSettingsTypedef SystemSettings;                   //ϵͳ��Ϣ
CarInfoTypedef CarInfo;                                 //С����Ϣ
//ControlPidTypedef SpeedControlPid = {150, 50, 90};{8, 0, 12};    //�ٶȿ���pid������ʼ��
ControlPidTypedef SpeedControlPid = {200,100,200};      //�ٶȿ���pid������ʼ��
ControlPidTypedef AngleControlPid = {10, 0, 12};         //�Ƕ�pid����
ControlPidTypedef PositionPid_y = {0.7, 0, 3};          //y��λ��pid����
ControlPidTypedef PositionPid_x = {0.7, 0, 3};          //x��λ��pid����
ControlPidTypedef CorrectControlPid={0.3, 0, 1};        //����pid����
CarPulseTypedef CarPulse = {0, 0, 0, 0};                //��������ֵ
GyroOffsetTypedef GyroOffset = {0.0, 0.0, 0.0};         //��������Ʈ��ʼ������
ICMTypedef icm = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};        //ICM���ݳ�ʼ��
speedTypedef speed = {0.0, 0.0};                        //�ٶȳ�ʼ��
positionTypedef position = {0, 0};                      //λ���趨ֵ��ʼ��
coordinateTypedef orignal_pos = {0, 0};                 //������λ��ֵ
coordinateTypedef coordinate = {0, 0};                  //����������õ���λ�ã����ڵ����жϣ�
DotTypedef dot[20];                                     //Ŀ������
int rec[4][2];                                          //���ο�
//�¼�
rt_event_t event1;
rt_event_t run_event;
//�����ź���
rt_sem_t server1_sem;
rt_sem_t server2_sem;
rt_sem_t server3_sem;
rt_sem_t server4_sem;
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
		 rt_thread_mdelay(5);
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
	SystemSettings.Is_control = 'T';              //���� ��
	SystemSettings.Is_arrival = 'T';              //���� ��
  SystemSettings.IsFound_dot = 'F';             //Ŀ���Ѱ�� ��
	SystemSettings.Iscorrect = 'F';               //������̬ ��
  SystemSettings.IsAiOn = 'F';                  //ʶ�� ��
	SystemSettings.IsCarry = 'F';                 //���� ��
	
  SystemSettings.Binary_start = 'T';            //��ֵ��ȡ ��  
  SystemSettings.ChangeIEnable = 'F';           //�ٶȱ���ֿ��� ��
	
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
	//�����˶��¼�
	run_event = rt_event_create("event1", RT_IPC_FLAG_FIFO);
	//��������ź���
	server1_sem = rt_sem_create("server1", 0, RT_IPC_FLAG_FIFO);
	server2_sem = rt_sem_create("server2", 0, RT_IPC_FLAG_FIFO);
	server3_sem = rt_sem_create("server3", 0, RT_IPC_FLAG_FIFO);
	server4_sem = rt_sem_create("server4", 0, RT_IPC_FLAG_FIFO);
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
	//�������ʼ��
	gpio_init(DIAN_PIN, GPO, 0, GPIO_PIN_CONFIG);
	//�����ʼ��
	smotor_init();
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
	server3_thread_init();		 //����3�̳߳�ʼ�������ȼ�11��
	server4_thread_init();     //����4�̳߳�ʼ�������ȼ�11��
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
 * @brief ͣ��
 * @param
 * @return
 ***********************************************************/
void stop_car(void)
{
  SystemSettings.Is_control = 'F';              //���� ��
	SystemSettings.Is_arrival = 'T';              //���� ��
  SystemSettings.IsFound_dot = 'F';             //Ŀ���Ѱ�� ��
	SystemSettings.Iscorrect = 'F';               //������̬ ��
  SystemSettings.IsAiOn = 'F';                  //ʶ�� ��
	SystemSettings.IsCarry = 'F';                 //���� ��
	
	MotorStopped();                  //���ֹͣ
}
/***********************************************************
 * @brief ���̧����
 * @param
 * @return
 ***********************************************************/
void smotor_up(void)
{
	smotor1_control(4000);
}
void smotor_down(void)
{
	smotor1_control(1490);
}
void smotor_down1(void)
{
	smotor1_control(1800);
}