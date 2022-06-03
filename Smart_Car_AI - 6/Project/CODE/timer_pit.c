/*
 * timer_pit.c
 * �����ʱ��
 * Author: ����˶
 */
#include "timer_pit.h"
#include "control.h"
#include "System.h"
#include "communicate.h"
#include "ProjectMath.h"
#include "buzzer.h"

rt_timer_t timer1, timer2;
char test[] = "1\n";
char test1[] = "2\n";
char test2[] = "3\n";

void timer1_pit_entry(void *parameter)
{
  // rt_thread_mdelay(10);										// ��ʱ�ȴ�finsh��ʼ�����
  static uint32 time;
  time++;

  if (0 == (time % 5))
  {
    //�ٶȿ��ƣ���������5ms		
		SpeedControl();
    //seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
  }
  if (0 == (time % 20))
  {
    //λ�ÿ�����Ƕȿ��ƣ���������20ms��
		if(SystemSettings.Is_rotation == 'T')
		{
			AngleControl();
		}
		if(SystemSettings.Is_straight == 'T')
		{
			PositionControl();
		}		
  }
	if (0 == time % 100)
	{
		Report_info(); //����λ��������Ϣ
		time = 0;
	}
}

void timer2_pit_entry(void *parameter)
{
	static uint32 time;
	time++;
	//0.5s�ж�һ���Ƿ���ת���
	if(0 == time % 5)
	{
		if(SystemSettings.Is_rotation == 'T')
		{
			if(DetAbs(CarInfo.yaw, CarInfo.AngleSet) <= 1)
			{
				rt_event_send(run_event,(1));
			}
		}
		time = 0;
	}
	if(0 == time % 2)
	{
		if((CarInfo.RunDistance1 / 210) * 5.5 * PI > position.x*200 - 85 && SystemSettings.Is_straight == 'T')
		{
			rt_event_send(run_event, (2));
		}
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
	timer2 =	rt_timer_create("timer2",                //��ʱ������
                           timer2_pit_entry,        //��ʱ�������ָ��
                           RT_NULL,                 //��ڲ���
                           50,                       //��ʱʱ��Ϊ0.05s
                           RT_TIMER_FLAG_PERIODIC); //������
	if (RT_NULL != timer2)
  {
    rt_timer_start(timer2);
  }
	
}

