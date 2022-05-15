/*
 * timer_pit.c
 * �����ʱ��
 * Author: ����˶
 */
#include "encoder.h"
#include "motor.h"
#include "timer_pit.h"
#include "Position.h"
#include "control.h"
#include "image.h"
#include "System.h"

rt_timer_t timer1, timer2;

void timer1_pit_entry(void *parameter)
{
  // rt_thread_mdelay(10);										// ��ʱ�ȴ�finsh��ʼ�����
  static uint32 time;
  time++;

  if (0 == (time % 5) && SystemSettings.Is_control == 'T')
  {
    //�ٶȿ�����Ƕȿ��ƣ���������5ms��
    SpeedControl();
  }
  if (0 == (time % 20) && SystemSettings.Is_control == 'T')
  {
    //λ�ÿ��ƣ���������20ms��
    AngleControl();
    PositionControl();
  }
  // if (0 == (time % 100))
  //{
  //  /*************ÿ100ms���¼���һ����ֵ**************/
  // Binary_threshold(CarInfo.BinaryMethod);
  //}
}

void timer2_pit_entry(void *parameter)
{
  static uint16 time;
  if (0 == time % 100)
  {
    
  }
}

void timer1_pit_init(void)
{
  //������ʱ��1 ��������
  timer1 = rt_timer_create("timer1",                //��ʱ������
                           timer1_pit_entry,        //��ʱ�������ָ��
                           RT_NULL,                 //��ڲ���
                           1,                       //��ʱʱ��Ϊ0.001s
                           RT_TIMER_FLAG_PERIODIC); //������

  //������ʱ��
  if (RT_NULL != timer1)
  {
    rt_timer_start(timer1);
  }
}
void timer2_pit_init(void)
{
  //������ʱ��2 ��������
  timer2 = rt_timer_create("timer2",
                           timer2_pit_entry,
                           RT_NULL,
                           1,
                           RT_TIMER_FLAG_PERIODIC);
  //������ʱ��
  if (RT_NULL != timer2)
  {
    rt_timer_start(timer2);
  }
}
