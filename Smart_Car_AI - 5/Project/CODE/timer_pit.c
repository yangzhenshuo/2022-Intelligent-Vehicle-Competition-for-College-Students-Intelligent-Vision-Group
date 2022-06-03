/*
 * timer_pit.c
 * �����ʱ��
 * Author: ����˶
 */
#include "timer_pit.h"
#include "control.h"
#include "System.h"
#include "communicate.h"
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
		if(SystemSettings.IsControl == 'T')
		{
			SpeedControl();
		}
    //seekfree_wireless_send_buff((uint8 *)test, sizeof(test) - 1);
  }
  if (0 == (time % 20))
  {
    //λ�ÿ�����Ƕȿ��ƣ���������20ms��
		if(SystemSettings.IsControl == 'T')
		{
			AngleControl();
		}
		if(SystemSettings.IsControl== 'T' && SystemSettings.Is_straight == 'T')
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

